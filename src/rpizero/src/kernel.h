/**
 * FCS80 for RaspberryPi Baremetal Environment - Kernel definition
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
#pragma once
#include <SDCard/emmc.h>
#include <circle/actled.h>
#include <circle/devicenameservice.h>
#include <circle/exceptionhandler.h>
#include <circle/interrupt.h>
#include <circle/koptions.h>
#include <circle/logger.h>
#include <circle/sched/scheduler.h>
#include <circle/screen.h>
#include <circle/serial.h>
#include <circle/sound/soundbasedevice.h>
#include <circle/timer.h>
#include <circle/types.h>
#include <circle/usb/usbgamepad.h>
#include <circle/usb/usbhcidevice.h>
#include <fatfs/ff.h>
#include <vc4/sound/vchiqsoundbasedevice.h>
#include <vc4/sound/vchiqsounddevice.h>
#include <vc4/vchiq/vchiqdevice.h>

enum TShutdownMode {
    ShutdownNone,
    ShutdownHalt,
    ShutdownReboot
};

class CKernel
{
  public:
    CKernel(void);
    ~CKernel(void);

    boolean initialize(void);
    TShutdownMode run(void);

  private:
    // do not change this order
    CActLED led;
    CKernelOptions options;
    CDeviceNameService deviceNameService;
    CScreenDevice screen;
    CSerialDevice serial;
    CExceptionHandler exceptionHandler;
    CInterruptSystem interrupt;
    CTimer timer;
    CLogger logger;
    CUSBHCIDevice usb;
    CScheduler scheduler;
    CVCHIQDevice vchiq;
    CVCHIQSoundDevice sound;
    CEMMCDevice emmc;
    FATFS fatFs;
    CUSBGamePadDevice* volatile gamePad;
    void updateUsbStatus(void);
};
