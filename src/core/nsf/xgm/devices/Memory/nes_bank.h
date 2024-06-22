#ifndef _NES_BANK_H_
#define _NES_BANK_H_
#include "../device.h"
namespace xgm
{

class NES_BANK : public IDevice
{
  protected:
    uint8_t null_bank[0x1000];
    uint8_t* bank[256];
    uint8_t* image;
    int bankswitch[16];
    int bankdefault[16];
    bool fds_enable;
    int bankmax;

  public:
    NES_BANK();
    ~NES_BANK();
    void Reset();
    bool Read(uint32_t adr, uint32_t& val, uint32_t id = 0);
    bool Write(uint32_t adr, uint32_t val, uint32_t id = 0);
    void SetBankDefault(uint8_t bank, int value);
    bool SetImage(uint8_t* data, uint32_t offset, uint32_t size);
    void SetFDSMode(bool); // enables banks 6/7 for FDS
};

} // namespace xgm

#endif
