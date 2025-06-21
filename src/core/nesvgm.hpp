// NES APU Emulator for VGM (modified from nsfplay)
// This code is a NES APU emulator that supports VGM playback with a basic configuration of waveform memory sound source (2 channels), triangle wave (1 channel), and noise (1 channel).
// DMC-related audio that depends on CPU emulation may not be played back correctly. (Not tested)
//
// nsfplay: https://github.com/bbbradsmith/nsfplay
// Original auther: Mamiya (mamiya@proc.org.tohoku.ac.jp)
// Modified by: Yoji Suzuki (SUZUKI PLAN)
// License under GPLv3
//
// NOTE: This source code is based on the following source codes:
// - https://github.com/bbbradsmith/nsfplay/blob/master/xgm/devices/device.h
// - https://github.com/bbbradsmith/nsfplay/blob/master/xgm/devices/Audio/amplifier.h
// - https://github.com/bbbradsmith/nsfplay/blob/master/xgm/devices/Audio/mixer.h
// - https://github.com/bbbradsmith/nsfplay/blob/master/xgm/devices/Audio/rconv.cpp
// - https://github.com/bbbradsmith/nsfplay/blob/master/xgm/devices/Audio/rconv.h
// - https://github.com/bbbradsmith/nsfplay/blob/master/xgm/devices/Sound/nes_apu.cpp
// - https://github.com/bbbradsmith/nsfplay/blob/master/xgm/devices/Sound/nes_apu.h
// - https://github.com/bbbradsmith/nsfplay/blob/master/xgm/devices/Sound/nes_dmc.cpp
// - https://github.com/bbbradsmith/nsfplay/blob/master/xgm/devices/Sound/nes_dmc.h
#pragma once
#include <stdio.h>
#include <string.h>
#include <stdint.h>

extern "C" {
extern const unsigned short vgs0_rand16[65536];
extern const short rom_tndtable[65536];
};

namespace xgm
{

// https://github.com/bbbradsmith/nsfplay/blob/master/xgm/devices/device.h
const int DEFAULT_CLOCK = 1789772;
const int DEFAULT_RATE = 48000;
const int PRECISION = 16;

class IDevice
{
  public:
    virtual void Reset() = 0;
    virtual bool Write(uint32_t adr, uint32_t val, uint32_t id = 0) = 0;
    virtual bool Read(uint32_t adr, uint32_t& val, uint32_t id = 0) = 0;
    virtual void SetOption(int id, int val) {};
    virtual ~IDevice() {};
};

class IRenderable
{
  public:
    virtual uint32_t Render(int32_t b[2]) = 0;
    virtual void Skip() {}
    virtual void Tick(uint32_t clocks) {}
    virtual ~IRenderable() {};
};

class ISoundChip : public IDevice, virtual public IRenderable
{
  public:
    virtual void Tick(uint32_t clocks) = 0;
    virtual void SetClock(long clock) = 0;
    virtual void SetRate(long rate) = 0;
    virtual void SetMask(int mask) = 0;
    virtual void SetStereoMix(int trk, int16_t mixl, int16_t mixr) = 0;
    virtual ~ISoundChip() {};
};

class Bus : public IDevice
{
  protected:
    IDevice* vd_ptr[256];
    int vd_num;

  public:
    void Reset()
    {
        for (int i = 0; i < vd_num; i++) {
            vd_ptr[i]->Reset();
        }
    }

    void DetachAll()
    {
        vd_num = 0;
    }

    void Attach(IDevice* d)
    {
        vd_ptr[vd_num++] = d;
    }

    bool Write(uint32_t adr, uint32_t val, uint32_t id = 0)
    {
        bool ret = false;
        for (int i = 0; i < vd_num; i++) {
            vd_ptr[i]->Write(adr, val);
        }
        return ret;
    }

    bool Read(uint32_t adr, uint32_t& val, uint32_t id = 0)
    {
        bool ret = false;
        uint32_t vtmp = 0;
        val = 0;
        for (int i = 0; i < vd_num; i++) {
            if (vd_ptr[i]->Read(adr, vtmp)) {
                val |= vtmp;
                ret = true;
            }
        }
        return ret;
    }
};

class Layer : public Bus
{
  public:
    bool Write(uint32_t adr, uint32_t val, uint32_t id = 0)
    {
        for (int i = 0; i < vd_num; i++) {
            if (vd_ptr[i]->Write(adr, val)) {
                return true;
            }
        }
        return false;
    }

    bool Read(uint32_t adr, uint32_t& val, uint32_t id = 0)
    {
        val = 0;
        for (int i = 0; i < vd_num; i++) {
            if (vd_ptr[i]->Read(adr, val)) {
                return true;
            }
        }
        return false;
    }
};

// https://github.com/bbbradsmith/nsfplay/blob/master/xgm/devices/Audio/amplifier.h
class Amplifier : virtual public IRenderable
{
  protected:
    IRenderable* target;
    int mute, volume;
    int combo;

  public:
    Amplifier()
    {
        target = NULL;
        mute = false;
        volume = 64;
    }

    ~Amplifier() {}
    void Attach(IRenderable* p) { target = p; }
    void Tick(uint32_t clocks) { target->Tick(clocks); }

    uint32_t Render(int32_t b[2])
    {
        if (mute) {
            b[0] = b[1] = 0;
            return 2;
        }
        target->Render(b);
        b[0] = (b[0] * volume) / 16;
        b[1] = (b[1] * volume) / 16;
        return 2;
    }

    void SetVolume(int v) { volume = v; }
    int GetVolume() { return volume; }
    void SetMute(int m) { mute = m; }
    int GetMute() { return mute; }
};

// https://github.com/bbbradsmith/nsfplay/blob/master/xgm/devices/Audio/mixer.h
class Mixer : virtual public IRenderable
{
  protected:
    IRenderable* dlist[256];
    int dlist_num;

  public:
    Mixer()
    {
        Reset();
    }

    ~Mixer()
    {
    }

    void DetachAll()
    {
        dlist_num = 0;
    }

    void Attach(IRenderable* dev)
    {
        dlist[dlist_num++] = dev;
    }

    void Reset()
    {
    }

    virtual void Skip()
    {
        // Should pass Skip on to all attached devices, but nothing currently requires it.

        // DeviceList::iterator it;
        // for (it = dlist.begin (); it != dlist.end (); it++)
        //{
        //   (*it)->Skip ();
        // }
    }

    virtual void Tick(uint32_t clocks)
    {
        for (int i = 0; i < dlist_num; i++) {
            dlist[i]->Tick(clocks);
        }
    }

