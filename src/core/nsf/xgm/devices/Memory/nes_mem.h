#ifndef _NES_RAM_H_
#define _NES_RAM_H_
#include "../device.h"

namespace xgm
{
const uint32_t PLAYER_RESERVED = 0x4100;
const uint32_t PLAYER_RESERVED_SIZE = 0x0020;

class NES_MEM : public IDevice
{
  protected:
    uint8_t image[0x10000];
    bool fds_enable;

  public:
    NES_MEM();
    ~NES_MEM();
    void Reset();
    bool Read(uint32_t adr, uint32_t& val, uint32_t id = 0);
    bool Write(uint32_t adr, uint32_t val, uint32_t id = 0);
    bool SetImage(uint8_t* data, uint32_t offset, uint32_t size);
    void SetFDSMode(bool); // enables writing to $6000-DFFF for FDS
    void SetReserved(const uint8_t* data, uint32_t size);
    bool WriteReserved(uint32_t adr, uint32_t val);
};

} // namespace xgm

#endif
