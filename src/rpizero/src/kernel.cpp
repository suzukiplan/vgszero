/**
 * FCS80 for RaspberryPi Baremetal Environment - Kernel implementation
 * -----------------------------------------------------------------------------
 * The MIT License (MIT)
 *
 * Copyright (c) 2023 Yoji Suzuki.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 * -----------------------------------------------------------------------------
 */
#include "fcs80.hpp"
#include "kernel.h"

#define TAG "kernel"
static uint8_t pad1_;
static uint8_t rom_[2097152];
static UINT romSize_;
#define MOUNT_DRIVE "SD:"
#define ROM_FILE "/game.rom"
extern "C" const unsigned short splash[46080];

CKernel::CKernel(void) : screen(240, 192),
                         timer(&interrupt),
                         logger(LogPanic, nullptr),
                         usb(&interrupt, &timer, TRUE),
                         vchiq(CMemorySystem::Get(), &interrupt),
                         sound(&vchiq, (TVCHIQSoundDestination)options.GetSoundOption()),
                         emmc(&interrupt, &timer, &led),
                         gamePad(nullptr)
{
    led.Blink(5);
}

CKernel::~CKernel(void)
{
}

boolean CKernel::initialize(void)
{
    boolean bOK = TRUE;
    led.On();

    if (bOK) {
        bOK = screen.Initialize();
    }

    auto buffer = screen.GetFrameBuffer();
    auto hdmiPitch = buffer->GetPitch() / sizeof(TScreenColor);
    unsigned long ptr = buffer->GetBuffer();
    auto hdmiBuffer = (uint16_t*)ptr;
    uint16_t* splashPtr = (uint16_t*)splash;
    buffer->WaitForVerticalSync();
    for (int y = 0; y < 192; y++) {
        memcpy(hdmiBuffer, splashPtr, 240 * 2);
        hdmiBuffer += hdmiPitch;
        splashPtr += 240;
    }

    if (bOK) {
        bOK = serial.Initialize(115200);
    }

    if (bOK) {
        CDevice* target = deviceNameService.GetDevice(options.GetLogDevice(), FALSE);
        if (target == 0) {
            target = &screen;
        }
        bOK = logger.Initialize(target);
    }

    if (bOK) {
        logger.Write(TAG, LogNotice, "init interrupt");
        bOK = interrupt.Initialize();
    }

    if (bOK) {
        logger.Write(TAG, LogNotice, "init timer");
        bOK = timer.Initialize();
    }

    if (bOK) {
        logger.Write(TAG, LogNotice, "init vchiq");
        bOK = vchiq.Initialize();
    }

    if (bOK) {
        logger.Write(TAG, LogNotice, "init usb");
        bOK = usb.Initialize();
    }

    if (bOK) {
        logger.Write(TAG, LogNotice, "init emmc");
        bOK = emmc.Initialize();
    }

    if (bOK) {
        led.Off();
    }
    return bOK;
}

void CKernel::updateUsbStatus(void)
{
    if (usb.UpdatePlugAndPlay()) {
        if (!gamePad) {
            gamePad = (CUSBGamePadDevice*)deviceNameService.GetDevice("upad1", FALSE);
            if (gamePad) {
                gamePad->RegisterStatusHandler([](unsigned index, const TGamePadState* state) {
                    pad1_ = 0;
                    pad1_ |= state->axes[0].value == state->axes[0].minimum ? FCS80_JOYPAD_LE : 0;
                    pad1_ |= state->axes[0].value == state->axes[0].maximum ? FCS80_JOYPAD_RI : 0;
                    pad1_ |= state->axes[1].value == state->axes[1].minimum ? FCS80_JOYPAD_UP : 0;
                    pad1_ |= state->axes[1].value == state->axes[1].maximum ? FCS80_JOYPAD_DW : 0;
                    pad1_ |= (state->buttons & 0x0001) ? FCS80_JOYPAD_T2 : 0;
                    pad1_ |= (state->buttons & 0x0002) ? FCS80_JOYPAD_T1 : 0;
                    pad1_ |= (state->buttons & 0x0100) ? FCS80_JOYPAD_SE : 0;
                    pad1_ |= (state->buttons & 0x0200) ? FCS80_JOYPAD_ST : 0;
                });
            }
        } else {
            if (!(CUSBGamePadDevice*)deviceNameService.GetDevice("upad1", FALSE)) {
                gamePad = nullptr;
                pad1_ = 0;
            }
        }
    }
}

TShutdownMode CKernel::run(void)
{
    logger.Write(TAG, LogNotice, "loading game.rom...");
    FRESULT result = f_mount(&fatFs, MOUNT_DRIVE, 1);
    if (FR_OK != result) {
        logger.Write(TAG, LogPanic, "Mount failed! (%d)", (int)result);
        return ShutdownHalt;
    }
    FIL gameRom;
    result = f_open(&gameRom, MOUNT_DRIVE ROM_FILE, FA_READ | FA_OPEN_EXISTING);
    if (FR_OK != result) {
        logger.Write(TAG, LogPanic, "File not found! (%d)", (int)result);
        return ShutdownHalt;
    }
    result = f_read(&gameRom, rom_, sizeof(rom_), &romSize_);
    if (FR_OK != result) {
        logger.Write(TAG, LogPanic, "File read error! (%d)", (int)result);
        return ShutdownHalt;
    }
    logger.Write(TAG, LogNotice, "Load success: %d bytes", (int)romSize_);
    f_close(&gameRom);
    f_unmount(MOUNT_DRIVE);

    sound.SetControl(VCHIQ_SOUND_VOLUME_MAX);
    auto buffer = screen.GetFrameBuffer();
    auto hdmiPitch = buffer->GetPitch() / sizeof(TScreenColor);
    unsigned long ptr = buffer->GetBuffer();
    auto hdmiBuffer = (uint16_t*)ptr;
    FCS80 fcs80(FCS80Video::ColorMode::RGB565);
    fcs80.loadRom(rom_, romSize_);
    int swap = 0;
    while (1) {
        updateUsbStatus();
        fcs80.tick(pad1_, 0);
        uint16_t* display = fcs80.getDisplay();
        uint16_t* hdmi = hdmiBuffer;
        for (int y = 0; y < 192; y++) {
            memcpy(hdmi, display, 240 * 2);
            display += 240;
            hdmi += hdmiPitch;
        }
        swap = 192 - swap;

        buffer->SetVirtualOffset(0, swap);
        buffer->WaitForVerticalSync();

        // play sound
        size_t pcmSize;
        int16_t* pcmData = (int16_t*)fcs80.dequeSoundBuffer(&pcmSize);
        while (sound.PlaybackActive()) {
            scheduler.Sleep(1);
        }
        sound.Playback(pcmData, pcmSize / 4, 2, 16);
    }

    return ShutdownHalt;
}