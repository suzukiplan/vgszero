#ifndef _NES_DMC_H_
#define _NES_DMC_H_

#include "../device.h"
#include "../CPU/nes_cpu.h"

extern const short rom_tndtable[65536];

namespace xgm
{
class NES_APU; // forward declaration

/** Bottom Half of APU **/
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
    static const uint32_t freq_table[2][16];
    static const uint32_t wavlen_table[2][16];
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

    NES_CPU* cpu; // IRQ needs CPU access

    inline uint32_t calc_tri(uint32_t clocks);
    inline uint32_t calc_dmc(uint32_t clocks);
    inline uint32_t calc_noise(uint32_t clocks);

  public:
    NES_DMC();
    ~NES_DMC();

    void InitializeTNDTable(int wt, int wn, int wd);
    void SetPal(bool is_pal);
    void SetAPU(NES_APU* apu_);
    void SetMemory(IDevice* r);
    void FrameSequence(int s);
    int GetDamp() { return (damp << 1) | dac_lsb; }
    void TickFrameSequence(uint32_t clocks);

    virtual void Reset();
    virtual void Tick(uint32_t clocks);
    virtual uint32_t Render(int32_t b[2]);
    virtual bool Write(uint32_t adr, uint32_t val, uint32_t id = 0);
    virtual bool Read(uint32_t adr, uint32_t& val, uint32_t id = 0);
    virtual void SetRate(long rate);
    virtual void SetClock(long clock);
    virtual void SetOption(int, int);
    virtual void SetMask(int m) { mask = m; }
    virtual void SetStereoMix(int trk, int16_t mixl, int16_t mixr);
    void SetCPU(NES_CPU* cpu_);
};

} // namespace xgm

#endif
