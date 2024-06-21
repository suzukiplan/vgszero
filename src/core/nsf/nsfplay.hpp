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
#include "xgm/devices/Sound/nes_fme7.h"
#include "xgm/devices/Sound/nes_vrc6.h"
#include "xgm/devices/Sound/nes_dmc.h"
#include "xgm/devices/Sound/nes_mmc5.h"
#include "xgm/devices/Sound/nes_n106.h"
#include "xgm/devices/Sound/nes_fds.h"
#include "xgm/devices/Audio/mixer.h"
#include "xgm/devices/Audio/fader.h"
#include "xgm/devices/Audio/amplifier.h"
#include "xgm/devices/Audio/rconv.h"
#include "xgm/devices/Misc/nsf2_irq.h"
#include <stdint.h>

enum DeviceCode { APU = 0,
                  DMC,
                  FME7,
                  MMC5,
                  N106,
                  VRC6,
                  VRC7,
                  FDS,
                  NES_DEVICE_MAX };

#define NES_CHANNEL_MAX 32

class NSFPlayer
{
  private:
    double rate;
    int nch; // number of channels
    int song;

    int last_out;
    int silent_length;

    double cpu_clock_rest;
    double apu_clock_rest;

    int time_in_ms;
    bool infinite;

    xgm::Bus apu_bus;
    xgm::Layer stack;
    xgm::Layer layer;
    xgm::Mixer mixer;
    xgm::Fader fader;

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
    xgm::InfoBuffer infobuf[NES_TRACK_MAX]; // �e�g���b�N�̏���ۑ�

    int total_render; // ����܂łɐ��������g�`�̃o�C�g��
    int frame_render; // �P�t���[�����̃o�C�g��
    int frame_in_ms;  // �P�t���[���̒���(ms)

    // �e�T�E���h�`�b�v�̃G�C���A�X�Q��
    xgm::NES_APU* apu;
    xgm::NES_DMC* dmc;
    xgm::NES_VRC6* vrc6;
    xgm::NES_FME7* fme7;
    xgm::NES_MMC5* mmc5;
    xgm::NES_N106* n106;
    xgm::NES_FDS* fds;
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
        sc[APU] = (apu = new xgm::NES_APU());
        sc[DMC] = (dmc = new xgm::NES_DMC());
        sc[FDS] = (fds = new xgm::NES_FDS());
        sc[FME7] = (fme7 = new xgm::NES_FME7());
        sc[MMC5] = (mmc5 = new xgm::NES_MMC5());
        sc[N106] = (n106 = new xgm::NES_N106());
        sc[VRC6] = (vrc6 = new xgm::NES_VRC6());
        nsf2_irq.SetCPU(&cpu);
        dmc->SetAPU(apu);
        dmc->SetCPU(&cpu);
        mmc5->SetCPU(&cpu);
        for (int i = 0; i < NES_DEVICE_MAX; i++) {
            if (i != VRC7) {
                amp[i].Attach(sc[i]);
            }
        }
        rconv.Attach(&mixer);
        fader.Attach(&rconv);
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
        int quality = 10;
        double clock;
        switch (region) {
            case REGION_NTSC: clock = 1789773; break;
            case REGION_PAL: clock = 1662607; break;
            case REGION_DENDY: clock = 1773448; break;
        }
        double oversample = rate * quality;
        if (oversample > clock) oversample = clock;
        if (oversample < rate) oversample = rate;
        for (int i = 0; i < NES_DEVICE_MAX; i++) {
            if (i != VRC7) {
                sc[i]->SetClock(clock);
                sc[i]->SetRate(oversample);
            }
        }
        rconv.SetClock(oversample);
        rconv.SetRate(rate);
        rconv.SetFastSkip(true);
        mixer.Reset();
        rconv.Reset();
        fader.Reset();
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
        apu_clock_rest = 0.0;
        cpu_clock_rest = 0.0;

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

        double speed;
        uint16_t nsfspeed = (region == REGION_DENDY) ? nsf->speed_dendy : (region == REGION_PAL) ? nsf->speed_pal
                                                                                                 : nsf->speed_ntsc;
        speed = 1000000.0 / nsfspeed;

        int song = nsf->song;
        int region_register = (region == REGION_PAL) ? 1 : 0;
        if (region == REGION_DENDY && (nsf->regn & 4)) region_register = 2; // use 2 for Dendy iff explicitly supported, otherwise 0

        cpu.Start(
            nsf->init_address,
            nsf->play_address,
            speed,
            song,
            region_register,
            nsf->nsf2_bits,
            false, // disable IRQ
            &nsf2_irq);

        apu->SetMask(0);
        dmc->SetMask(0);
        fds->SetMask(0);
        mmc5->SetMask(0);
        fme7->SetMask(0);
        vrc6->SetMask(0);
        n106->SetMask(0);

        for (int i = 0; i < NES_TRACK_MAX; i++) {
            infobuf[i].Clear();
        }

