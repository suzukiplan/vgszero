#include "multicoremanager.h"
#include "vgs0.hpp"

#include <circle/timer.h>

extern VGS0* vgs0_;
extern uint8_t pad1_;
extern size_t hdmiPitch_;
extern uint16_t* hdmiBuffer_;
extern uint16_t pcmData_[735];
extern CLogger* logger_;
extern CTimer* timer_;

MultiCoreManager::MultiCoreManager(CMemorySystem* pMemorySystem) : CMultiCoreSupport(pMemorySystem)
{
    for (unsigned nCore = 0; nCore < CORES; nCore++) {
        coreStatus[nCore] = CoreStatus::Init;
    }
}

MultiCoreManager::~MultiCoreManager(void)
{
    for (unsigned nCore = 1; nCore < CORES; nCore++) {
        assert(coreStatus[nCore] == CoreStatus::Idle);
        coreStatus[nCore] = CoreStatus::Exit;
        while (coreStatus[nCore] == CoreStatus::Exit) {}
    }
}

boolean MultiCoreManager::Initialize(void)
{
    logger_->Write("MCM", LogNotice, "init multi-core-support");
    if (!CMultiCoreSupport::Initialize()) {
        return FALSE;
    }
    logger_->Write("MCM", LogNotice, "wait for idle...");
    for (unsigned nCore = 1; nCore < 4; nCore++) {
        while (coreStatus[nCore] != CoreStatus::Idle) {
            ; // just wait
        }
    }
    return TRUE;
}

void MultiCoreManager::Run(unsigned nCore)
{
    assert(1 <= nCore && nCore < CORES);
    char buf[80];
    sprintf(buf, "cpu#%u idle", nCore);
    logger_->Write("MCM", LogNotice, buf);
    coreStatus[nCore] = CoreStatus::Idle;
    if (1 <= nCore && nCore < 4) {
        while (1) {
            ; // just wait interrupt
        }
    }
}

void MultiCoreManager::IPIHandler(unsigned nCore, unsigned nIPI)
{
    if (nIPI == IPI_USER + 0) {
        // CPU1: execute main core (z80) tick and render display
        unsigned start = timer_->GetClockTicks();
        vgs0_->tick(pad1_);
        uint16_t* display = vgs0_->getDisplay();
        uint16_t* hdmi = hdmiBuffer_;
        for (int y = 0; y < 192; y++) {
            memcpy(hdmi, display, 240 * 2);
            display += 240;
            hdmi += hdmiPitch_;
        }
        logger_->Write("CPU1", LogNotice, "proctime: %uus", timer_->GetClockTicks() - start);
    } else if (nIPI == IPI_USER + 1) {
        // CPU2: execute sound core (vgs) tick and buffering the result
        memcpy(pcmData_, vgs0_->tickSound(1470), 1470);
    } else if (nIPI == IPI_USER + 2) {
        vgs0_->executeExternalRendering();
    }
}