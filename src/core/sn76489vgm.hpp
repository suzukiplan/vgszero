/**
 * VGM Driver for SN76489
 * License under GPLv3
 */
#pragma once
#include <stdint.h>
#include <string.h>
#include "emu76489.hpp"

#define SN76489_CLOCK 3579545

class SN76489VgmDriver
{
  private:
    const uint8_t* vgm;
    EMU76489* emu;
    size_t vgmSize;
    int vgmCursor;
    int vgmLoopOffset;
    int vgmWait;
    bool vgmEnd;

  public:
    SN76489VgmDriver()
    {
        this->vgm = nullptr;
        this->emu = new EMU76489(SN76489_CLOCK, 44100);
    }

    ~SN76489VgmDriver()
    {
        delete this->emu;
    }

    bool load(const uint8_t* vgm, size_t size)
    {
        this->vgm = nullptr;
        if (size < 0x100) {
            return false;
        }
        if (0 != memcmp("Vgm ", vgm, 4)) {
            return false;
        }

        uint32_t version;
        memcpy(&version, &vgm[0x08], 4);
        if (version < 0x161) {
            return false;
        }

        uint32_t clock;
        memcpy(&clock, &vgm[0x0C], 4);
        if (clock != SN76489_CLOCK) {
            return false;
        }

        // Set stream offset
        memcpy(&this->vgmCursor, &vgm[0x34], 4);
        this->vgmCursor += 0x40 - 0x0C;

        // Set loop offset
        memcpy(&this->vgmLoopOffset, &vgm[0x1C], 4);
        this->vgmLoopOffset += this->vgmLoopOffset ? 0x1C : 0;

        this->vgm = vgm;
        this->vgmSize = size;
        this->vgmEnd = false;
        this->vgmWait = 0;
        this->reset();
        return true;
    }

    void render(int16_t* buf, int samples)
    {
        if (!this->vgm) {
            memset(buf, 0, samples * 2);
            return;
        }
        int cursor = 0;
        while (cursor < samples) {
            if (this->vgmWait < 1) {
                this->vgmexec();
            }
            this->vgmWait--;
            buf[cursor] = emu->calc() << 1;
            cursor++;
        }
    }

  private:
    void reset()
    {
        emu->reset();
    }

    inline void vgmexec()
    {
        if (!this->vgm || this->vgmEnd) {
            return;
        }

        while (this->vgmWait < 1) {
            uint8_t cmd = this->vgm[this->vgmCursor++];
            switch (cmd) {
                case 0x4F:
                    emu->writeGGIO(this->vgm[this->vgmCursor++]);
                    break;
                case 0x50:
                    emu->writeIO(this->vgm[this->vgmCursor++]);
                    break;
                case 0x61: {
                    // Wait nn samples
                    unsigned short nn;
                    memcpy(&nn, &this->vgm[this->vgmCursor], 2);
                    this->vgmCursor += 2;
                    this->vgmWait += nn;
                    break;
                }
                case 0x62: this->vgmWait += 735; break;
                case 0x63: this->vgmWait += 882; break;
                case 0x66: {
                    // End of sound data
                    if (this->vgmLoopOffset) {
                        this->vgmCursor = this->vgmLoopOffset;
                        break;
                    } else {
                        this->vgmEnd = true;
                        return;
                    }
                }

                case 0xDD:
                case 0xDE:
                case 0xDF:
                case 0xFD:
                case 0xFE:
                case 0xFF:
                    // Skip: Furnace outputs thies unsupport commands (use for labels?)
                    break;

                default:
                    // unsupported command
                    this->vgmEnd = true;
                    return;
            }
        }
    }
};