    virtual uint32_t Render(int32_t b[2])
    {
        int32_t tmp[2];

        b[0] = b[1] = 0;

        for (int i = 0; i < dlist_num; i++) {
            dlist[i]->Render(tmp);
            b[0] += tmp[0];
            b[1] += tmp[1];
        }

        return 2;
    }
};

// https://github.com/bbbradsmith/nsfplay/blob/master/xgm/devices/Sound/nes_apu.cpp
// https://github.com/bbbradsmith/nsfplay/blob/master/xgm/devices/Sound/nes_apu.h
class NES_APU : public ISoundChip
{
  public:
    enum {
        OPT_UNMUTE_ON_RESET = 0,
        OPT_PHASE_REFRESH,
        OPT_NONLINEAR_MIXER,
        OPT_DUTY_SWAP,
        OPT_NEGATE_SWEEP_INIT,
        OPT_END
    };

    enum { SQR0_MASK = 1,
           SQR1_MASK = 2,
    };

  protected:
    int option[OPT_END]; // �e��I�v�V����
    int mask;
    int32_t sm[2][2];

    uint32_t gclock;
    uint8_t reg[0x20];
    int32_t out[2];
    long rate, clock;

    int32_t square_table[32]; // nonlinear mixer
    int32_t square_linear;    // linear mix approximation

    int scounter[2]; // frequency divider
    int sphase[2];   // phase counter

    int duty[2];
    int volume[2];
    int freq[2];
    int sfreq[2];

    bool sweep_enable[2];
    bool sweep_mode[2];
    bool sweep_write[2];
    int sweep_div_period[2];
    int sweep_div[2];
    int sweep_amount[2];

    bool envelope_disable[2];
    bool envelope_loop[2];
    bool envelope_write[2];
    int envelope_div_period[2];
    int envelope_div[2];
    int envelope_counter[2];

    int length_counter[2];

    bool enable[2];

    // calculates target sweep frequency
    void sweep_sqr(int ch)
    {
        int shifted = freq[ch] >> sweep_amount[ch];
        if (ch == 0 && sweep_mode[ch]) shifted += 1;
        sfreq[ch] = freq[ch] + (sweep_mode[ch] ? -shifted : shifted);
    }

