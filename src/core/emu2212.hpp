/*
emu2212.hpp -- emu2212 for Sigle Header C++

Original: https://github.com/digital-sound-antiques/emu2212

The MIT License (MIT)

Copyright (c) 2014 Mitsutaka Okazaki

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.

------------------------------
Modified by Yoji Suzuki
License under GPLv3
*/
#include <stdint.h>

class EMU2212
{
  public:
    enum class Type {
        Standard,
        Enhanced,
    };

  private:
    inline int SCC_MASK_CH(int x) { return (1 << (x)); }
    const int GETA_BITS = 22;

    typedef struct {
        uint32_t clk, rate, base_incr, quality;
        int16_t out;
        Type type;
        uint32_t mode;
        uint32_t active;
        uint32_t base_adr;
        uint32_t mask;

        uint32_t realstep;
        uint32_t scctime;
        uint32_t sccstep;

        uint32_t incr[5];

        int8_t wave[5][32];

        uint32_t count[5];
        uint32_t freq[5];
        uint32_t phase[5];
        uint32_t volume[5];
        uint32_t offset[5];
        uint8_t reg[0x100 - 0xC0];

        int ch_enable;
        int ch_enable_next;

        int cycle_4bit;
        int cycle_8bit;
        int refresh;
        int rotate[5];

        int16_t ch_out[5];
    } Context;

    Context* scc;

  public:
    EMU2212(uint32_t c, uint32_t r)
    {
        scc = new Context();
        scc->clk = c;
        scc->rate = r ? r : 44100;
        set_quality(1);
        scc->type = Type::Enhanced;
    }

    ~EMU2212()
    {
        delete scc;
    }

    void reset()
    {
        int i, j;

        if (scc == NULL)
            return;

        scc->mode = 0;
        scc->active = 0;
        scc->base_adr = 0x9000;

        for (i = 0; i < 5; i++) {
            for (j = 0; j < 5; j++)
                scc->wave[i][j] = 0;
            scc->count[i] = 0;
            scc->freq[i] = 0;
            scc->phase[i] = 0;
            scc->volume[i] = 0;
            scc->offset[i] = 0;
            scc->rotate[i] = 0;
            scc->ch_out[i] = 0;
        }

        memset(scc->reg, 0, 0x100 - 0xC0);

        scc->mask = 0;

        scc->ch_enable = 0xff;
        scc->ch_enable_next = 0xff;

        scc->cycle_4bit = 0;
        scc->cycle_8bit = 0;
        scc->refresh = 0;

        scc->out = 0;

        return;
    }

    void set_rate(uint32_t r)
    {
        scc->rate = r ? r : 44100;
        internal_refresh();
    }

    void set_quality(uint32_t q)
    {
        scc->quality = q;
        internal_refresh();
    }

    void set_type(Type type)
    {
        scc->type = type;
    }

    int16_t calc()
    {
        if (!scc->quality) {
            update_output();
            return mix_output();
        }

        while (scc->realstep > scc->scctime) {
            scc->scctime += scc->sccstep;
            update_output();
        }
        scc->scctime -= scc->realstep;

        return mix_output();
    }

    void write(uint32_t adr, uint32_t val)
    {
        val = val & 0xFF;

        if (scc->type == Type::Enhanced && (adr & 0xFFFE) == 0xBFFE) {
            scc->base_adr = 0x9000 | ((val & 0x20) << 8);
            return;
        }

        if (adr < scc->base_adr) return;
        adr -= scc->base_adr;

        if (adr == 0) {
            if (val == 0x3F) {
                scc->mode = 0;
                scc->active = 1;
            } else if (val & 0x80 && scc->type == Type::Enhanced) {
                scc->mode = 1;
                scc->active = 1;
            } else {
                scc->mode = 0;
                scc->active = 0;
            }
            return;
        }

        if (!scc->active || adr < 0x800 || 0x8FF < adr) return;

        if (scc->type == Type::Standard) {
            write_standard(adr, val);
        } else {
            if (scc->mode)
                write_enhanced(adr, val);
            else
                write_standard(adr, val);
        }
    }

