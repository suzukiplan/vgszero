#include "rconv.h"
#include "../CPU/nes_cpu.h"
#include "../Sound/nes_dmc.h"
#include <math.h>

namespace xgm
{

#define PRECISION 16

RateConverter::RateConverter() : clock(0), rate(0), mult(0), clocks(0),
                                 cpu(NULL), dmc(NULL), cpu_clocks(0), cpu_rest(0),
                                 fast_skip(true)
{
}

RateConverter::~RateConverter()
{
}

void RateConverter::Attach(IRenderable* t)
{
    target = t;
}

void RateConverter::Reset()
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

// �{���͊�{�̂�
void RateConverter::SetClock(long c)
{
    clock = c;
}

void RateConverter::SetRate(long r)
{
    rate = r;
}

void RateConverter::Tick(uint32_t clocks_)
{
    clocks += clocks_;
}

uint32_t RateConverter::Render(int32_t b[2])
{
    return FastRender(b);
}

void RateConverter::ClockCPU(int c)
{
    int real_cpu_clocks = 0;
    if (cpu) {
        cpu_rest += c;
        if (cpu_rest > 0) {
            real_cpu_clocks = cpu->Exec(cpu_rest);
            cpu_rest -= real_cpu_clocks;
        }
    }
    if (dmc) dmc->TickFrameSequence(real_cpu_clocks);
}

void RateConverter::Skip()
{
}

// ���͂�-32768�`+32767�܂�
inline uint32_t RateConverter::FastRender(int32_t b[2])
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
    assert(mclocks == 0); // all clocks must be used
    assert(mcclocks == 0);
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

} // namespace xgm