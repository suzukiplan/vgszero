#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "nes_bank.h"

// this workaround solves a problem with mirrored FDS RAM writes
// when the same bank is used twice; some NSF rips reuse bank 00
// in "unused" banks that actually get written to.
// it is preferred to fix the NSFs and leave this disabled.
#define FDS_MEMCPY 0

// for detecting mirrored writes in FDS NSFs
#define DETECT_FDS_MIRROR 0

#if FDS_MEMCPY
static uint8_t* fds_image = NULL;
#endif

namespace xgm
{

NES_BANK::NES_BANK()
{
    image = NULL;
    fds_enable = false;
};

NES_BANK::~NES_BANK()
{
    if (image)
        delete[] image;

#if FDS_MEMCPY
    if (fds_image)
        delete[] fds_image;
#endif
}

void NES_BANK::SetBankDefault(uint8_t bank, int value)
{
    bankdefault[bank] = value;
}

bool NES_BANK::SetImage(uint8_t* data, uint32_t offset, uint32_t size)
{
    int i;

    for (i = 0; i < 16; i++) {
        bankdefault[i] = -1; // -1 is special empty bank
    }
    int total_size = ((offset & 0xfff) + size);
    bankmax = (total_size >> 12);         // count of full banks
    if (total_size & 0xfff) bankmax += 1; // include last partial bank
    if (bankmax > 256) bankmax = 256;
    uint32_t image_size = 0x1000 * bankmax;

    if (image)
        delete[] image;
    image = new uint8_t[image_size];
    memset(image, 0, image_size);

    uint32_t start = offset & 0xfff;
    if ((start + size) > image_size)
        size = image_size;
    memcpy(image + (offset & 0xfff), data, size);

    for (i = 0; i < bankmax; i++) {
        bank[i] = image + 0x1000 * i;
    }
    for (i = bankmax; i < 256; i++) {
        bank[i] = null_bank;
    }

    return true;
}

void NES_BANK::Reset()
{
    memset(null_bank, 0, 0x1000);
    for (int i = 0; i < 16; i++) {
        bankswitch[i] = bankdefault[i];
    }
}

bool NES_BANK::Write(uint32_t adr, uint32_t val, uint32_t id)
{
    if (0x5ff8 <= adr && adr < 0x6000) {
        bankswitch[(adr & 7) + 8] = val & 0xff;
        return true;
    }

    if (fds_enable) {
        if (0x5ff6 <= adr && adr < 0x5ff8) {
            bankswitch[adr & 7] = val & 0xff;
            return true;
        }
        if (0 <= bankswitch[adr >> 12] && 0x6000 <= adr && adr < 0xe000) {
            bank[bankswitch[adr >> 12]][adr & 0x0fff] = val;
            return true;
        }
    }
    return false;
}

bool NES_BANK::Read(uint32_t adr, uint32_t& val, uint32_t id)
{
    if (0x5ff8 <= adr && adr < 0x5fff) {
        val = bankswitch[(adr & 7) + 8];
        return true;
    }

    if (0 <= bankswitch[adr >> 12] && 0x8000 <= adr && adr < 0x10000) {
        val = bank[bankswitch[adr >> 12]][adr & 0xfff];
        return true;
    }

    if (fds_enable) {
        if (0x5ff6 <= adr && adr < 0x5ff8) {
            val = bankswitch[adr & 7];
            return true;
        }

        if (0 <= bankswitch[adr >> 12] && 0x6000 <= adr && adr < 0x8000) {
            val = bank[bankswitch[adr >> 12]][adr & 0xfff];
            return true;
        }
    }

    return false;
}

void NES_BANK::SetFDSMode(bool t)
{
    fds_enable = t;
}

} // namespace xgm
