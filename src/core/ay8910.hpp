/**
 * SUZUKI PLAN - AY-3-8910 Emulator
 * -----------------------------------------------------------------------------
 * The MIT License (MIT)
 *
 * Copyright (c) 2020 Yoji Suzuki.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 * -----------------------------------------------------------------------------
 */

#ifndef INCLUDE_AY8910_HPP
#define INCLUDE_AY8910_HPP

#include <string.h>

class AY8910
{
  private:
    unsigned char regMask[16];
    unsigned int levels[32];

  public:
    struct Context {
        int bobo;
        unsigned char latch;
        unsigned char reg[16];
        unsigned int tPeriod[3];
        int tCounter[3];
        unsigned int tUp[3];
        unsigned int nPeriod;
        int nCounter;
        unsigned int nUp;
        unsigned int ePeriod;
        int eCounter;
        unsigned int eState;
        int eFace;
        unsigned int eHolding;
        unsigned int random;
        int mix[3];
    } ctx;

    void reset(int gain)
    {
        memset(&this->ctx, 0, sizeof(this->ctx));
        this->ctx.ePeriod = 1;
        this->ctx.eFace = 1;
        this->ctx.random = 0xFFFF;
        this->ctx.reg[7] = 0x80;
        this->ctx.reg[14] = 0x7F;
        unsigned char regMask[16] = {0xFF, 0x0F, 0xFF, 0x0F, 0xFF, 0x0F, 0x1F, 0xFF, 0x1F, 0x1F, 0x1F, 0xFF, 0xFF, 0x0F, 0xFF, 0xFF};
        memcpy(this->regMask, regMask, sizeof(this->regMask));
        unsigned int levels[32] = {0, 1, 1, 1, 2, 2, 3, 4, 5, 6, 7, 9, 10, 12, 15, 18, 22, 26, 31, 37, 44, 53, 63, 75, 90, 107, 127, 151, 180, 214, 255, 255};
        memcpy(this->levels, levels, sizeof(this->levels));
        for (int i = 0; i < 32; i++) this->levels[i] *= gain;
    }

    inline void latch(unsigned char value) { this->ctx.latch = value & 0x0F; }
    inline unsigned char read() { return this->read(this->ctx.latch); }
    inline unsigned char read(int n) { return this->ctx.reg[n]; }
    inline unsigned char getPad1() { return this->ctx.reg[0x0E]; }
    inline unsigned char getPad2() { return this->ctx.reg[0x0F]; }

    inline void setPads(unsigned char pad1, unsigned char pad2)
    {
        this->ctx.reg[0x0E] = ~pad1;
        this->ctx.reg[0x0F] = ~pad2;
    }

    inline void write(unsigned char value)
    {
        this->write(this->ctx.latch, value);
    }

    inline void write(int n, unsigned char value)
    {
        this->ctx.reg[n] = value & this->regMask[n];
        switch (n) {
            case 0: this->ctx.tPeriod[0] = (this->ctx.tPeriod[0] & 0xF000) | (value << 4); break;
            case 1: this->ctx.tPeriod[0] = (this->ctx.tPeriod[0] & 0x0FF0) | (value << 12); break;
            case 2: this->ctx.tPeriod[1] = (this->ctx.tPeriod[1] & 0xF000) | (value << 4); break;
            case 3: this->ctx.tPeriod[1] = (this->ctx.tPeriod[1] & 0x0FF0) | (value << 12); break;
            case 4: this->ctx.tPeriod[2] = (this->ctx.tPeriod[2] & 0xF000) | (value << 4); break;
            case 5: this->ctx.tPeriod[2] = (this->ctx.tPeriod[2] & 0x0FF0) | (value << 12); break;
            case 6: this->ctx.nPeriod = value << 4; break;
            case 11:
                this->ctx.ePeriod = this->ctx.ePeriod & 0xFF000;
                this->ctx.ePeriod |= value << 4;
                this->ctx.ePeriod = this->ctx.ePeriod ? this->ctx.ePeriod : 1;
                break;
            case 12:
                this->ctx.ePeriod = this->ctx.ePeriod & 0x00FF0;
                this->ctx.ePeriod |= value << 12;
                this->ctx.ePeriod = this->ctx.ePeriod ? this->ctx.ePeriod : 1;
                break;
            case 13:
                if (value & 0b0100) {
                    this->ctx.eFace = 1;
                    this->ctx.eState = 0;
                } else {
                    this->ctx.eFace = -1;
                    this->ctx.eState = 0x1F;
                }
                this->ctx.eHolding = 1;
                break;
        }
    }

