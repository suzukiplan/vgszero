#include <assert.h>
#include <string.h>
#include "nes_mem.h"

namespace xgm
{
NES_MEM::NES_MEM()
{
    fds_enable = false;
}

NES_MEM::~NES_MEM()
{
}

void NES_MEM::Reset()
{
    memset(image, 0, 0x800);
}

bool NES_MEM::SetImage(uint8_t* data, uint32_t offset, uint32_t size)
{
    memset(image, 0, 0x10000);
    if (offset + size < 0x10000)
        memcpy(image + offset, data, size);
    else
        memcpy(image + offset, data, 0x10000 - offset);
    return true;
}

bool NES_MEM::Write(uint32_t adr, uint32_t val, uint32_t id)
{
    if (0x0000 <= adr && adr < 0x2000) {
        image[adr & 0x7ff] = val;
        return true;
    }
    if (0x6000 <= adr && adr < 0x8000) {
        image[adr] = val;
        return true;
    }
    if (fds_enable && 0x8000 <= adr && adr < 0xe000) {
        image[adr] = val;
    }
    return false;
}

bool NES_MEM::Read(uint32_t adr, uint32_t& val, uint32_t id)
{
    if (0x0000 <= adr && adr < 0x2000) {
        val = image[adr & 0x7ff];
        return true;
    }
    if (PLAYER_RESERVED <= adr && adr < (PLAYER_RESERVED + PLAYER_RESERVED_SIZE)) {
        val = image[adr];
        return true;
    }
    if (0x6000 <= adr && adr < 0x10000) {
        val = image[adr];
        return true;
    }
    val = 0;
    return false;
}

void NES_MEM::SetFDSMode(bool t)
{
    fds_enable = t;
}

void NES_MEM::SetReserved(const uint8_t* data, uint32_t size)
{
    assert(size <= PLAYER_RESERVED_SIZE);
    ::memcpy(image + PLAYER_RESERVED, data, size);
}

bool NES_MEM::WriteReserved(uint32_t adr, uint32_t val)
{
    if (PLAYER_RESERVED <= adr && adr < (PLAYER_RESERVED + PLAYER_RESERVED_SIZE)) {
        image[adr] = val;
        return true;
    }
    return false;
}

} // namespace xgm