    int32_t calc_sqr(int ch, uint32_t clocks)
    {
        static const int16_t sqrtbl[4][16] = {
            {0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
            {0, 0, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
            {0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0},
            {1, 1, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1}};

        scounter[ch] -= clocks;
        while (scounter[ch] < 0) {
            sphase[ch] = (sphase[ch] + 1) & 15;
            scounter[ch] += freq[ch] + 1;
        }

        int32_t ret = 0;
        if (length_counter[ch] > 0 &&
            freq[ch] >= 8 &&
            sfreq[ch] < 0x800) {
            int v = envelope_disable[ch] ? volume[ch] : envelope_counter[ch];
            ret = sqrtbl[duty[ch]][sphase[ch]] ? v : 0;
        }
        return ret;
    }

  public:
    NES_APU()
    {
        SetClock(DEFAULT_CLOCK);
        SetRate(DEFAULT_RATE);
        option[OPT_UNMUTE_ON_RESET] = true;
        option[OPT_PHASE_REFRESH] = true;
        option[OPT_NONLINEAR_MIXER] = true;
        option[OPT_DUTY_SWAP] = false;
        option[OPT_NEGATE_SWEEP_INIT] = false;

        square_table[0] = 0;
        for (int i = 1; i < 32; i++)
            square_table[i] = (int32_t)((8192.0 * 95.88) / (8128.0 / i + 100));

        square_linear = square_table[15]; // match linear scale to one full volume square of nonlinear

        for (int c = 0; c < 2; ++c)
            for (int t = 0; t < 2; ++t)
                sm[c][t] = 128;
    }

    ~NES_APU() {}

    void FrameSequence(int s)
    {
        if (s > 3) return; // no operation in step 4

        // 240hz clock
        for (int i = 0; i < 2; ++i) {
            bool divider = false;
            if (envelope_write[i]) {
                envelope_write[i] = false;
                envelope_counter[i] = 15;
                envelope_div[i] = 0;
            } else {
                ++envelope_div[i];
                if (envelope_div[i] > envelope_div_period[i]) {
                    divider = true;
                    envelope_div[i] = 0;
                }
            }
            if (divider) {
                if (envelope_loop[i] && envelope_counter[i] == 0)
                    envelope_counter[i] = 15;
                else if (envelope_counter[i] > 0)
                    --envelope_counter[i];
            }
        }

        // 120hz clock
        if ((s & 1) == 0)
            for (int i = 0; i < 2; ++i) {
                if (!envelope_loop[i] && (length_counter[i] > 0))
                    --length_counter[i];

                if (sweep_enable[i]) {
                    --sweep_div[i];
                    if (sweep_div[i] <= 0) {
                        sweep_sqr(i);                                                // calculate new sweep target
                        if (freq[i] >= 8 && sfreq[i] < 0x800 && sweep_amount[i] > 0) // update frequency if appropriate
                        {
                            freq[i] = sfreq[i] < 0 ? 0 : sfreq[i];
                        }
                        sweep_div[i] = sweep_div_period[i] + 1;
                    }

                    if (sweep_write[i]) {
                        sweep_div[i] = sweep_div_period[i] + 1;
                        sweep_write[i] = false;
                    }
                }
            }
    }

    virtual void Reset()
    {
        int i;
        gclock = 0;
        mask = 0;

        for (int i = 0; i < 2; ++i) {
            scounter[i] = 0;
            sphase[i] = 0;
            duty[i] = 0;
            volume[i] = 0;
            freq[i] = 0;
            sfreq[i] = 0;
            sweep_enable[i] = 0;
            sweep_mode[i] = 0;
            sweep_write[i] = 0;
            sweep_div_period[i] = 0;
            sweep_div[i] = 1;
            sweep_amount[i] = 0;
            envelope_disable[i] = 0;
            envelope_loop[i] = 0;
            envelope_write[i] = 0;
            envelope_div_period[i] = 0;
            envelope_div[0] = 0;
            envelope_counter[i] = 0;
            length_counter[i] = 0;
            enable[i] = 0;
        }

        for (i = 0x4000; i < 0x4008; i++)
            Write(i, 0);

        Write(0x4015, 0);
        if (option[OPT_UNMUTE_ON_RESET])
            Write(0x4015, 0x0f);
        if (option[OPT_NEGATE_SWEEP_INIT]) {
            Write(0x4001, 0x08);
            Write(0x4005, 0x08);
        }

        for (i = 0; i < 2; i++)
            out[i] = 0;

        SetRate(rate);
    }

    virtual void Tick(uint32_t clocks)
    {
        out[0] = calc_sqr(0, clocks);
        out[1] = calc_sqr(1, clocks);
    }

    virtual uint32_t Render(int32_t b[2])
    {
        out[0] = (mask & 1) ? 0 : out[0];
        out[1] = (mask & 2) ? 0 : out[1];

        int32_t m[2];

        if (option[OPT_NONLINEAR_MIXER]) {
            int32_t voltage = square_table[out[0] + out[1]];
            m[0] = out[0] << 6;
            m[1] = out[1] << 6;
            int32_t ref = m[0] + m[1];
            if (ref > 0) {
                m[0] = (m[0] * voltage) / ref;
                m[1] = (m[1] * voltage) / ref;
            } else {
                m[0] = voltage;
                m[1] = voltage;
            }
        } else {
            m[0] = (out[0] * square_linear) / 15;
            m[1] = (out[1] * square_linear) / 15;
        }

        b[0] = m[0] * sm[0][0];
        b[0] += m[1] * sm[0][1];
        b[0] >>= 7;

        b[1] = m[0] * sm[1][0];
        b[1] += m[1] * sm[1][1];
        b[1] >>= 7;

        return 2;
    }

    virtual bool Read(uint32_t adr, uint32_t& val, uint32_t id = 0)
    {
        if (0x4000 <= adr && adr < 0x4008) {
            val |= reg[adr & 0x7];
            return true;
        } else if (adr == 0x4015) {
            val |= (length_counter[1] ? 2 : 0) | (length_counter[0] ? 1 : 0);
            return true;
        } else
            return false;
    }

    virtual bool Write(uint32_t adr, uint32_t val, uint32_t id = 0)
    {
        int ch;

        static const uint8_t length_table[32] = {
            0x0A, 0xFE,
            0x14, 0x02,
            0x28, 0x04,
            0x50, 0x06,
            0xA0, 0x08,
            0x3C, 0x0A,
            0x0E, 0x0C,
            0x1A, 0x0E,
            0x0C, 0x10,
            0x18, 0x12,
            0x30, 0x14,
            0x60, 0x16,
            0xC0, 0x18,
            0x48, 0x1A,
            0x10, 0x1C,
            0x20, 0x1E};

        if (0x4000 <= adr && adr < 0x4008) {
            adr &= 0xf;
            ch = adr >> 2;
            switch (adr) {
                case 0x0:
                case 0x4:
                    volume[ch] = val & 15;
                    envelope_disable[ch] = (val >> 4) & 1;
                    envelope_loop[ch] = (val >> 5) & 1;
                    envelope_div_period[ch] = (val & 15);
                    duty[ch] = (val >> 6) & 3;
                    if (option[OPT_DUTY_SWAP]) {
                        if (duty[ch] == 1)
                            duty[ch] = 2;
                        else if (duty[ch] == 2)
                            duty[ch] = 1;
                    }
                    break;

                case 0x1:
                case 0x5:
                    sweep_enable[ch] = (val >> 7) & 1;
                    sweep_div_period[ch] = (((val >> 4) & 7));
                    sweep_mode[ch] = (val >> 3) & 1;
                    sweep_amount[ch] = val & 7;
                    sweep_write[ch] = true;
                    sweep_sqr(ch);
                    break;

                case 0x2:
                case 0x6:
                    freq[ch] = val | (freq[ch] & 0x700);
                    sweep_sqr(ch);
                    break;

                case 0x3:
                case 0x7:
                    freq[ch] = (freq[ch] & 0xFF) | ((val & 0x7) << 8);
                    if (option[OPT_PHASE_REFRESH])
                        sphase[ch] = 0;
                    envelope_write[ch] = true;
                    if (enable[ch]) {
                        length_counter[ch] = length_table[(val >> 3) & 0x1f];
                    }
                    sweep_sqr(ch);
                    break;

                default:
                    return false;
            }
            reg[adr] = val;
            return true;
        } else if (adr == 0x4015) {
            enable[0] = (val & 1) ? true : false;
            enable[1] = (val & 2) ? true : false;

            if (!enable[0])
                length_counter[0] = 0;
            if (!enable[1])
                length_counter[1] = 0;

            reg[adr - 0x4000] = val;
            return true;
        }

        // 4017 is handled in nes_dmc.cpp
        // else if (adr == 0x4017)
        //{
        //}

        return false;
    }

    virtual void SetRate(long rate) { this->rate = rate; }
    virtual void SetClock(long clock) { this->clock = clock; }
    virtual void SetMask(int m) { mask = m; }

    virtual void SetOption(int id, int val)
    {
        if (id < OPT_END) {
            option[id] = val;
        }
    }

    virtual void SetStereoMix(int trk, int16_t mixl, int16_t mixr)
    {
        if (trk < 0) return;
        if (trk > 1) return;
        sm[0][trk] = mixl;
        sm[1][trk] = mixr;
    }
};

// https://github.com/bbbradsmith/nsfplay/blob/master/xgm/devices/Sound/nes_dmc.cpp
// https://github.com/bbbradsmith/nsfplay/blob/master/xgm/devices/Sound/nes_dmc.h
class NES_DMC : public ISoundChip
{
  public:
    enum {
        OPT_ENABLE_4011 = 0,
        OPT_ENABLE_PNOISE,
        OPT_UNMUTE_ON_RESET,
        OPT_DPCM_ANTI_CLICK,
        OPT_NONLINEAR_MIXER,
        OPT_RANDOMIZE_NOISE,
        OPT_TRI_MUTE,
        OPT_RANDOMIZE_TRI,
        OPT_DPCM_REVERSE,
        OPT_END
    };

  protected:
    const int GETA_BITS;

    const uint32_t wavlen_table[2][16] = {
        {// NTSC
         4, 8, 16, 32, 64, 96, 128, 160, 202, 254, 380, 508, 762, 1016, 2034, 4068},
        {// PAL
         4, 8, 14, 30, 60, 88, 118, 148, 188, 236, 354, 472, 708, 944, 1890, 3778}};

    const uint32_t freq_table[2][16] = {
        {// NTSC
         428, 380, 340, 320, 286, 254, 226, 214, 190, 160, 142, 128, 106, 84, 72, 54},
        {// PAL
         398, 354, 316, 298, 276, 236, 210, 198, 176, 148, 132, 118, 98, 78, 66, 50}};

    const uint32_t BITREVERSE[256] = {
        0x00,
        0x80,
        0x40,
        0xC0,
        0x20,
        0xA0,
        0x60,
        0xE0,
        0x10,
        0x90,
        0x50,
        0xD0,
        0x30,
        0xB0,
        0x70,
        0xF0,
        0x08,
        0x88,
        0x48,
        0xC8,
        0x28,
        0xA8,
        0x68,
        0xE8,
        0x18,
        0x98,
        0x58,
        0xD8,
        0x38,
        0xB8,
        0x78,
        0xF8,
        0x04,
        0x84,
        0x44,
        0xC4,
        0x24,
        0xA4,
        0x64,
        0xE4,
        0x14,
        0x94,
        0x54,
        0xD4,
        0x34,
        0xB4,
        0x74,
        0xF4,
        0x0C,
        0x8C,
        0x4C,
        0xCC,
        0x2C,
        0xAC,
        0x6C,
        0xEC,
        0x1C,
        0x9C,
        0x5C,
        0xDC,
        0x3C,
        0xBC,
        0x7C,
        0xFC,
        0x02,
        0x82,
        0x42,
        0xC2,
        0x22,
        0xA2,
        0x62,
        0xE2,
        0x12,
        0x92,
        0x52,
        0xD2,
        0x32,
        0xB2,
        0x72,
        0xF2,
        0x0A,
        0x8A,
        0x4A,
        0xCA,
        0x2A,
        0xAA,
        0x6A,
        0xEA,
        0x1A,
        0x9A,
        0x5A,
        0xDA,
        0x3A,
        0xBA,
        0x7A,
        0xFA,
        0x06,
        0x86,
        0x46,
        0xC6,
        0x26,
        0xA6,
        0x66,
        0xE6,
        0x16,
        0x96,
        0x56,
        0xD6,
        0x36,
        0xB6,
        0x76,
        0xF6,
        0x0E,
        0x8E,
        0x4E,
        0xCE,
        0x2E,
        0xAE,
        0x6E,
        0xEE,
        0x1E,
        0x9E,
        0x5E,
        0xDE,
        0x3E,
        0xBE,
        0x7E,
        0xFE,
        0x01,
        0x81,
        0x41,
        0xC1,
        0x21,
        0xA1,
        0x61,
        0xE1,
        0x11,
        0x91,
        0x51,
        0xD1,
        0x31,
        0xB1,
        0x71,
        0xF1,
        0x09,
        0x89,
        0x49,
        0xC9,
        0x29,
        0xA9,
        0x69,
        0xE9,
        0x19,
        0x99,
        0x59,
        0xD9,
        0x39,
        0xB9,
        0x79,
        0xF9,
        0x05,
        0x85,
        0x45,
        0xC5,
        0x25,
        0xA5,
        0x65,
        0xE5,
        0x15,
        0x95,
        0x55,
        0xD5,
        0x35,
        0xB5,
        0x75,
        0xF5,
        0x0D,
        0x8D,
        0x4D,
        0xCD,
        0x2D,
        0xAD,
        0x6D,
        0xED,
        0x1D,
        0x9D,
        0x5D,
        0xDD,
        0x3D,
        0xBD,
        0x7D,
        0xFD,
        0x03,
        0x83,
        0x43,
        0xC3,
        0x23,
        0xA3,
        0x63,
        0xE3,
        0x13,
        0x93,
        0x53,
        0xD3,
        0x33,
        0xB3,
        0x73,
        0xF3,
        0x0B,
        0x8B,
        0x4B,
        0xCB,
        0x2B,
        0xAB,
        0x6B,
        0xEB,
        0x1B,
        0x9B,
        0x5B,
        0xDB,
        0x3B,
        0xBB,
        0x7B,
        0xFB,
        0x07,
        0x87,
        0x47,
        0xC7,
        0x27,
        0xA7,
        0x67,
        0xE7,
        0x17,
        0x97,
        0x57,
        0xD7,
        0x37,
        0xB7,
        0x77,
        0xF7,
        0x0F,
        0x8F,
        0x4F,
        0xCF,
        0x2F,
        0xAF,
        0x6F,
        0xEF,
        0x1F,
        0x9F,
        0x5F,
        0xDF,
        0x3F,
        0xBF,
        0x7F,
        0xFF,
    };
    uint16_t tnd_table[2][16][16][128];

    int option[OPT_END];
    int mask;
    int32_t sm[2][3];
    uint8_t reg[0x10];
    uint32_t len_reg;
    uint32_t adr_reg;
    IDevice* memory;
    uint32_t out[3];
    uint32_t daddress;
    uint32_t dlength;
    uint32_t data;
    bool empty;
    int16_t damp;
    int dac_lsb;
    bool dmc_pop;
    int32_t dmc_pop_offset;
    int32_t dmc_pop_follow;
    long clock;
    uint32_t rate;
    int pal;
    int mode;
    bool irq;

    int32_t counter[3]; // frequency dividers
    int tphase;         // triangle phase
    uint32_t nfreq;     // noise frequency
    uint32_t dfreq;     // DPCM frequency

    uint32_t tri_freq;
    int linear_counter;
    int linear_counter_reload;
    bool linear_counter_halt;
    bool linear_counter_control;

    int noise_volume;
    uint32_t noise, noise_tap;

    // noise envelope
    bool envelope_loop;
    bool envelope_disable;
    bool envelope_write;
    int envelope_div_period;
    int envelope_div;
    int envelope_counter;

    bool enable[2];        // tri/noise enable
    int length_counter[2]; // 0=tri, 1=noise

    // frame sequencer
    NES_APU* apu;              // apu is clocked by DMC's frame sequencer
    int frame_sequence_count;  // current cycle count
    int frame_sequence_length; // CPU cycles per FrameSequence
    int frame_sequence_step;   // current step of frame sequence
    int frame_sequence_steps;  // 4/5 steps per frame
    bool frame_irq;
    bool frame_irq_enable;

    // NES_CPU* cpu; // IRQ needs CPU access

    inline uint32_t calc_tri(uint32_t clocks)
    {
        static uint32_t tritbl[32] =
            {
                15,
                14,
                13,
                12,
                11,
                10,
                9,
                8,
                7,
                6,
                5,
                4,
                3,
                2,
                1,
                0,
                0,
                1,
                2,
                3,
                4,
                5,
                6,
                7,
                8,
                9,
                10,
                11,
                12,
                13,
                14,
                15,
            };

        if (linear_counter > 0 && length_counter[0] > 0 && (!option[OPT_TRI_MUTE] || tri_freq > 0)) {
            counter[0] -= clocks;
            while (counter[0] < 0) {
                tphase = (tphase + 1) & 31;
                counter[0] += (tri_freq + 1);
            }
        }

        uint32_t ret = tritbl[tphase];
        return ret;
    }

    inline uint32_t calc_dmc(uint32_t clocks)
    {
        counter[2] -= clocks;
        while (counter[2] < 0) {
            counter[2] += dfreq;

            if (data > 0x100) // data = 0x100 when shift register is empty
            {
                if (!empty) {
                    if ((data & 1) && (damp < 63))
                        damp++;
                    else if (!(data & 1) && (0 < damp))
                        damp--;
                }
                data >>= 1;
            }

            if (data <= 0x100) // shift register is empty
            {
                if (dlength > 0) {
                    memory->Read(daddress, data);
                    // cpu->StealCycles(4); // DMC read takes 3 or 4 CPU cycles, usually 4
                    //  (checking for the 3-cycle case would require sub-instruction emulation)
                    data &= 0xFF; // read 8 bits
                    if (option[OPT_DPCM_REVERSE]) data = BITREVERSE[data];
                    data |= 0x10000; // use an extra bit to signal end of data
                    empty = false;
                    daddress = ((daddress + 1) & 0xFFFF) | 0x8000;
                    --dlength;
                    if (dlength == 0) {
                        if (mode & 1) // looped DPCM = auto-reload
                        {
                            daddress = ((adr_reg << 6) | 0xC000);
                            dlength = (len_reg << 4) + 1;
                        } else if (mode & 2) // IRQ and not looped
                        {
                            irq = true;
                            // cpu->UpdateIRQ(NES_CPU::IRQD_DMC, true);
                        }
                    }
                } else {
                    data = 0x10000; // DMC will do nothing
                    empty = true;
                }
            }
        }

        return (damp << 1) + dac_lsb;
    }

    inline uint32_t calc_noise(uint32_t clocks)
    {
        uint32_t env = envelope_disable ? noise_volume : envelope_counter;
        if (length_counter[1] < 1) env = 0;

        uint32_t last = (noise & 0x4000) ? 0 : env;
        if (clocks < 1) return last;

        // simple anti-aliasing (noise requires it, even when oversampling is off)
        uint32_t count = 0;
        uint32_t accum = counter[1] * last; // samples pending from previous calc
        uint32_t accum_clocks = counter[1];
        if (counter[1] < 0) // only happens on startup when using the randomize noise option
        {
            accum = 0;
            accum_clocks = 0;
        }

        counter[1] -= clocks;
        while (counter[1] < 0) {
            // tick the noise generator
            uint32_t feedback = (noise & 1) ^ ((noise & noise_tap) ? 1 : 0);
            noise = (noise >> 1) | (feedback << 14);

            last = (noise & 0x4000) ? 0 : env;
            accum += (last * nfreq);
            counter[1] += nfreq;
            ++count;
            accum_clocks += nfreq;
        }

        if (count < 1) // no change over interval, don't anti-alias
        {
            return last;
        }

        accum -= (last * counter[1]); // remove these samples which belong in the next calc
        accum_clocks -= counter[1];
        uint32_t average = accum / accum_clocks;
        return average;
    }

  public:
    NES_DMC() : GETA_BITS(20)
    {
        SetClock(DEFAULT_CLOCK);
        SetRate(DEFAULT_RATE);
        SetPal(false);
        option[OPT_ENABLE_4011] = 1;
        option[OPT_ENABLE_PNOISE] = 1;
        option[OPT_UNMUTE_ON_RESET] = 1;
        option[OPT_DPCM_ANTI_CLICK] = 0;
        option[OPT_NONLINEAR_MIXER] = 1;
        option[OPT_RANDOMIZE_NOISE] = 1;
        option[OPT_RANDOMIZE_TRI] = 1;
        option[OPT_TRI_MUTE] = 1;
        option[OPT_DPCM_REVERSE] = 0;
        tnd_table[0][0][0][0] = 0;
        tnd_table[1][0][0][0] = 0;

        apu = NULL;
        frame_sequence_count = 0;
        frame_sequence_length = 7458;
        frame_sequence_steps = 4;

        for (int c = 0; c < 2; ++c)
            for (int t = 0; t < 3; ++t)
                sm[c][t] = 128;
    }

    ~NES_DMC() {}

    void InitializeTNDTable(int wt, int wn, int wd) { memcpy(tnd_table, rom_tndtable, sizeof(tnd_table)); }

    void SetPal(bool is_pal)
    {
        pal = (is_pal ? 1 : 0);
        // set CPU cycles in frame_sequence
        frame_sequence_length = is_pal ? 8314 : 7458;
    }

    void SetAPU(NES_APU* apu_) { this->apu = apu_; }
    void SetMemory(IDevice* r) { this->memory = r; }
    int GetDamp() { return (damp << 1) | dac_lsb; }

    void FrameSequence(int s)
    {
        if (s > 3) return; // no operation in step 4

        if (apu) {
            apu->FrameSequence(s);
        }

        if (s == 0 && (frame_sequence_steps == 4)) {
            if (frame_irq_enable) frame_irq = true;
            // cpu->UpdateIRQ(NES_CPU::IRQD_FRAME, frame_irq & frame_irq_enable);
        }

        // 240hz clock
        {
            // triangle linear counter
            if (linear_counter_halt) {
                linear_counter = linear_counter_reload;
            } else {
                if (linear_counter > 0) --linear_counter;
            }
            if (!linear_counter_control) {
                linear_counter_halt = false;
            }

            // noise envelope
            bool divider = false;
            if (envelope_write) {
                envelope_write = false;
                envelope_counter = 15;
                envelope_div = 0;
            } else {
                ++envelope_div;
                if (envelope_div > envelope_div_period) {
                    divider = true;
                    envelope_div = 0;
                }
            }
            if (divider) {
                if (envelope_loop && envelope_counter == 0)
                    envelope_counter = 15;
                else if (envelope_counter > 0)
                    --envelope_counter;
            }
        }

        // 120hz clock
        if ((s & 1) == 0) {
            // triangle length counter
            if (!linear_counter_control && (length_counter[0] > 0))
                --length_counter[0];

            // noise length counter
            if (!envelope_loop && (length_counter[1] > 0))
                --length_counter[1];
        }
    }

    void TickFrameSequence(uint32_t clocks)
    {
        frame_sequence_count += clocks;
        while (frame_sequence_count > frame_sequence_length) {
            FrameSequence(frame_sequence_step);
            frame_sequence_count -= frame_sequence_length;
            ++frame_sequence_step;
            if (frame_sequence_step >= frame_sequence_steps)
                frame_sequence_step = 0;
        }
    }

    virtual void Reset()
    {
        int i;
        mask = 0;

        InitializeTNDTable(8227, 12241, 22638);

        counter[0] = 0;
        counter[1] = 0;
        counter[2] = 0;
        tphase = 0;
        nfreq = wavlen_table[0][0];
        dfreq = freq_table[0][0];
        tri_freq = 0;
        linear_counter = 0;
        linear_counter_reload = 0;
        linear_counter_halt = 0;
        linear_counter_control = 0;
        noise_volume = 0;
        noise = 0;
        noise_tap = 0;
        envelope_loop = 0;
        envelope_disable = 0;
        envelope_write = 0;
        envelope_div_period = 0;
        envelope_div = 0;
        envelope_counter = 0;
        enable[0] = 0;
        enable[1] = 0;
        length_counter[0] = 0;
        length_counter[1] = 0;
        frame_irq = false;
        frame_irq_enable = false;
        frame_sequence_count = 0;
        frame_sequence_steps = 4;
        frame_sequence_step = 0;
        // cpu->UpdateIRQ(NES_CPU::IRQD_FRAME, false);

        for (i = 0; i < 0x0F; i++)
            Write(0x4008 + i, 0);
        Write(0x4017, 0x40);

        irq = false;
        Write(0x4015, 0x00);
        if (option[OPT_UNMUTE_ON_RESET])
            Write(0x4015, 0x0f);
        // cpu->UpdateIRQ(NES_CPU::IRQD_DMC, false);

        out[0] = out[1] = out[2] = 0;
        damp = 0;
        dmc_pop = false;
        dmc_pop_offset = 0;
        dmc_pop_follow = 0;
        dac_lsb = 0;
        data = 0x100;
        empty = true;
        adr_reg = 0;
        dlength = 0;
        len_reg = 0;
        daddress = 0;
        noise = 1;
        noise_tap = (1 << 1);

        if (option[OPT_RANDOMIZE_NOISE]) {
            noise |= ::rand();
            counter[1] = -(rand() & 511);
        }
        if (option[OPT_RANDOMIZE_TRI]) {
            tphase = ::rand() & 31;
            counter[0] = -(rand() & 2047);
        }

        SetRate(rate);
    }

    virtual void Tick(uint32_t clocks)
    {
        out[0] = calc_tri(clocks);
        out[1] = calc_noise(clocks);
        out[2] = calc_dmc(clocks);
    }

    virtual uint32_t Render(int32_t b[2])
    {
        out[0] = (mask & 1) ? 0 : out[0];
        out[1] = (mask & 2) ? 0 : out[1];
        out[2] = (mask & 4) ? 0 : out[2];

        int32_t m[3];
        m[0] = tnd_table[0][out[0]][0][0];
        m[1] = tnd_table[0][0][out[1]][0];
        m[2] = tnd_table[0][0][0][out[2]];

        if (option[OPT_NONLINEAR_MIXER]) {
            int32_t ref = m[0] + m[1] + m[2];
            int32_t voltage = tnd_table[1][out[0]][out[1]][out[2]];
            if (ref) {
                for (int i = 0; i < 3; ++i) {
                    m[i] = (m[i] * voltage) / ref;
                }
            } else {
                for (int i = 0; i < 3; ++i) {
                    m[i] = voltage;
                }
            }
        }

        // anti-click nullifies any 4011 write but preserves nonlinearity
        if (option[OPT_DPCM_ANTI_CLICK]) {
            if (dmc_pop) // $4011 will cause pop this frame
            {
                // adjust offset to counteract pop
                dmc_pop_offset += dmc_pop_follow - m[2];
                dmc_pop = false;

                // prevent overflow, keep headspace at edges
                const int32_t OFFSET_MAX = (1 << 30) - (4 << 16);
                if (dmc_pop_offset > OFFSET_MAX) dmc_pop_offset = OFFSET_MAX;
                if (dmc_pop_offset < -OFFSET_MAX) dmc_pop_offset = -OFFSET_MAX;
            }
            dmc_pop_follow = m[2]; // remember previous position

            m[2] += dmc_pop_offset; // apply offset

            // TODO implement this in a better way
            // roll off offset (not ideal, but prevents overflow)
            if (dmc_pop_offset > 0)
                --dmc_pop_offset;
            else if (dmc_pop_offset < 0)
                ++dmc_pop_offset;
        }

        b[0] = m[0] * sm[0][0];
        b[0] += m[1] * sm[0][1];
        b[0] += m[2] * sm[0][2];
        b[0] >>= 7;

        b[1] = m[0] * sm[1][0];
        b[1] += m[1] * sm[1][1];
        b[1] += m[2] * sm[1][2];
        b[1] >>= 7;

        return 2;
    }

    virtual bool Write(uint32_t adr, uint32_t val, uint32_t id = 0)
    {
        static const uint8_t length_table[32] = {
            0x0A, 0xFE,
            0x14, 0x02,
            0x28, 0x04,
            0x50, 0x06,
            0xA0, 0x08,
            0x3C, 0x0A,
            0x0E, 0x0C,
            0x1A, 0x0E,
            0x0C, 0x10,
            0x18, 0x12,
            0x30, 0x14,
            0x60, 0x16,
            0xC0, 0x18,
            0x48, 0x1A,
            0x10, 0x1C,
            0x20, 0x1E};

        if (adr == 0x4015) {
            enable[0] = (val & 4) ? true : false;
            enable[1] = (val & 8) ? true : false;

            if (!enable[0]) {
                length_counter[0] = 0;
            }
            if (!enable[1]) {
                length_counter[1] = 0;
            }

            if ((val & 16) && dlength == 0) {
                daddress = (0xC000 | (adr_reg << 6));
                dlength = (len_reg << 4) + 1;
            } else if (!(val & 16)) {
                dlength = 0;
            }

            irq = false;
            // cpu->UpdateIRQ(NES_CPU::IRQD_DMC, false);

            reg[adr - 0x4008] = val;
            return true;
        }

        if (adr == 0x4017) {
            frame_irq_enable = ((val & 0x40) != 0x40);
            if (frame_irq_enable) frame_irq = false;
            // cpu->UpdateIRQ(NES_CPU::IRQD_FRAME, false);

            frame_sequence_count = 0;
            if (val & 0x80) {
                frame_sequence_steps = 5;
                frame_sequence_step = 0;
                FrameSequence(frame_sequence_step);
                ++frame_sequence_step;
            } else {
                frame_sequence_steps = 4;
                frame_sequence_step = 1;
            }
        }

        if (adr < 0x4008 || 0x4013 < adr)
            return false;

        reg[adr - 0x4008] = val & 0xff;
        switch (adr) {
            case 0x4008:
                linear_counter_control = (val >> 7) & 1;
                linear_counter_reload = val & 0x7F;
                break;

            case 0x4009:
                break;

            case 0x400a:
                tri_freq = val | (tri_freq & 0x700);
                break;

            case 0x400b:
                tri_freq = (tri_freq & 0xff) | ((val & 0x7) << 8);
                linear_counter_halt = true;
                if (enable[0]) {
                    length_counter[0] = length_table[(val >> 3) & 0x1f];
                }
                break;

                // noise

            case 0x400c:
                noise_volume = val & 15;
                envelope_div_period = val & 15;
                envelope_disable = (val >> 4) & 1;
                envelope_loop = (val >> 5) & 1;
                break;

            case 0x400d:
                break;

            case 0x400e:
                if (option[OPT_ENABLE_PNOISE])
                    noise_tap = (val & 0x80) ? (1 << 6) : (1 << 1);
                else
                    noise_tap = (1 << 1);
                nfreq = wavlen_table[pal][val & 15];
                break;

            case 0x400f:
                if (enable[1]) {
                    length_counter[1] = length_table[(val >> 3) & 0x1f];
                }
                envelope_write = true;
                break;

                // dmc

            case 0x4010:
                mode = (val >> 6) & 3;
                if (!(mode & 2)) {
                    irq = false;
                    // cpu->UpdateIRQ(NES_CPU::IRQD_DMC, false);
                }
                dfreq = freq_table[pal][val & 15];
                break;

            case 0x4011:
                if (option[OPT_ENABLE_4011]) {
                    damp = (val >> 1) & 0x3f;
                    dac_lsb = val & 1;
                    dmc_pop = true;
                }
                break;

            case 0x4012:
                adr_reg = val & 0xff;
                // ������daddress�͍X�V����Ȃ�
                break;

            case 0x4013:
                len_reg = val & 0xff;
                // ������length�͍X�V����Ȃ�
                break;

            default:
                return false;
        }

        return true;
    }

    virtual bool Read(uint32_t adr, uint32_t& val, uint32_t id = 0)
    {
        if (adr == 0x4015) {
            val |= (irq ? 0x80 : 0) | (frame_irq ? 0x40 : 0) | ((dlength > 0) ? 0x10 : 0) | (length_counter[1] ? 0x08 : 0) | (length_counter[0] ? 0x04 : 0);

            frame_irq = false;
            // cpu->UpdateIRQ(NES_CPU::IRQD_FRAME, false);
            return true;
        } else if (0x4008 <= adr && adr <= 0x4014) {
            val |= reg[adr - 0x4008];
            return true;
        } else
            return false;
    }

    virtual void SetRate(long rate) { this->rate = rate ? rate : DEFAULT_RATE; };
    virtual void SetClock(long clock) { this->clock = clock; }
    virtual void SetMask(int m) { mask = m; }

    virtual void SetOption(int id, int val)
    {
        if (id < OPT_END) {
            option[id] = val;
            if (id == OPT_NONLINEAR_MIXER) {
                InitializeTNDTable(8227, 12241, 22638);
            }
        }
    }

    virtual void SetStereoMix(int trk, int16_t mixl, int16_t mixr)
    {
        if (trk < 0) return;
        if (trk > 2) return;
        sm[0][trk] = mixl;
        sm[1][trk] = mixr;
    }
    // void SetCPU(NES_CPU* cpu_);
};

// https://github.com/bbbradsmith/nsfplay/blob/master/xgm/devices/Audio/rconv.cpp
// https://github.com/bbbradsmith/nsfplay/blob/master/xgm/devices/Audio/rconv.h
class RateConverter : public IRenderable
{
  protected:
    IRenderable* target;
    long clock, rate;
    int mult;
    int32_t tap[2][128];
    int hri[128];
    uint32_t clocks; // render clocks pending Tick
    NES_DMC* dmc;
    int cpu_clocks; // CPU clocks pending Tick
    int cpu_rest;   // extra clock accumulator (instructions will get ahead by a few clocks)
    bool fast_skip;

    void ClockCPU(int c)
    {
        int real_cpu_clocks = 0;
        /*
        if (cpu) {
            cpu_rest += c;
            if (cpu_rest > 0) {
                real_cpu_clocks = cpu->Exec(cpu_rest);
                cpu_rest -= real_cpu_clocks;
            }
        } */
        if (dmc) dmc->TickFrameSequence(real_cpu_clocks);
    }

  public:
    RateConverter() : clock(0), rate(0), mult(0), clocks(0), dmc(NULL), cpu_clocks(0), cpu_rest(0), fast_skip(true) {}
    virtual ~RateConverter() {}
    void Attach(IRenderable* t) { target = t; }
    void SetClock(long clock) { this->clock = clock; }
    void SetRate(long rate) { this->rate = rate; }
    virtual void Tick(uint32_t clocks_) { this->clocks += clocks_; } // ticks get executed during Render
    virtual uint32_t Render(int32_t b[2]) { return FastRender(b); }
    virtual void Skip() {} // Does ticks in lieu of Render

    void Reset()
    {
        clocks = 0; // cancel any pending ticks
        cpu_clocks = 0;
        cpu_rest = 0;
        if (clock && rate) {
            mult = (int)(clock / rate);
            if (mult < 2) mult = 1;
            if (mult > 63) mult = 63;
            hri[0] = 6023;
            hri[1] = 5887;
            hri[2] = 5494;
            hri[3] = 4881;
            hri[4] = 4108;
            hri[5] = 3252;
            hri[6] = 2396;
            hri[7] = 1624;
            hri[8] = 1011;
            hri[9] = 617;
            hri[10] = 481;
            for (int i = 0; i <= mult * 2; i++) {
                tap[0][i] = tap[1][i] = 0;
            }
        }
    }
    void TickCPU(int t) { cpu_clocks += t; }
    void SetDMC(NES_DMC* d) { dmc = d; }
    void SetFastSkip(bool s) { fast_skip = s; }

  private:
    inline uint32_t FastRender(int32_t b[2])
    {
        int64_t out[2];
        static int32_t t[2];

        for (int i = 0; i <= mult; i++) {
            tap[0][i] = tap[0][i + mult];
            tap[1][i] = tap[1][i + mult];
        }

        // divide clock ticks among samples evenly
        int mclocks = 0;
        int mcclocks = 0;
        for (int i = 1; i <= mult; i++) {
            // CPU first
            mcclocks += cpu_clocks;
            if (mcclocks >= mult) {
                int sub_clocks = mcclocks / mult;
                ClockCPU(sub_clocks);
                mcclocks -= (sub_clocks * mult);
            }

            // Audio devices second
            mclocks += clocks;
            if (mclocks >= mult) {
                int sub_clocks = mclocks / mult;
                target->Tick(sub_clocks);
                mclocks -= (sub_clocks * mult);
            }

            target->Render(t);
            tap[0][mult + i] = t[0];
            tap[1][mult + i] = t[1];
        }
        clocks = 0;
        cpu_clocks = 0;

        out[0] = hri[0] * tap[0][mult];
        out[1] = hri[0] * tap[1][mult];

        for (int i = 1; i <= mult; i++) {
            out[0] += hri[i] * (tap[0][mult + i] + tap[0][mult - i]);
            out[1] += hri[i] * (tap[1][mult + i] + tap[1][mult - i]);
        }

        // b[0] = (int32_t)out[0];
        // b[1] = (int32_t)out[1];
        b[0] = int32_t(out[0] >> PRECISION);
        b[1] = int32_t(out[1] >> PRECISION);
        return 2;
    }
};

enum DeviceCode {
    APU = 0,
    DMC,
    NES_DEVICE_MAX
};

enum RegionCode {
    REGION_NTSC = 0,
    REGION_PAL,
    REGION_DENDY
};

class NesVgmDriver
{
  private:
    long rate;
    int nch; // number of channels
    int song;
    int last_out;
    int silent_length;
    long cpu_clock_rest;
    int time_in_ms;
    bool infinite;
    int quality;
    int total_render;
    int frame_render;
    int frame_in_ms;
    xgm::Bus apu_bus;
    xgm::Layer stack;
    xgm::Layer layer;
    xgm::Mixer mixer;
    xgm::ISoundChip* sc[NES_DEVICE_MAX];
    xgm::Amplifier amp[NES_DEVICE_MAX];
    xgm::RateConverter rconv;
    xgm::NES_APU* apu;
    xgm::NES_DMC* dmc;
    const uint8_t* vgm;
    size_t vgmSize;
    uint32_t vgmClock;
    int vgmCursor;
    int vgmLoopOffset;
    int vgmWait;
    bool vgmEnd;

  public:
    NesVgmDriver()
    {
        quality = 10; // 変更する場合は rconv の hri を適切に設定する必要がある（10決め打ちの実装にすることで浮動小数点回避している）
        sc[APU] = (apu = new xgm::NES_APU());
        sc[DMC] = (dmc = new xgm::NES_DMC());
        dmc->SetAPU(apu);
        for (int i = 0; i < NES_DEVICE_MAX; i++) {
            amp[i].Attach(sc[i]);
        }
        rconv.Attach(&mixer);
        nch = 1;
        infinite = false;
        last_out = 0;
    }

    bool Load(const uint8_t* vgm, size_t size)
    {
        if (!vgm || size < 0x100) {
            return false;
        }
        this->vgm = vgm;
        this->vgmSize = size;
        this->vgmCursor = 0;
        this->vgmWait = 0;
        this->vgmEnd = false;

        // Check eye-catch
        if (0 != memcmp(vgm, "Vgm ", 4)) {
            this->vgm = nullptr;
            return false;
        }

        // Check version
        uint32_t version;
        memcpy(&version, &vgm[0x08], 4);
        if (version < 0x161) {
            this->vgm = nullptr;
            return false; // require NES APU supported version (1.61 or later)
        }

        // Check NES APU clock
        memcpy(&this->vgmClock, &vgm[0x84], 4);
        if (0 == this->vgmClock) {
            this->vgm = nullptr;
            return false; // Not NES APU data
        }

        // Set stream offset
        memcpy(&this->vgmCursor, &vgm[0x34], 4);
        this->vgmCursor += 0x40 - 0x0C;

        // Set loop offset
        memcpy(&this->vgmLoopOffset, &vgm[0x1C], 4);
        this->vgmLoopOffset += this->vgmLoopOffset ? 0x1C : 0;

        Reload();
        return true;
    }

    RegionCode GetRegion()
    {
        if (vgm) {
            uint32_t rate;
            memcpy(&rate, &vgm[0x24], 4);
            switch (rate) {
                case 60: return REGION_NTSC;
                case 50: return REGION_PAL;
                case 59: return REGION_DENDY;
            }
        }
        return REGION_NTSC;
    }

    uint32_t GetNesBaseCycle()
    {
        switch (GetRegion()) {
            case REGION_NTSC: return 1789773;
            case REGION_PAL: return 1662607;
            case REGION_DENDY: return 1773448;
        }
        return 1789773;
    }

    void SetPlayFreq(int samplingRate)
    {
        rate = samplingRate;
        dmc->SetPal(GetRegion() == REGION_PAL);
        long oversample = rate * quality;
        if (oversample > this->vgmClock) oversample = this->vgmClock;
        if (oversample < rate) oversample = rate;
        for (int i = 0; i < NES_DEVICE_MAX; i++) {
            sc[i]->SetClock(this->vgmClock);
            sc[i]->SetRate(oversample);
        }
        rconv.SetClock(oversample);
        rconv.SetRate(rate);
        rconv.SetFastSkip(true);
        mixer.Reset();
        rconv.Reset();
    }

    void SetChannels(int channels)
    {
        if (channels > 2) channels = 2;
        if (channels < 1) channels = 1;
        nch = channels;
    }

    void Reset()
    {
        time_in_ms = 0;
        silent_length = 0;
        total_render = 0;
        frame_render = (int)(rate) / 60;
        cpu_clock_rest = 0;
        // RegionCode region = GetRegion();
        Reload();
        SetPlayFreq(rate);
        stack.Reset();
        // int region_register = (region == REGION_PAL) ? 1 : 0;
        apu->SetMask(0);
        dmc->SetMask(0);

        // suppress starting click by setting DC filter to balance the starting level at 0
        int32_t b[2];
        for (int i = 0; i < NES_DEVICE_MAX; ++i) {
            sc[i]->Tick(0); // determine starting state for all sound units
        }
        rconv.Tick(0);
        for (int i = 0; i < (quality + 1); ++i) {
            // warm up rconv/render with enough sample to reach a steady state
            rconv.Render(b);
        }
    }

    uint32_t Render(int16_t* stream, int length)
    {
        int buf[2];
        int out[2];
        int outm;
        uint32_t i;
        int master_volume = 512;
        int apu_clock_per_sample = (GetNesBaseCycle() * 256) / rate;

        for (i = 0; i < (uint32_t)length; i++) {
            total_render++;
            // tick CPU
            cpu_clock_rest += apu_clock_per_sample;
            int cpu_clocks = (int)(cpu_clock_rest / 256);
            rconv.TickCPU(cpu_clocks);
            dmc->TickFrameSequence(cpu_clocks);
            rconv.Tick(cpu_clocks);
            this->vgmWait -= 1;
            this->UpdateRegister();
            cpu_clock_rest -= cpu_clocks * 256;

            // render output
            rconv.Render(buf);             // ticks APU/CPU and renders with subdivision and resampling (also does UpdateInfo)
            outm = (buf[0] + buf[1]) >> 1; // mono mix
            if (outm == last_out) {
                silent_length++;
            } else {
                silent_length = 0;
            }
            last_out = outm;

            out[0] = buf[0];
            out[1] = buf[1];
            out[0] = (out[0] * master_volume) >> 8;
            out[1] = (out[1] * master_volume) >> 8;

            if (out[0] < -32767) {
                out[0] = -32767;
            } else if (32767 < out[0]) {
                out[0] = 32767;
            }

            if (out[1] < -32767) {
                out[1] = -32767;
            } else if (32767 < out[1]) {
                out[1] = 32767;
            }

            if (nch == 2) {
                stream[0] = out[0];
                stream[1] = out[1];
            } else // if not 2 channels, presume mono
            {
                outm = (out[0] + out[1]) >> 1;
                for (int i = 0; i < nch; ++i)
                    stream[0] = outm;
            }
            stream += nch;
        }
        time_in_ms += (int)(1000 * length / rate);
        return length;
    }

  private:
    void Reload()
    {
        // virtual machine controlling memory reads and writes
        // to various devices, expansions, etc.
        stack.DetachAll();
        layer.DetachAll();
        mixer.DetachAll();
        apu_bus.DetachAll();

        // APU units are combined into a single bus
        apu_bus.Attach(sc[APU]);
        apu_bus.Attach(sc[DMC]);
        stack.Attach(&apu_bus);
        mixer.Attach(&amp[APU]);
        mixer.Attach(&amp[DMC]);

        rconv.SetDMC(dmc);

        // memory layer comes last
        stack.Attach(&layer);
    }

    void UpdateRegister()
    {
        if (!this->vgm || this->vgmEnd) {
            return;
        }

        while (this->vgmWait < 1) {
            uint8_t cmd = this->vgm[this->vgmCursor++];
            switch (cmd) {
                case 0xB4: {
                    // NES APU Write Reg
                    uint8_t addr = this->vgm[this->vgmCursor++];
                    uint8_t value = this->vgm[this->vgmCursor++];
                    apu_bus.Write(0x4000 + addr, value);
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
                    if (0x70 <= cmd && cmd < 0x80) {
                        // Wait n+1 samples
                        this->vgmWait += cmd - 0x6F;
                    } else {
                        // unsupported command
                        this->vgmEnd = true;
                        return;
                    }
                    break;
            }
        }
    }
};

} // namespace xgm