        // suppress starting click by setting DC filter to balance the starting level at 0
        int quality = 10;
        int32_t b[2];
        for (int i = 0; i < NES_DEVICE_MAX; ++i) {
            if (i != VRC7) {
                sc[i]->Tick(0); // determine starting state for all sound units
            }
        }
        fader.Tick(0);
        for (int i = 0; i < (quality + 1); ++i) {
            // warm up rconv/render with enough sample to reach a steady state
            fader.Render(b);
        }
    }

    uint32_t Render(int16_t* stream, int length)
    {
        int buf[2];
        int out[2];
        int outm;
        uint32_t i;
        int master_volume = 512;
        int mult_speed = 256;
        double apu_clock_per_sample = cpu.nes_basecycles / rate;
        double cpu_clock_per_sample = apu_clock_per_sample * ((double)(mult_speed) / 256.0);

        for (i = 0; i < (uint32_t)length; i++) {
            total_render++;
            // tick CPU
            cpu_clock_rest += cpu_clock_per_sample;
            int cpu_clocks = (int)(cpu_clock_rest);
            // Moved to RateConverter:
            // if (cpu_clocks > 0)
            //{
            //    UINT32 real_cpu_clocks = cpu.Exec ( cpu_clocks );
            //    cpu_clock_rest -= (double)(real_cpu_clocks);
            //
            //    // tick APU frame sequencer
            //    dmc->TickFrameSequence(real_cpu_clocks);
            //    if (nsf->use_mmc5)
            //        mmc5->TickFrameSequence(real_cpu_clocks);
            //}
            // UpdateInfo();
            rconv.TickCPU(cpu_clocks);
            cpu_clock_rest -= double(cpu_clocks);

            // tick fader and queue accumulated ticks for APU/CPU to be done during Render
            apu_clock_rest += apu_clock_per_sample;
            int apu_clocks = (int)(apu_clock_rest);
            if (apu_clocks > 0) {
                fader.Tick(apu_clocks);
                apu_clock_rest -= (double)(apu_clocks);
            }

            // render output
            fader.Render(buf);             // ticks APU/CPU and renders with subdivision and resampling (also does UpdateInfo)
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
            UpdateInfo();
        }
        time_in_ms += (int)(1000 * length / rate * mult_speed / 256);
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
        rconv.SetMMC5(NULL);

        if (nsf->use_mmc5) {
            stack.Attach(sc[MMC5]);
            mixer.Attach(&amp[MMC5]);
            rconv.SetMMC5(mmc5);
        }
        if (nsf->use_vrc6) {
            stack.Attach(sc[VRC6]);
            mixer.Attach(&amp[VRC6]);
        }
        if (nsf->use_fme7) {
            stack.Attach(sc[FME7]);
            mixer.Attach(&amp[FME7]);
        }
        if (nsf->use_n106) {
            stack.Attach(sc[N106]);
            mixer.Attach(&amp[N106]);
        }
        if (nsf->use_fds) {
            bool multichip =
                nsf->use_mmc5 ||
                nsf->use_vrc6 ||
                nsf->use_vrc7 ||
                nsf->use_fme7 ||
                nsf->use_n106;
            stack.Attach(sc[FDS]); // last before memory layer
            mixer.Attach(&amp[FDS]);
            mem.SetFDSMode(false);
            bank.SetFDSMode(false);
            if (!multichip) {
                bank.SetBankDefault(6, nsf->bankswitch[6]);
                bank.SetBankDefault(7, nsf->bankswitch[7]);
            }
        } else {
            mem.SetFDSMode(false);
            bank.SetFDSMode(false);
        }

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

    void UpdateInfo()
    {
        if (total_render % frame_render == 0) {
            int i;

            for (i = 0; i < 2; i++)
                infobuf[APU1_TRK0 + i].AddInfo(total_render, apu->GetTrackInfo(i));

            for (i = 0; i < 3; i++)
                infobuf[APU2_TRK0 + i].AddInfo(total_render, dmc->GetTrackInfo(i));

            if (nsf->use_fds)
                infobuf[FDS_TRK0].AddInfo(total_render, fds->GetTrackInfo(0));

            if (nsf->use_vrc6) {
                for (i = 0; i < 3; i++)
                    infobuf[VRC6_TRK0 + i].AddInfo(total_render, vrc6->GetTrackInfo(i));
            }

            if (nsf->use_n106) {
                for (i = 0; i < 8; i++)
                    infobuf[N106_TRK0 + i].AddInfo(total_render, n106->GetTrackInfo(i));
            }

            if (nsf->use_vrc7) {
                // remove vrc7 support
            }

            if (nsf->use_mmc5) {
                for (i = 0; i < 3; i++)
                    infobuf[MMC5_TRK0 + i].AddInfo(total_render, mmc5->GetTrackInfo(i));
            }

            if (nsf->use_fme7) {
                for (i = 0; i < 5; i++)
                    infobuf[FME7_TRK0 + i].AddInfo(total_render, fme7->GetTrackInfo(i));
            }
        }
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