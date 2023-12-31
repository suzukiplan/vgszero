/**
 * VGS-Zero for RaspberryPi Baremetal Environment - Joypad Input Checker
 * License under GPLv3: https://github.com/suzukiplan/vgszero/blob/master/LICENSE-VGS0.txt
 * (C)2023, SUZUKI PLAN
 */
#include "kernel.h"
#include <string.h>

#define TAG "kernel"
static TGamePadState padState_;

CKernel::CKernel(void) : screen(640, 480),
                         timer(&interrupt),
                         logger(LogNotice, nullptr),
                         usb(&interrupt, &timer, TRUE),
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
        logger.Write(TAG, LogNotice, "init usb");
        bOK = usb.Initialize();
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
                    memcpy(&padState_, state, sizeof(TGamePadState));
                });
            }
        } else {
            if (!(CUSBGamePadDevice*)deviceNameService.GetDevice("upad1", FALSE)) {
                gamePad = nullptr;
            }
        }
    }
}

TShutdownMode CKernel::run(void)
{
    auto buffer = screen.GetFrameBuffer();
    bool joypadConnected = false;
    unsigned previousButtons = 0;
    int previousAxisValue[MAX_AXIS];
    memset(previousAxisValue, 0, sizeof(previousAxisValue));
    while (1) {
        updateUsbStatus();
        buffer->WaitForVerticalSync();
        if (gamePad && !joypadConnected) {
            logger.Write(TAG, LogNotice, "USB joypad connected");
            previousButtons = 0;
            for (int i = 0; i < MAX_AXIS; i++) {
                previousAxisValue[i] = padState_.axes[i].value;
            }
            joypadConnected = true;
        } else if (!gamePad && joypadConnected) {
            logger.Write(TAG, LogNotice, "USB joypad disconnected");
            joypadConnected = false;
        }
        if (!joypadConnected) {
            continue;
        }
        if (previousButtons != padState_.buttons) {
            previousButtons = padState_.buttons;
            unsigned mask = 1;
            for (int i = 0; i < 32; i++) {
                if (previousButtons & mask) {
                    logger.Write(TAG, LogNotice, "Push BUTTON_%d detected", i);
                }
                mask <<= 1;
            }
        }
        for (int i = 0; i < MAX_AXIS; i++) {
            if (previousAxisValue[i] != padState_.axes[i].value) {
                previousAxisValue[i] = padState_.axes[i].value;
                logger.Write(TAG, LogNotice, "Changed AXIS_%d = %d", i, previousAxisValue[i]);
            }
        }
    }
    return ShutdownHalt;
}