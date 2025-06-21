// SN76489 Emulator + VGM Driver
// License under GPLv3
#pragma once
#include <stdint.h>
#include <string.h>

class SN76489
{
  private:
    uint32_t clock;
    uint8_t levels[16];
    const uint8_t* vgm;
    size_t vgmSize;
    int vgmCursor;
    int vgmLoopOffset;
    int vgmWait;
    bool vgmEnd;

  public:
    struct Context {
        int i;
        uint32_t r[8];
        uint32_t c[4];
        uint32_t e[4];
        uint32_t np;
        uint32_t ns;
        uint32_t nx;
    } ctx;

    SN76489()
    {
        this->vgm = nullptr;
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
        memcpy(&this->clock, &vgm[0x0C], 4);
        if (!this->clock) {
            return false;
        }
        uint32_t version;
        memcpy(&version, &vgm[0x08], 4);
        if (version < 0x151) {
            this->vgm = nullptr;
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
        this->reset(this->clock, 44100);
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
            buf[cursor++] = tick();
        }
    }

  private:
    void reset(int cpuRate, int sampleRate)
    {
        memset(&ctx, 0, sizeof(ctx));
        this->clock = cpuRate / ((double)sampleRate) * (1 << 16);
        uint8_t levels[16] = {255, 180, 127, 90, 63, 44, 31, 22, 15, 10, 7, 5, 3, 2, 1, 0};
        memcpy(this->levels, &levels, sizeof(levels));
    }

    inline void write(uint8_t value)
    {
        if (value & 0x80) {
            this->ctx.i = (value >> 4) & 7;
            this->ctx.r[this->ctx.i] = value & 0x0f;
        } else {
            this->ctx.r[this->ctx.i] |= (value & 0x3f) << 4;
        }
        switch (this->ctx.r[6] & 3) {
            case 0: this->ctx.np = 1; break;
            case 1: this->ctx.np = 2; break;
            case 2: this->ctx.np = 4; break;
            case 3: this->ctx.np = this->ctx.r[4]; break;
        }
        this->ctx.nx = (this->ctx.r[6] & 0x04) ? 0x12000 : 0x08000;
    }

    inline int16_t tick()
    {
        for (int i = 0; i < 3; i++) {
            int regidx = i << 1;
            if (this->ctx.r[regidx]) {
                uint32_t cc = this->clock + this->ctx.c[i];
                while ((cc & 0x80000000) == 0) {
                    cc -= (this->ctx.r[regidx] << (16 + 4));
                    this->ctx.e[i] ^= 1;
                }
                this->ctx.c[i] = cc;
            } else {
                this->ctx.e[i] = 1;
            }
        }
        if (this->ctx.np) {
            uint32_t cc = this->clock + this->ctx.c[3];
            while ((cc & 0x80000000) == 0) {
                cc -= (this->ctx.np << (16 + 4));
                this->ctx.ns >>= 1;
                if (this->ctx.ns & 1) {
                    this->ctx.ns = this->ctx.ns ^ this->ctx.nx;
                    this->ctx.e[3] = 1;
                } else {
                    this->ctx.e[3] = 0;
                }
            }
            this->ctx.c[3] = cc;
        }
        int w = 0;
        if (this->ctx.e[0]) w += this->levels[this->ctx.r[1]];
        if (this->ctx.e[1]) w += this->levels[this->ctx.r[3]];
        if (this->ctx.e[2]) w += this->levels[this->ctx.r[5]];
        if (this->ctx.e[3]) w += this->levels[this->ctx.r[7]];
        w <<= 4;
        if (32767 < w) {
            w = 32767;
        } else if (w < -32768) {
            w = -32768;
        }
        return (int16_t)w;
    }

    inline void vgmexec()
    {
        if (!this->vgm || this->vgmEnd) {
            return;
        }

        while (this->vgmWait < 1) {
            uint8_t cmd = this->vgm[this->vgmCursor++];
            switch (cmd) {
                case 0x4F: {
                    // Game Gear PSG stereo, write dd to port 0x06
                    // NOTE: ignore this command because support mono only.
                    break;
                }
                case 0x50: {
                    // PSG (SN76489/SN76496) write value dd
                    this->write(this->vgm[this->vgmCursor++]);
                    break;
                }
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
                    } else {
                        this->vgmEnd = true;
                        return;
                    }
                }
                default:
                    // unsupported command
                    this->vgmEnd = true;
                    return;
            }
        }
    }
};
