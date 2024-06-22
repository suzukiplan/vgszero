// NSF Player implementation without STL
// Original auther: Mamiya (mamiya@proc.org.tohoku.ac.jp)
// Modified by: Yoji Suzuki
// License under GPLv3
// NOTE: This source code is based on the following source code:
// - https://github.com/bbbradsmith/nsfplay/blob/master/xgm/player/nsf/nsfplay.cpp
// - https://github.com/bbbradsmith/nsfplay/blob/master/xgm/player/nsf/nsfplay.h
#include "xgm/player/nsf/nsf.h"
#include "xgm/devices/CPU/nes_cpu.h"
#include "xgm/devices/Memory/nes_bank.h"
#include "xgm/devices/Memory/nes_mem.h"
#include "xgm/devices/Memory/nsf2_vectors.h"
#include "xgm/devices/Sound/nes_apu.h"
#include "xgm/devices/Sound/nes_vrc6.h"
#include "xgm/devices/Sound/nes_dmc.h"
#include "xgm/devices/Audio/mixer.h"
#include "xgm/devices/Audio/amplifier.h"
#include "xgm/devices/Audio/rconv.h"
#include "xgm/devices/Misc/nsf2_irq.h"
#include <stdint.h>

enum DeviceCode { APU = 0,
                  DMC,
                  VRC6,
                  NES_DEVICE_MAX };

#define NES_CHANNEL_MAX 32

