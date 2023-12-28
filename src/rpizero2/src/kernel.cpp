/**
 * VGS0 for RaspberryPi Baremetal Environment - Kernel implementation
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
#include "kernel.h"
#include "vgs0.hpp"

#define TAG "kernel"
static uint8_t pkg_[16777216];
static UINT pkgSize_;
#define MOUNT_DRIVE "SD:"
#define PKG_FILE "/game.pkg"
#define SAVE_FILE "/save.dat"
extern "C" const unsigned short splash[46080];
VGS0* vgs0_;
uint8_t pad1_;
size_t hdmiPitch_;
uint16_t* hdmiBuffer_;
uint16_t pcmData_[735];
uint8_t saveDataCache_[0x4000];
bool saveDataChanged_;
UINT saveDataSize_;
CLogger* logger_;

CKernel::CKernel(void) : screen(240, 192),
                         timer(&interrupt),
                         logger(LogPanic, nullptr),
                         usb(&interrupt, &timer, TRUE),
                         vchiq(CMemorySystem::Get(), &interrupt),
                         sound(&vchiq, (TVCHIQSoundDestination)options.GetSoundOption()),
                         emmc(&interrupt, &timer, &led),
                         mcm(CMemorySystem::Get()),
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
    hdmiPitch_ = buffer->GetPitch() / sizeof(TScreenColor);
    unsigned long ptr = buffer->GetBuffer();
    hdmiBuffer_ = (uint16_t*)ptr;
    auto bptr = hdmiBuffer_;
    uint16_t* splashPtr = (uint16_t*)splash;
    buffer->WaitForVerticalSync();
    for (int y = 0; y < 192; y++) {
        memcpy(bptr, splashPtr, 240 * 2);
        bptr += hdmiPitch_;
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
        logger.Write(TAG, LogNotice, "init mcm");
        logger_ = &logger;
        bOK = mcm.Initialize();
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
                    pad1_ |= state->axes[0].value == state->axes[0].minimum ? VGS0_JOYPAD_LE : 0;
                    pad1_ |= state->axes[0].value == state->axes[0].maximum ? VGS0_JOYPAD_RI : 0;
                    pad1_ |= state->axes[1].value == state->axes[1].minimum ? VGS0_JOYPAD_UP : 0;
                    pad1_ |= state->axes[1].value == state->axes[1].maximum ? VGS0_JOYPAD_DW : 0;
                    pad1_ |= (state->buttons & 0x0001) ? VGS0_JOYPAD_T2 : 0;
                    pad1_ |= (state->buttons & 0x0002) ? VGS0_JOYPAD_T1 : 0;
                    pad1_ |= (state->buttons & 0x0100) ? VGS0_JOYPAD_SE : 0;
                    pad1_ |= (state->buttons & 0x0200) ? VGS0_JOYPAD_ST : 0;
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
    logger.Write(TAG, LogNotice, "Mounting SD card");
    FRESULT result = f_mount(&fatFs, MOUNT_DRIVE, 1);
    if (FR_OK != result) {
        logger.Write(TAG, LogPanic, "Mount failed! (%d)", (int)result);
        return ShutdownHalt;
    }

    logger.Write(TAG, LogNotice, "Loading game.pkg...");
    FIL gamePkg;
    result = f_open(&gamePkg, MOUNT_DRIVE PKG_FILE, FA_READ | FA_OPEN_EXISTING);
    if (FR_OK != result) {
        logger.Write(TAG, LogPanic, "File not found! (%d)", (int)result);
        return ShutdownHalt;
    }
    result = f_read(&gamePkg, pkg_, sizeof(pkg_), &pkgSize_);
    if (FR_OK != result) {
        logger.Write(TAG, LogPanic, "File read error! (%d)", (int)result);
        return ShutdownHalt;
    }
    logger.Write(TAG, LogNotice, "Load success: %d bytes", (int)pkgSize_);
    f_close(&gamePkg);

    FIL saveDat;
    result = f_open(&saveDat, MOUNT_DRIVE SAVE_FILE, FA_READ | FA_OPEN_EXISTING);
    if (FR_OK == result) {
        memset(saveDataCache_, 0, sizeof(saveDataCache_));
        saveDataChanged_ = false;
        if (FR_OK != f_read(&saveDat, saveDataCache_, sizeof(saveDataCache_), &saveDataSize_)) {
            saveDataSize_ = 0;
        }
        f_close(&saveDat);
    }

    f_unmount(MOUNT_DRIVE);

    void* rom = nullptr;
    void* bgm = nullptr;
    void* se = nullptr;
    int romSize = 0;
    int bgmSize = 0;
    int seSize = 0;
    { // extract package
        uint8_t* ptr = pkg_;
        if (0 != memcmp(ptr, "VGS0PKG", 8)) {
            logger.Write(TAG, LogPanic, "Invalid game.pkg data");
            return ShutdownHalt;
        }
        ptr += 8;
        memcpy(&romSize, ptr, 4);
        if (romSize < 8 + 8192) {
            logger.Write(TAG, LogPanic, "Invalid game.pkg data");
            return ShutdownHalt;
        }
        logger.Write(TAG, LogNotice, "game.rom: %d bytes", romSize);
        ptr += 4;
        rom = ptr;
        ptr += romSize;
        memcpy(&bgmSize, ptr, 4);
        if (bgmSize < 0) {
            logger.Write(TAG, LogPanic, "Invalid game.pkg data");
            return ShutdownHalt;
        }
        logger.Write(TAG, LogNotice, "bgm.dat: %d bytes", bgmSize);
        ptr += 4;
        bgm = ptr;
        ptr += bgmSize;
        memcpy(&seSize, ptr, 4);
        if (seSize < 0) {
            logger.Write(TAG, LogPanic, "Invalid game.pkg data");
            return ShutdownHalt;
        }
        logger.Write(TAG, LogNotice, "se.dat: %d bytes", seSize);
        if (sizeof(pkg_) < (size_t)romSize + bgmSize + seSize) {
            logger.Write(TAG, LogPanic, "Invalid game.pkg data (size too large)");
            return ShutdownHalt;
        }
        ptr += 4;
        se = ptr;
    }

    sound.SetControl(VCHIQ_SOUND_VOLUME_MAX);
    VGS0 vgs0(VDP::ColorMode::RGB565);
    vgs0.loadRom(rom, romSize);
    if (0 < bgmSize) vgs0.loadBgm(bgm, bgmSize);
    if (0 < seSize) vgs0.loadSoundEffect(se, seSize);
    vgs0.setExternalRenderingCallback([](void* arg) {
        CMultiCoreSupport::SendIPI(3, IPI_USER + 2); // request execute rendering core (vdp)
    });
    vgs0.saveCallback = [](VGS0* vgs0, const void* data, size_t size) -> bool {
        // この処理はサブCPUで実行されるのでキャッシュと変更フラグのみ更新して実際のセーブはメイン処理に委ねる
        if (size < sizeof(saveDataCache_)) return false;
        if (saveDataSize_ != size || 0 != memcmp(saveDataCache_, data, size)) {
            memcpy(saveDataCache_, data, size);
            saveDataChanged_ = true;
            saveDataSize_ = size;
        }
        return true;
    };
    vgs0.loadCallback = [](VGS0* vgs0, void* data, size_t size) -> bool {
        // 実際にSDカードからは読み込まずキャッシュから読む
        // NOTE: ゲーム稼働中にSDカードのsave.datを置き換えてゲーム内でロードしても無効
        if (0 < saveDataSize_) {
            memcpy(data, saveDataCache_, size);
            return true;
        } else {
            return false;
        }
    };
    vgs0_ = &vgs0;

    int swap = 0;
    auto buffer = screen.GetFrameBuffer();
    while (1) {
        // update status of the peripheral devices
        updateUsbStatus();

        // request to the another cpus
        CMultiCoreSupport::SendIPI(1, IPI_USER + 0); // request execute main core (z80)
        CMultiCoreSupport::SendIPI(2, IPI_USER + 1); // request execute sound core (vgs)

        // wait v-sync (cpu0 expects synchronize with the another cores)
        buffer->WaitForVerticalSync();

        // flip screen buffer
        swap = 192 - swap;
        buffer->SetVirtualOffset(0, swap);

        // playback sound
        while (sound.PlaybackActive()) scheduler.Sleep(1);
        sound.Playback(pcmData_, 735, 1, 16);

        // save if needed
        if (saveDataChanged_ && 0 < saveDataSize_ && saveDataSize_ < sizeof(saveDataCache_)) {
            saveDataChanged_ = false;
            result = f_mount(&fatFs, MOUNT_DRIVE, 1);
            if (FR_OK != result) {
                logger.Write(TAG, LogError, "Mount failed! (%d)", (int)result);
            } else {
                result = f_open(&saveDat, MOUNT_DRIVE SAVE_FILE, FA_WRITE | FA_CREATE_ALWAYS);
                if (FR_OK != result) {
                    logger.Write(TAG, LogError, "File open failed! (%d)", (int)result);
                } else {
                    UINT wrote;
                    result = f_read(&saveDat, saveDataCache_, saveDataSize_, &wrote);
                    if (FR_OK != result || wrote != saveDataSize_) {
                        logger.Write(TAG, LogError, "File write failed! (%d)", (int)result);
                    }
                    f_close(&saveDat);
                }
                f_unmount(MOUNT_DRIVE);
            }
        }
    }

    return ShutdownHalt;
}