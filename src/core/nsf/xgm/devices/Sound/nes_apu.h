#ifndef _NES_APU_H_
#define _NES_APU_H_
#include "../device.h"
#include "nes_dmc.h"

namespace xgm
{
/** Upper half of APU **/
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

    void sweep_sqr(int ch); // calculates target sweep frequency
    int32_t calc_sqr(int ch, uint32_t clocks);

  public:
    NES_APU();
    ~NES_APU();

    void FrameSequence(int s);

    virtual void Reset();
    virtual void Tick(uint32_t clocks);
    virtual uint32_t Render(int32_t b[2]);
    virtual bool Read(uint32_t adr, uint32_t& val, uint32_t id = 0);
    virtual bool Write(uint32_t adr, uint32_t val, uint32_t id = 0);
    virtual void SetRate(long rate);
    virtual void SetClock(long clock);
    virtual void SetOption(int id, int b);
    virtual void SetMask(int m) { mask = m; }
    virtual void SetStereoMix(int trk, int16_t mixl, int16_t mixr);
};

} // namespace xgm

#endif