class NSFPlayer
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

    xgm::Bus apu_bus;
    xgm::Layer stack;
    xgm::Layer layer;
    xgm::Mixer mixer;

    xgm::NES_CPU cpu;
    xgm::NES_MEM mem;
    xgm::NES_BANK bank;
    xgm::NSF2_Vectors nsf2_vectors;
    xgm::NSF2_IRQ nsf2_irq;

    xgm::ISoundChip* sc[NES_DEVICE_MAX];
    xgm::Amplifier amp[NES_DEVICE_MAX];
    xgm::RateConverter rconv;

    enum {
        APU1_TRK0 = 0,
        APU1_TRK1,
        APU2_TRK0,
        APU2_TRK1,
        APU2_TRK2,
        FDS_TRK0,
        MMC5_TRK0,
        MMC5_TRK1,
        MMC5_TRK2,
        FME7_TRK0,
        FME7_TRK1,
        FME7_TRK2,
        FME7_TRK3,
        FME7_TRK4,
        VRC6_TRK0,
        VRC6_TRK1,
        VRC6_TRK2,
        VRC7_TRK0,
        VRC7_TRK1,
        VRC7_TRK2,
        VRC7_TRK3,
        VRC7_TRK4,
        VRC7_TRK5,
        N106_TRK0,
        N106_TRK1,
        N106_TRK2,
        N106_TRK3,
        N106_TRK4,
        N106_TRK5,
        N106_TRK6,
        N106_TRK7,
        VRC7_TRK6,
        VRC7_TRK7,
        VRC7_TRK8,
        NES_TRACK_MAX
    };

    int total_render;
    int frame_render;
    int frame_in_ms;

    xgm::NES_APU* apu;
    xgm::NES_DMC* dmc;
    xgm::NES_VRC6* vrc6;
    xgm::NSF* nsf;

    enum {
        REGION_NTSC = 0,
        REGION_PAL,
        REGION_DENDY
    };

  public:
    NSFPlayer()
    {
        nsf = NULL;
        quality = 10; // 変更する場合は rconv の hri を適切に設定する必要がある（10決め打ちの実装にすることで浮動小数点回避している）
        sc[APU] = (apu = new xgm::NES_APU());
        sc[DMC] = (dmc = new xgm::NES_DMC());
        sc[VRC6] = (vrc6 = new xgm::NES_VRC6());
        nsf2_irq.SetCPU(&cpu);
        dmc->SetAPU(apu);
        dmc->SetCPU(&cpu);
        for (int i = 0; i < NES_DEVICE_MAX; i++) {
            amp[i].Attach(sc[i]);
        }
        rconv.Attach(&mixer);
        nch = 1;
        infinite = false;
        last_out = 0;
    }

    bool Load(xgm::NSF* nsf)
    {
        if (!nsf) {
            return false;
        }
        this->nsf = nsf;
        Reload();
        return true;
    }

    void SetPlayFreq(int samplingRate)
    {
        rate = samplingRate;
        int region = GetRegion(nsf->regn, nsf->regn_pref);
        dmc->SetPal(region == REGION_PAL);
        long clock;
        switch (region) {
            case REGION_NTSC: clock = 1789773; break;
            case REGION_PAL: clock = 1662607; break;
            case REGION_DENDY: clock = 1773448; break;
        }
        long oversample = rate * quality;
        if (oversample > clock) oversample = clock;
        if (oversample < rate) oversample = rate;
        for (int i = 0; i < NES_DEVICE_MAX; i++) {
            sc[i]->SetClock(clock);
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

    bool SetSong(int trackNumber)
    {
        nsf->song = trackNumber % nsf->songs;
        UpdateInfinite();
        return true;
    }

    void Reset()
    {
        time_in_ms = 0;
        silent_length = 0;
        total_render = 0;
        frame_render = (int)(rate) / 60;
        cpu_clock_rest = 0;

        int region = GetRegion(nsf->regn, nsf->regn_pref);
        switch (region) {
            case REGION_NTSC: cpu.nes_basecycles = 1789773; break;
            case REGION_PAL: cpu.nes_basecycles = 1662607; break;
            case REGION_DENDY: cpu.nes_basecycles = 1773448; break;
        }

        Reload();
        SetPlayFreq(rate);
        stack.Reset();
        cpu.Reset();

        uint16_t nsfspeed = (region == REGION_DENDY) ? nsf->speed_dendy : (region == REGION_PAL) ? nsf->speed_pal
                                                                                                 : nsf->speed_ntsc;

        int song = nsf->song;
        int region_register = (region == REGION_PAL) ? 1 : 0;
        if (region == REGION_DENDY && (nsf->regn & 4)) region_register = 2; // use 2 for Dendy iff explicitly supported, otherwise 0

        cpu.Start(
            nsf->init_address,
            nsf->play_address,
            1000000 / nsfspeed,
            song,
            region_register,
            nsf->nsf2_bits,
            false, // disable IRQ
            &nsf2_irq);

        apu->SetMask(0);
        dmc->SetMask(0);
        vrc6->SetMask(0);

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
        int apu_clock_per_sample = (cpu.nes_basecycles * 256) / rate;

        for (i = 0; i < (uint32_t)length; i++) {
            total_render++;
            // tick CPU
            cpu_clock_rest += apu_clock_per_sample;
            int cpu_clocks = (int)(cpu_clock_rest / 256);
            rconv.TickCPU(cpu_clocks);
            rconv.Tick(cpu_clocks);
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
        int i, bmax = 0;
        for (i = 0; i < 8; i++) {
            if (bmax < nsf->bankswitch[i]) {
                bmax = nsf->bankswitch[i];
            }
        }
        mem.SetImage(nsf->body, nsf->load_address, nsf->bodysize);
        if (bmax) {
            bank.SetImage(nsf->body, nsf->load_address, nsf->bodysize);
            for (i = 0; i < 8; i++) {
                bank.SetBankDefault(i + 8, nsf->bankswitch[i]);
            }
        }
        // virtual machine controlling memory reads and writes
        // to various devices, expansions, etc.
        stack.DetachAll();
        layer.DetachAll();
        mixer.DetachAll();
        apu_bus.DetachAll();

        // setup player program at PLAYER_RESERVED ($4100)
        const uint8_t PLAYER_PROGRAM[] =
            {
                0x20, 0x1E, 0x41,             // $4100 JSR PLAY ($411E is a placeholder RTS)
                0x4C, 0x03, 0x41,             // $4103 JMP to self (do nothing loop for detecting end of frame)
                0x48, 0x8A, 0x48, 0x98, 0x48, // $4106 PHA TXA PHA TYA PHA
                0xA9, 0x00, 0x8D, 0x00, 0x20, // $410B LDA #$00 STA $2000 (disable NMI re-entry)
                0x20, 0x1E, 0x41,             // $4110 JSR PLAY for non-returning INIT ($411E placeholder)
                0xA9, 0x80, 0x8D, 0x00, 0x20, // $4113 LDA #$80 STA $2000 (re-enable NMI)
                0x68, 0xA8, 0x68, 0xAA, 0x68, // $4118 PLA TAY PLA TAX PLA
                0x40,                         // $411D RTI
                0x60,                         // $411E RTS
            };
        const int PLAYER_PROGRAM_SIZE = sizeof(PLAYER_PROGRAM);
        mem.SetReserved(PLAYER_PROGRAM, PLAYER_PROGRAM_SIZE);
        // Note: PLAYER_RESERVED+1,2 are used directly in nes_cpu.cpp.
        //       If the JSR PLAY at the start ever moves or changes, be sure to update that.

        if (nsf->nsf2_bits & 0x30) // uses IRQ or non-returning INIT
        {
            layer.Attach(&nsf2_vectors);
            nsf2_vectors.SetCPU(&cpu);
            nsf2_vectors.ForceVector(0, xgm::PLAYER_RESERVED + 0x06); // NMI routine that calls PLAY
            nsf2_vectors.ForceVector(1, xgm::PLAYER_RESERVED + 0x03); // Reset routine goes to "breaked" infinite loop (not used)
            mem.WriteReserved(xgm::PLAYER_RESERVED + 0x11, nsf->play_address & 0xFF);
            mem.WriteReserved(xgm::PLAYER_RESERVED + 0x12, nsf->play_address >> 8);
            // Initialize IRQ vector with existing contents of $FFFE.
            uint32_t iv[2];
            mem.Read(0xFFFE, iv[0]);
            mem.Read(0xFFFF, iv[1]);
            if (bmax) {
                bank.Read(0xFFFE, iv[0]);
                bank.Read(0xFFFF, iv[1]);
            }
            uint32_t iva = (iv[0] & 0xFF) | ((iv[1] & 0xFF) << 8);
            nsf2_vectors.ForceVector(2, iva);
        }
        if (nsf->nsf2_bits & 0x10) // uses IRQ
        {
            stack.Attach(&nsf2_irq);
        }
        if (bmax) layer.Attach(&bank);
        layer.Attach(&mem);
        dmc->SetMemory(&layer);

        // APU units are combined into a single bus
        apu_bus.Attach(sc[APU]);
        apu_bus.Attach(sc[DMC]);
        stack.Attach(&apu_bus);

        mixer.Attach(&amp[APU]);
        mixer.Attach(&amp[DMC]);

        rconv.SetCPU(&cpu);
        rconv.SetDMC(dmc);

        if (nsf->use_vrc6) {
            stack.Attach(sc[VRC6]);
            mixer.Attach(&amp[VRC6]);
        }
        mem.SetFDSMode(false);
        bank.SetFDSMode(false);

        // memory layer comes last
        stack.Attach(&layer);

        // NOTE: each layer in the stack is given a chance to take a read or write
        // exclusively. The stack is structured like this:
        //     loop detector > APU > expansions > main memory

        // main memory comes after other expansions because
        // when the FDS mode is enabled, VRC6/VRC7/5B have writable registers
        // in RAM areas of main memory. To prevent these from overwriting RAM
        // I allow the expansions above it in the stack to prevent them.

        // MMC5 comes high in the stack so that its PCM read behaviour
        // can reread from the stack below and does not get blocked by any
        // stack above.

        cpu.SetMemory(&stack);
        cpu.SetNESMemory(&mem);
    }

    void UpdateInfinite()
    {
        infinite = false;
    }

    int GetRegion(uint8_t flags, int pref)
    {
        int region;
        switch (pref) {
            case 0: region = REGION_NTSC; break;
            case 1: region = REGION_PAL; break;
            case 2: region = REGION_DENDY; break;
            default: region = -1;
        }

        // extra fallback if the chosen region is not supported
        if (region == REGION_DENDY && !(flags & 4)) region = -1;
        if (region == REGION_PAL && !(flags & 2)) region = -1;
        if (region == REGION_NTSC && !(flags & 1)) region = -1;

        // a valid region!
        if (region >= REGION_NTSC && region <= REGION_DENDY) return region;

        // any port in a storm
        if (flags & 1) return REGION_NTSC;
        if (flags & 2) return REGION_PAL;
        if (flags & 4) return REGION_DENDY;

        // no valid regions? giving up
        return REGION_NTSC;
    }
};