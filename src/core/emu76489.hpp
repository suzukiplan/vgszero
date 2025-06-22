/*
emu76489.hpp -- SN76489 emulator for Sigle Header C++

Original: https://github.com/digital-sound-antiques/emu76489
Copyright (c) 2014 Mitsutaka Okazaki
Modified by Yoji Suzuki

The MIT License (MIT)

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
*/
#pragma once
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

class EMU76489
{
  private:
    const int GETA_BITS = 24;
    const uint32_t voltbl[16] = {0xff, 0xcb, 0xa1, 0x80, 0x65, 0x50, 0x40, 0x33, 0x28, 0x20, 0x19, 0x14, 0x10, 0x0c, 0x0a, 0x00};

    typedef struct {
        int32_t out;
        uint32_t clk, rate, base_incr, quality;
        uint32_t count[3];
        uint32_t volume[3];
        uint32_t freq[3];
        uint32_t edge[3];
        uint32_t mute[3];
        uint32_t noise_seed;
        uint32_t noise_count;
        uint32_t noise_freq;
        uint32_t noise_volume;
        uint32_t noise_mode;
        uint32_t noise_fref;
        uint32_t base_count;
        uint32_t realstep;
        uint32_t sngtime;
        uint32_t sngstep;
        uint32_t adr;
        uint32_t stereo;
        int16_t ch_out[4];
    } Context;
    Context* sng;

  public:
    EMU76489(uint32_t clk, uint32_t rate)
    {
        sng = new Context();
        sng->clk = clk;
        sng->rate = rate ? rate : 44100;
        set_quality(0);
    }

    ~EMU76489()
    {
        delete sng;
    }

    void set_quality(uint32_t quality)
    {
        sng->quality = quality;
        internal_refresh();
    }

    void set_rate(uint32_t rate)
    {
        sng->rate = rate;
        internal_refresh();
    }

    void reset()
    {
        int i;
        sng->base_count = 0;
        for (i = 0; i < 3; i++) {
            sng->count[i] = 0;
            sng->freq[i] = 0;
            sng->edge[i] = 0;
            sng->volume[i] = 0x0f;
            sng->mute[i] = 0;
        }
        sng->adr = 0;
        sng->noise_seed = 0x8000;
        sng->noise_count = 0;
        sng->noise_freq = 0;
        sng->noise_volume = 0x0f;
        sng->noise_mode = 0;
        sng->noise_fref = 0;
        sng->out = 0;
        sng->stereo = 0xFF;
        sng->ch_out[0] = sng->ch_out[1] = sng->ch_out[2] = sng->ch_out[3] = 0;
    }

    void writeIO(uint32_t val)
    {
        if (val & 0x80) {
            sng->adr = (val & 0x70) >> 4;
            switch (sng->adr) {
                case 0:
                case 2:
                case 4:
                    sng->freq[sng->adr >> 1] = (sng->freq[sng->adr >> 1] & 0x3F0) | (val & 0x0F);
                    break;

                case 1:
                case 3:
                case 5:
                    sng->volume[(sng->adr - 1) >> 1] = val & 0xF;
                    break;

                case 6:
                    sng->noise_mode = (val & 4) >> 2;

                    if ((val & 0x03) == 0x03) {
                        sng->noise_freq = sng->freq[2];
                        sng->noise_fref = 1;
                    } else {
                        sng->noise_freq = 32 << (val & 0x03);
                        sng->noise_fref = 0;
                    }

                    if (sng->noise_freq == 0)
                        sng->noise_freq = 1;

                    sng->noise_seed = 0x8000;
                    break;

                case 7:
                    sng->noise_volume = val & 0x0f;
                    break;
            }
        } else {
            sng->freq[sng->adr >> 1] = ((val & 0x3F) << 4) | (sng->freq[sng->adr >> 1] & 0x0F);
        }
    }

    void writeGGIO(uint32_t val)
    {
        sng->stereo = val;
    }

    int16_t calc()
    {
        if (!sng->quality) {
            update_output();
            return mix_output();
        }

        /* Simple rate converter */
        while (sng->realstep > sng->sngtime) {
            sng->sngtime += sng->sngstep;
            update_output();
        }

        sng->sngtime = sng->sngtime - sng->realstep;
        return mix_output();
    }

    void calc_stereo(int32_t out[2])
    {
        if (!sng->quality) {
            update_output();
            mix_output_stereo(out);
            return;
        }

        while (sng->realstep > sng->sngtime) {
            sng->sngtime += sng->sngstep;
            update_output();
        }

        sng->sngtime = sng->sngtime - sng->realstep;
        mix_output_stereo(out);
        return;
    }

  private:
    int parity(int val)
    {
        val ^= val >> 8;
        val ^= val >> 4;
        val ^= val >> 2;
        val ^= val >> 1;
        return val & 1;
    }

    void internal_refresh()
    {
        if (sng->quality) {
            sng->base_incr = 1 << GETA_BITS;
            sng->realstep = (uint32_t)((1 << 31) / sng->rate);
            sng->sngstep = (uint32_t)((1 << 31) / (sng->clk / 16));
            sng->sngtime = 0;
        } else {
            sng->base_incr = (uint32_t)((double)sng->clk * (1 << GETA_BITS) / (16 * sng->rate));
        }
    }

    inline void update_output()
    {
        int i;
        uint32_t incr;

        sng->base_count += sng->base_incr;
        incr = (sng->base_count >> GETA_BITS);
        sng->base_count &= (1 << GETA_BITS) - 1;

        /* Noise */
        sng->noise_count += incr;
        if (sng->noise_count & 0x100) {
            if (sng->noise_mode) /* White */
                sng->noise_seed = (sng->noise_seed >> 1) | (parity(sng->noise_seed & 0x0009) << 15);
            else /* Periodic */
                sng->noise_seed = (sng->noise_seed >> 1) | ((sng->noise_seed & 1) << 15);

            if (sng->noise_fref)
                sng->noise_count -= sng->freq[2];
            else
                sng->noise_count -= sng->noise_freq;
        }

        if (sng->noise_seed & 1) {
            sng->ch_out[3] += voltbl[sng->noise_volume] << 4;
        }
        sng->ch_out[3] >>= 1;

        /* Tone */
        for (i = 0; i < 3; i++) {
            sng->count[i] += incr;
            if (sng->count[i] & 0x400) {
                if (sng->freq[i] > 1) {
                    sng->edge[i] = !sng->edge[i];
                    sng->count[i] -= sng->freq[i];
                } else {
                    sng->edge[i] = 1;
                }
            }

            if (sng->edge[i] && !sng->mute[i]) {
                sng->ch_out[i] += voltbl[sng->volume[i]] << 4;
            }

            sng->ch_out[i] >>= 1;
        }
    }

    inline int16_t mix_output()
    {
        sng->out = sng->ch_out[0] + sng->ch_out[1] + sng->ch_out[2] + sng->ch_out[3];
        return (int16_t)sng->out;
    }

    inline void mix_output_stereo(int32_t out[2])
    {
        int i;

        out[0] = out[1] = 0;
        if ((sng->stereo >> 4) & 0x08) {
            out[0] += sng->ch_out[3];
        }
        if (sng->stereo & 0x08) {
            out[1] += sng->ch_out[3];
        }

        for (i = 0; i < 3; i++) {
            if ((sng->stereo >> (i + 4)) & 0x01) {
                out[0] += sng->ch_out[i];
            }
            if ((sng->stereo >> i) & 0x01) {
                out[1] += sng->ch_out[i];
            }
        }
    }
};