    inline void tick(short* left, short* right, unsigned int cycles)
    {
        if (this->ctx.eHolding) {
            this->ctx.eCounter += cycles;
            while (0 < this->ctx.eCounter) {
                this->ctx.eCounter -= this->ctx.ePeriod;
                this->ctx.eState += this->ctx.eFace;
                if (this->ctx.eState & 0b00100000) {
                    switch (this->ctx.reg[13]) {
                        case 8:
                        case 12:
                            this->ctx.eState = this->ctx.eFace == 1 ? 0 : 0x1F;
                            break;
                        case 10:
                        case 14:
                            this->ctx.eFace = -this->ctx.eFace;
                            this->ctx.eState = this->ctx.eFace == 1 ? 0 : 0x1F;
                            break;
                        case 11:
                        case 15:
                            this->ctx.eState = this->ctx.eFace == 1 ? 0 : 0x1F;
                            this->ctx.eHolding = 1;
                            this->ctx.eCounter = 0;
                            break;
                        case 9:
                        case 13:
                            this->ctx.eFace = -this->ctx.eFace;
                            this->ctx.eState = this->ctx.eFace == 1 ? 0 : 0x1F;
                            this->ctx.eHolding = 1;
                            this->ctx.eCounter = 0;
                            break;
                        default:
                            this->ctx.eState = 0;
                            this->ctx.eHolding = 1;
                            this->ctx.eCounter = 0;
                    }
                }
            }
        }
        if (this->ctx.nPeriod) {
            this->ctx.nCounter += cycles;
            while (0 <= this->ctx.nCounter) {
                this->ctx.nCounter -= this->ctx.nPeriod;
                this->ctx.nUp = this->getRandom();
            }
        } else {
            this->ctx.nUp = this->getRandom();
        }
        int mix = this->getOutputMix(0, this->ctx.reg[7], cycles);
        mix += this->getOutputMix(1, this->ctx.reg[7] >> 1, cycles);
        mix += this->getOutputMix(2, this->ctx.reg[7] >> 2, cycles);
        if (32767 < mix)
            mix = 32767;
        else if (mix < -32768)
            mix = -32768;
        *left = (short)mix;
        *right = *left;
    }

  private:
    inline int getRandom()
    {
        if (this->ctx.random & 1) {
            this->ctx.random ^= 0x24000;
            this->ctx.random >>= 1;
            return 1;
        } else {
            this->ctx.random >>= 1;
            return 0;
        }
    }

    inline int getOutputMix(int ch, unsigned int mask, unsigned int cycles)
    {
        unsigned int volume;
        int mix = this->ctx.mix[ch];
        if (this->ctx.tPeriod[ch]) {
            this->ctx.tCounter[ch] += cycles;
            while (0 <= this->ctx.tCounter[ch]) {
                this->ctx.tCounter[ch] -= this->ctx.tPeriod[ch];
                this->ctx.tUp[ch] ^= 1;
            }
        } else {
            this->ctx.tUp[ch] = 1;
        }
        if (((mask & 0x01) || this->ctx.tUp[ch]) && ((mask & 0x08) || this->ctx.nUp)) {
            volume = this->ctx.reg[8 + ch] << 1;
            this->ctx.mix[ch] = volume & 0x20 ? this->levels[this->ctx.eState] : this->levels[volume & 0x1F];
        } else {
            this->ctx.mix[ch] >>= 1;
        }
        return (mix + this->ctx.mix[ch]) >> 1;
    }
};

#endif // INCLUDE_AY8910_HPP
