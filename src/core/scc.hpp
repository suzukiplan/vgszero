/**
 * FAIRY COMPUTER SYSTEM 80 - SOUND CREATIVE CHIP Emulator
 * -----------------------------------------------------------------------------
 * The MIT License (MIT)
 *
 * Copyright (c) 2023 Yoji Suzuki.
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
#ifndef INCLUDE_SCC_HPP
#define INCLUDE_SCC_HPP
#include <string.h>

class SCC
{
  public:
    struct Channel {
        signed char waveforms[32];
        int counter;
        unsigned short period;
        unsigned char volume;
        unsigned char index;
    };

    struct Context {
        struct Channel ch[5];
        int sw;
    } ctx;

    SCC()
    {
        this->reset();
    }

    void reset() { memset(&this->ctx, 0, sizeof(this->ctx)); }

    inline unsigned char read(unsigned short addr)
    {
        addr &= 0xFF;
        return addr < 0x80 ? this->ctx.ch[addr / 0x20].waveforms[addr & 0x1F] : 0xFF;
    }

    inline void write(unsigned short addr, unsigned char value)
    {
        addr &= 0xFF;
        if (addr & 0x80) {
            addr &= 0x3F;
            if (addr < 0x0A) {
                Channel* ch = &this->ctx.ch[addr >> 1];
                ch->period = addr & 0x01 ? (ch->period & 0xff) | ((value & 0x0F) << 8) : (ch->period & 0xf00) | value;
            } else if (addr < 0x0F) {
                this->ctx.ch[addr - 0x0A].volume = value & 0x0F;
            } else if (0x0F == addr) {
                this->ctx.sw = value & 0x1F;
            }
        } else {
            this->ctx.ch[addr >> 5].waveforms[addr & 0x1F] = (signed char)value;
        }
    }

    inline void tick(short* left, short* right, unsigned int cycles)
    {
        int mix[5];
        int sw = this->ctx.sw;
        for (int i = 0; i < 5; i++) {
            Channel* ch = &this->ctx.ch[i];
            if (ch->period) {
                ch->counter += cycles;
                while (0 <= ch->counter) {
                    ch->counter -= ch->period;
                    ch->index++;
                    ch->index &= 0x1F;
                }
            } else {
                ch->index++;
                ch->index &= 0x1F;
            }
            mix[i] = sw & 1 ? this->ctx.ch[4 == i ? 3 : i].waveforms[ch->index] * ch->volume : 0;
            sw >>= 1;
        }
        int result = mix[0] + mix[1] + mix[2] + mix[3] + mix[4];
        *left = this->to_short((*left) + result);
        *right = this->to_short((*right) + result);
    }

  private:
    inline short to_short(int i)
    {
        if (32767 < i) return (short)32767;
        if (i < -32768) return (short)-32768;
        return (short)i;
    }
};

#endif /* INCLUDE_SCC_HPP */