    void writeReg(uint32_t adr, uint32_t val)
    {
        int ch;
        uint32_t freq;

        adr &= 0xFF;

        if (adr < 0xA0) {
            ch = (adr & 0xF0) >> 5;
            if (!scc->rotate[ch]) {
                scc->wave[ch][adr & 0x1F] = (int8_t)val;
                if (scc->mode == 0 && ch == 3)
                    scc->wave[4][adr & 0x1F] = (int8_t)val;
            }
        } else if (0xC0 <= adr && adr <= 0xC9) {
            scc->reg[adr - 0xC0] = val;
            ch = (adr & 0x0F) >> 1;
            if (adr & 1)
                scc->freq[ch] = ((val & 0xF) << 8) | (scc->freq[ch] & 0xFF);
            else
                scc->freq[ch] = (scc->freq[ch] & 0xF00) | (val & 0xFF);

            if (scc->refresh)
                scc->count[ch] = 0;
            freq = scc->freq[ch];
            if (scc->cycle_8bit)
                freq &= 0xFF;
            if (scc->cycle_4bit)
                freq >>= 8;
            if (freq <= 8)
                scc->incr[ch] = 0;
            else
                scc->incr[ch] = scc->base_incr / (freq + 1);
        } else if (0xD0 <= adr && adr <= 0xD4) {
            scc->reg[adr - 0xC0] = val;
            scc->volume[adr & 0x0F] = (uint8_t)(val & 0xF);
        } else if (adr == 0xE0) {
            scc->reg[adr - 0xC0] = val;
            scc->mode = (uint8_t)val & 1;
        } else if (adr == 0xE1) {
            scc->reg[adr - 0xC0] = val;
            scc->ch_enable_next = (uint8_t)val & 0x1F;
        } else if (adr == 0xE2) {
            scc->reg[adr - 0xC0] = val;
            scc->cycle_4bit = val & 1;
            scc->cycle_8bit = val & 2;
            scc->refresh = val & 32;
            if (val & 64)
                for (ch = 0; ch < 5; ch++)
                    scc->rotate[ch] = 0x1F;
            else
                for (ch = 0; ch < 5; ch++)
                    scc->rotate[ch] = 0;
            if (val & 128)
                scc->rotate[3] = scc->rotate[4] = 0x1F;
        }
    }

    uint32_t read(uint32_t adr)
    {
        if (scc->type == Type::Enhanced && (adr & 0xFFFE) == 0xBFFE)
            return (scc->base_adr >> 8) & 0x20;

        if (adr < scc->base_adr) return 0;
        adr -= scc->base_adr;

        if (adr == 0) {
            if (scc->mode)
                return 0x80;
            else
                return 0x3F;
        }
        if (!scc->active || adr < 0x800 || 0x8FF < adr) return 0;
        return scc->type == Type::Standard || !scc->mode ? read_standard(adr) : read_enhanced(adr);
    }

    uint32_t readReg(uint32_t adr)
    {
        if (adr < 0xA0)
            return scc->wave[adr >> 5][adr & 0x1f];
        else if (0xC0 < adr && adr < 0xF0)
            return scc->reg[adr - 0xC0];
        else
            return 0;
    }

    void setMask(uint32_t mask) { scc->mask = mask; }
    void toggleMask(uint32_t mask) { scc->mask ^= mask; }

    // mapper function for VGM format
    inline void write_waveform1(uint32_t adr, uint32_t val) { writeReg(adr & 0x7F, val); }
    inline void write_waveform2(uint32_t adr, uint32_t val) { writeReg((adr & 0x1F) | 0x60, val); }
    inline void write_frequency(uint32_t adr, uint32_t val) { writeReg((adr & 0x0F) | 0xC0, val); }
    inline void write_volume(uint32_t adr, uint32_t val) { writeReg((adr & 0x0F) | 0xD0, val); }
    inline void write_keyoff(uint32_t val) { writeReg(0xE1, val); }
    inline void write_test(uint32_t val) { writeReg(0xE2, val); }

