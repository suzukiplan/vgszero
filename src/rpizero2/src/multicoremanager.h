/**
 * VGS-Zero for RaspberryPi Baremetal Environment - Multicore Support
 * License under GPLv3: https://github.com/suzukiplan/vgszero/blob/master/LICENSE-VGS0.txt
 * (C)2023, SUZUKI PLAN
 */
#pragma once
#include <stdio.h>
#include <string.h>

#include <circle/actled.h>
#include <circle/devicenameservice.h>
#include <circle/exceptionhandler.h>
#include <circle/interrupt.h>
#include <circle/koptions.h>
#include <circle/logger.h>
#include <circle/multicore.h>
#include <circle/sched/scheduler.h>
#include <circle/screen.h>
#include <circle/serial.h>
#include <circle/sound/soundbasedevice.h>
#include <circle/timer.h>
#include <circle/types.h>
#include <circle/usb/usbgamepad.h>
#include <circle/usb/usbhcidevice.h>

enum class CoreStatus {
    Init,
    Idle,
    Busy,
    Exit,
    Unknown
};

class MultiCoreManager : public CMultiCoreSupport
{
  private:
    CoreStatus coreStatus[CORES];

  public:
    MultiCoreManager(CMemorySystem* pMemorySystem);
    ~MultiCoreManager(void);
    boolean Initialize(void);
    void Run(unsigned nCore) override;
    void IPIHandler(unsigned nCore, unsigned nIPI) override;
};
