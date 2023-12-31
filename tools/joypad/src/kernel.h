/**
 * VGS-Zero for RaspberryPi Baremetal Environment - Joypad Input Checker
 * License under GPLv3: https://github.com/suzukiplan/vgszero/blob/master/LICENSE-VGS0.txt
 * (C)2023, SUZUKI PLAN
 */
#pragma once
#include <circle/actled.h>
#include <circle/devicenameservice.h>
#include <circle/exceptionhandler.h>
#include <circle/interrupt.h>
#include <circle/koptions.h>
#include <circle/logger.h>
#include <circle/sched/scheduler.h>
#include <circle/screen.h>
#include <circle/serial.h>
#include <circle/timer.h>
#include <circle/types.h>
#include <circle/usb/usbgamepad.h>
#include <circle/usb/usbhcidevice.h>

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
    CUSBGamePadDevice* volatile gamePad;
    void updateUsbStatus(void);
};