  private:
    void internal_refresh()
    {
        if (scc->quality) {
            scc->base_incr = 2 << GETA_BITS;
            scc->realstep = (uint32_t)((1 << 31) / scc->rate);
            scc->sccstep = (uint32_t)((1 << 31) / (scc->clk / 2));
            scc->scctime = 0;
        } else {
            scc->base_incr = (uint32_t)((double)scc->clk * (1 << GETA_BITS) / scc->rate);
        }
    }

    inline void update_output()
    {
        int i;

        for (i = 0; i < 5; i++) {
            scc->count[i] = (scc->count[i] + scc->incr[i]);

            if (scc->count[i] & (1 << (GETA_BITS + 5))) {
                scc->count[i] &= ((1 << (GETA_BITS + 5)) - 1);
                scc->offset[i] = (scc->offset[i] + 31) & scc->rotate[i];
                scc->ch_enable &= ~(1 << i);
                scc->ch_enable |= scc->ch_enable_next & (1 << i);
            }

            if (scc->ch_enable & (1 << i)) {
                scc->phase[i] = ((scc->count[i] >> (GETA_BITS)) + scc->offset[i]) & 0x1F;
                if (!(scc->mask & SCC_MASK_CH(i)))
                    scc->ch_out[i] += (scc->volume[i] * scc->wave[i][scc->phase[i]]) & 0xfff0;
            }

            scc->ch_out[i] >>= 1;
        }
    }

    inline int16_t mix_output()
    {
        scc->out = scc->ch_out[0] + scc->ch_out[1] + scc->ch_out[2] + scc->ch_out[3] + scc->ch_out[4];
        return (int16_t)scc->out;
    }

    inline void write_standard(uint32_t adr, uint32_t val)
    {
        adr &= 0xFF;

        if (adr < 0x80) /* wave */
        {
            writeReg(adr, val);
        } else if (adr < 0x8A) /* freq */
        {
            writeReg(adr + 0xC0 - 0x80, val);
        } else if (adr < 0x8F) /* volume */
        {
            writeReg(adr + 0xD0 - 0x8A, val);
        } else if (adr == 0x8F) /* ch enable */
        {
            writeReg(0xE1, val);
        } else if (0xE0 <= adr) /* flags */
        {
            writeReg(0xE2, val);
        }
    }

    inline void write_enhanced(uint32_t adr, uint32_t val)
    {
        adr &= 0xFF;

        if (adr < 0xA0) /* wave */
        {
            writeReg(adr, val);
        } else if (adr < 0xAA) /* freq */
        {
            writeReg(adr + 0xC0 - 0xA0, val);
        } else if (adr < 0xAF) /* volume */
        {
            writeReg(adr + 0xD0 - 0xAA, val);
        } else if (adr == 0xAF) /* ch enable */
        {
            writeReg(0xE1, val);
        } else if (0xC0 <= adr && adr <= 0xDF) /* flags */
        {
            writeReg(0xE2, val);
        }
    }

    inline uint32_t read_enhanced(uint32_t adr)
    {
        adr &= 0xFF;
        if (adr < 0xA0)
            return readReg(adr);
        else if (adr < 0xAA)
            return readReg(adr + 0xC0 - 0xA0);
        else if (adr < 0xAF)
            return readReg(adr + 0xD0 - 0xAA);
        else if (adr == 0xAF)
            return readReg(0xE1);
        else if (0xC0 <= adr && adr <= 0xDF)
            return readReg(0xE2);
        else
            return 0;
    }

    inline uint32_t read_standard(uint32_t adr)
    {
        adr &= 0xFF;
        if (adr < 0x80)
            return readReg(adr);
        else if (0xA0 <= adr && adr <= 0xBF)
            return readReg(0x80 + (adr & 0x1F));
        else if (adr < 0x8A)
            return readReg(adr + 0xC0 - 0x80);
        else if (adr < 0x8F)
            return readReg(adr + 0xD0 - 0x8A);
        else if (adr == 0x8F)
            return readReg(0xE1);
        else if (0xE0 <= adr)
            return readReg(0xE2);
        else
            return 0;
    }
};
