#ifndef _NSF2_IRQ_H_
#define _NSF2_IRQ_H_
#include "../device.h"

namespace xgm
{

class NES_CPU; // forward declaration

class NSF2_IRQ : public IDevice
{
  protected:
    uint16_t reload;
    uint32_t count;
    bool active;
    bool irq;
    NES_CPU* cpu;

  public:
    NSF2_IRQ();
    ~NSF2_IRQ();
    void Reset();
    bool Read(uint32_t adr, uint32_t& val, uint32_t id = 0);
    bool Write(uint32_t adr, uint32_t val, uint32_t id = 0);

    void Clock(uint32_t clocks);
    void SetCPU(NES_CPU* cpu_);
};

} // namespace xgm
#endif