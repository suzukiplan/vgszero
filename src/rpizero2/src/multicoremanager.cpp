/**
 * VGS-Zero for RaspberryPi Baremetal Environment - Multicore Support
 * License under GPLv3: https://github.com/suzukiplan/vgszero/blob/master/LICENSE-VGS0.txt
 * (C)2023, SUZUKI PLAN
 */
#include "multicoremanager.h"
#include "vgs0.hpp"

extern VGS0* vgs0_;
extern uint8_t pad1_;
extern size_t hdmiPitch_;
extern uint16_t* hdmiBuffer_;
extern uint16_t pcmData_[735];
extern CLogger* logger_;

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
        vgs0_->tick(pad1_);
    } else if (nIPI == IPI_USER + 1) {
        // CPU2: execute sound core (vgs) tick and buffering the result
        memcpy(pcmData_, vgs0_->tickSound(1470), 1470);
    } else if (nIPI == IPI_USER + 2) {
        // CPU3: execute VDP's rendering 1 scanline procedure
        if (vgs0_->executeExternalRendering()) {
            CMultiCoreSupport::SendIPI(2, IPI_USER + 3); // request execute mailbox buffering (vgs)
        }
    } else if (nIPI == IPI_USER + 3) {
        // CPU2: copy to the mailbox buffer
        uint16_t* display = vgs0_->getDisplay();
        uint16_t* hdmi = hdmiBuffer_;
        for (int y = 0; y < 192; y++) {
            for (int x = 0; x < 240; x++) {
                auto col = *display;
                display++;
                hdmi[x * 2] = col;
                hdmi[x * 2 + 1] = col & 0b1110011100011100; 
                hdmi[hdmiPitch_ + x * 2] = col & 0b1001110011110011;
                hdmi[hdmiPitch_ + x * 2 + 1] = col & 0b1000010000010000;
            }
            hdmi += hdmiPitch_ * 2;
        }
    }
}