// NSF Player implementation without STL
// Original auther: Mamiya (mamiya@proc.org.tohoku.ac.jp)
// Modified by: Yoji Suzuki
// License under GPLv3
// NOTE: This source code is based on the following source code:
// https://github.com/bbbradsmith/nsfplay/blob/master/xgm/devices/Sound/nes_vrc6.h
#ifndef _NES_VRC6_H_
#define _NES_VRC6_H_
#include "device.h"

namespace xgm
{

class NES_VRC6 : public ISoundChip
{
  public:
    enum {
        OPT_END
    };

  protected:
    uint32_t counter[3]; // frequency divider
    uint32_t phase[3];   // phase counter
    uint32_t freq2[3];   // adjusted frequency
    int count14;         // saw 14-stage counter

    // int option[OPT_END];
    int mask;
    int32_t sm[2][3]; // stereo mix
    int duty[2];
    int volume[3];
    int enable[3];
    int gate[3];
    uint32_t freq[3];
    int16_t calc_sqr(int i, uint32_t clocks);
    int16_t calc_saw(uint32_t clocks);
    bool halt;
    int freq_shift;
    long clock, rate;
    int32_t out[3];

  public:
    NES_VRC6();
    ~NES_VRC6();

    virtual void Reset();
    virtual void Tick(uint32_t clocks);
    virtual uint32_t Render(int32_t b[2]);
    virtual bool Read(uint32_t adr, uint32_t& val, uint32_t id = 0);
    virtual bool Write(uint32_t adr, uint32_t val, uint32_t id = 0);
    virtual void SetClock(long);
    virtual void SetRate(long);
    virtual void SetOption(int, int);
    virtual void SetMask(int m) { mask = m; }
    virtual void SetStereoMix(int trk, int16_t mixl, int16_t mixr);
};

} // namespace xgm

#endif
