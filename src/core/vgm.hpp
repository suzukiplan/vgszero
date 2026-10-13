// VGM Manager
// Copyright (c) 2025, Yoji Suzuki
// License under GPLv3
#include "nesvgm.hpp"
#include "emu76489.hpp"
#include "emu2149.hpp"
#include "emu2212.hpp"
#include "ymfm_opn2.hpp"
#include <math.h>

class VgmManager : public ymfm::ymfm_interface
{
  public:
    struct Ym2612AnalogConfig {
        bool enabled;
        float hpAlpha;
        float lpAlpha;
        float asymPosGain;
        float asymNegGain;
        float asymPosCurve;
        float asymNegCurve;
        float postLpAlpha;
        bool notchEnabled;
        float notchFrequencyHz;
        float notchQ;
        float notchMix;
        bool busSaturationEnabled;
        float saturatorDrive;
        float outputGain;
    };

  private:
    enum EmulatorType {
        ET_NES = 0,
        ET_DCSG,
        ET_PSG,
        ET_SCC,
        ET_OPN2,
        ET_Length
    };

    class Emulator
    {
      public:
        Emulator() : nes(),
                     dcsg(3579545, 44100),
                     psg(3579545, 44100),
                     scc(3579545, 44100) {}
        xgm::NesVgmDriver nes;
        EMU76489 dcsg;
        EMU2149 psg;
        EMU2212 scc;
    } emu;

    ymfm::ym2612 ym2612;

    struct VgmContext {
        uint32_t clocks[ET_Length];
        const uint8_t* data;
        size_t size;
        int version;
        int cursor;
        int loopOffset;
        int wait;
        bool end;
    } vgm;

    struct YM2612Context {
        uint64_t output_start;
        uint64_t pos;
        uint64_t step;
        bool resampler_primed;
        uint64_t previous_time;
        uint64_t next_time;
        int32_t previous_sample[2];
        int32_t next_sample[2];
    } ym;
    struct YM2612QueueEntry {
        uint32_t reg;
        uint8_t data;
    };
    enum { YM2612_QUEUE_CAPACITY = 1024 };
    YM2612QueueEntry ym2612_queue[YM2612_QUEUE_CAPACITY];
    int ym2612_queue_head;
    int ym2612_queue_tail;

    struct Ym2612AnalogNotchCoefficients {
        float b0;
        float b1;
        float b2;
        float a1;
        float a2;
    } ym2612AnalogNotch;

    struct Ym2612AnalogState {
        float hpLastInput[2];
        float hpLastOutput[2];
        float lpLastOutput[2];
        float postLpLastOutput[2];
        float notchInput1[2];
        float notchInput2[2];
        float notchOutput1[2];
        float notchOutput2[2];
    } ym2612AnalogState;

    Ym2612AnalogConfig ym2612AnalogConfig;
    float dcCutLastInput[2];
    float dcCutLastOutput[2];

  public:
    VgmManager() : ym2612(*this)
    {
        this->ym2612AnalogConfig = this->makeYm2612AnalogRealPreset();
        this->ym2612AnalogConfig.enabled = false;
        this->updateYm2612AnalogNotchCoefficients();
        this->resetYm2612OutputState();
        ym2612_queue_reset();
    }

    VgmManager(const VgmManager&) = delete;
    VgmManager& operator=(const VgmManager&) = delete;

    static Ym2612AnalogConfig makeYm2612AnalogCleanPreset()
    {
        return {
            false,
            0.9965f,
            0.40f,
            1.0f,
            1.0f,
            0.0f,
            0.0f,
            1.0f,
            false,
            4900.0f,
            2.5f,
            0.0f,
            false,
            1.0f,
            1.0f,
        };
    }

    static Ym2612AnalogConfig makeYm2612AnalogSubtlePreset()
    {
        return {
            true,
            0.9965f,
            0.34f,
            1.000f,
            0.994f,
            0.010f,
            0.016f,
            1.0f,
            false,
            4900.0f,
            2.5f,
            0.0f,
            true,
            1.05f,
            0.985f,
        };
    }

    static Ym2612AnalogConfig makeYm2612AnalogRealPreset()
    {
        return {
            true,
            0.9971f,
            0.62f,
            1.000f,
            0.996f,
            0.006f,
            0.009f,
            0.94f,
            true,
            4900.0f,
            9.0f,
            0.020f,
            true,
            1.006f,
            0.940f,
        };
    }

    static Ym2612AnalogConfig makeYm2612AnalogRe1ePreset()
    {
        return {
            true,
            0.995f,
            0.9985f,
            1.000f,
            1.000f,
            0.0f,
            0.0f,
            1.0f,
            false,
            4900.0f,
            9.0f,
            0.0f,
            false,
            1.000f,
            1.0f,
        };
    }

    static Ym2612AnalogConfig makeYm2612AnalogWarmPreset()
    {
        return {
            true,
            0.9955f,
            0.27f,
            1.002f,
            0.990f,
            0.016f,
            0.024f,
            1.0f,
            false,
            4900.0f,
            2.5f,
            0.0f,
            true,
            1.10f,
            0.965f,
        };
    }

    void setYm2612AnalogConfig(const Ym2612AnalogConfig& config)
    {
        this->ym2612AnalogConfig = config;
        this->clampYm2612AnalogConfig();
        this->updateYm2612AnalogNotchCoefficients();
        this->resetYm2612AnalogState();
    }

    const Ym2612AnalogConfig& getYm2612AnalogConfig() const
    {
        return this->ym2612AnalogConfig;
    }

    bool isYm2612AnalogEnabled() const
    {
        return this->ym2612AnalogConfig.enabled;
    }

    void setYm2612AnalogEnabled(bool enabled)
    {
        this->ym2612AnalogConfig.enabled = enabled;
        this->resetYm2612AnalogState();
    }

    void useYm2612AnalogCleanPreset() { this->setYm2612AnalogConfig(this->makeYm2612AnalogCleanPreset()); }
    void useYm2612AnalogSubtlePreset() { this->setYm2612AnalogConfig(this->makeYm2612AnalogSubtlePreset()); }
    void useYm2612AnalogRealPreset() { this->setYm2612AnalogConfig(this->makeYm2612AnalogRealPreset()); }
    void useYm2612AnalogRe1ePreset() { this->setYm2612AnalogConfig(this->makeYm2612AnalogRe1ePreset()); }
    void useYm2612AnalogWarmPreset() { this->setYm2612AnalogConfig(this->makeYm2612AnalogWarmPreset()); }

    bool load(const uint8_t* data, size_t size)
    {
        this->reset();
        if (size < 0x100) {
            return false;
        }
        if (0 != memcmp("Vgm ", data, 4)) {
            return false;
        }

        memcpy(&vgm.version, &data[0x08], 4);
        if (vgm.version < 0x161) {
            return false;
        }

        vgm.data = data;
        vgm.size = size;

        memcpy(&vgm.clocks[ET_NES], &data[0x84], 4);
        memcpy(&vgm.clocks[ET_DCSG], &data[0x0C], 4);
        memcpy(&vgm.clocks[ET_PSG], &data[0x74], 4);
        memcpy(&vgm.clocks[ET_SCC], &data[0x9C], 4);

        if (vgm.clocks[ET_NES]) {
            emu.nes.Load(data, size);
            emu.nes.SetPlayFreq(44100);
            emu.nes.SetChannels(1);
            emu.nes.Reset();
            return true;
        }

        if (vgm.clocks[ET_PSG]) {
            emu.psg.setVolumeMode(2);
            emu.psg.setClockDivider(1);
        }

        if (vgm.clocks[ET_SCC]) {
            emu.scc.set_type(EMU2212::Type::Standard);
        }

        memcpy(&vgm.clocks[ET_OPN2], &data[0x2C], 4);
        if (vgm.clocks[ET_OPN2]) {
            memset(&ym, 0, sizeof(ym));
            ym2612.reset();
            ym.step = 0x100000000ull / ym2612.sample_rate(vgm.clocks[ET_OPN2]);
            ym2612_queue_reset();
        }

        memcpy(&vgm.cursor, &data[0x34], 4);
        vgm.cursor += 0x40 - 0x0C;
        memcpy(&vgm.loopOffset, &data[0x1C], 4);
        vgm.loopOffset += vgm.loopOffset ? 0x1C : 0;
        return true;
    }

    void reset()
    {
        memset(&vgm, 0, sizeof(vgm));
        emu.nes.Reset();
        emu.dcsg.reset();
        emu.psg.reset();
        emu.scc.reset();
        ym2612.reset();
        ym2612_queue_reset();
        this->resetYm2612OutputState();
    }

    void render(int16_t* buf, int samples)
    {
        if (!vgm.data) {
            memset(buf, 0, samples * 2);
            return;
        }
        if (vgm.clocks[ET_NES]) {
            // Execute NES only
            emu.nes.Render(buf, samples);
            return;
        }
        int cursor = 0;
        while (cursor < samples) {
            if (vgm.wait < 1) {
                this->execute();
            }
            vgm.wait--;
            int32_t mixed = 0;
            if (vgm.clocks[ET_DCSG]) {
                mixed += emu.dcsg.calc() << 1;
            }
            if (vgm.clocks[ET_PSG]) {
                mixed += emu.psg.calc() << 1;
            }
            if (vgm.clocks[ET_SCC]) {
                mixed += emu.scc.calc() << 1;
            }
            if (vgm.clocks[ET_OPN2]) {
                uint32_t addr1 = 0xffff, addr2 = 0xffff;
                uint8_t data1 = 0, data2 = 0;
                uint32_t reg;
                uint8_t value;
                if (ym2612_queue_pop(&reg, &value)) {
                    addr1 = 0 + 2 * ((reg >> 8) & 3);
                    data1 = reg & 0xff;
                    addr2 = addr1 + 1;
                    data2 = value;
                }
                if (addr1 != 0xffff) {
                    ym2612.write(addr1, data1);
                    ym2612.write(addr2, data2);
                }
                int32_t opn2[2] = {0, 0};
                this->sampleYm2612At(ym.output_start, opn2);
                ym.output_start += 0x100000000ull / 44100;
                mixed += this->postProcessYm2612Sample(0, opn2[0]);
            }
            if (mixed < -32768) {
                mixed = -32768;
            } else if (32767 < mixed) {
                mixed = 32767;
            }
            buf[cursor] = static_cast<int16_t>(mixed);
            cursor++;
        }
    }

  private:
    static float clampFloat(float value, float minimum, float maximum)
    {
        if (value < minimum)
            return minimum;
        if (maximum < value)
            return maximum;
        return value;
    }

    static float clampUnit(float value)
    {
        return clampFloat(value, -1.0f, 1.0f);
    }

    static int32_t roundToInt(float value)
    {
        return static_cast<int32_t>(value < 0.0f ? value - 0.5f : value + 0.5f);
    }

    void resetYm2612AnalogState()
    {
        memset(&this->ym2612AnalogState, 0, sizeof(this->ym2612AnalogState));
    }

    void resetYm2612OutputState()
    {
        memset(&this->ym, 0, sizeof(this->ym));
        memset(this->dcCutLastInput, 0, sizeof(this->dcCutLastInput));
        memset(this->dcCutLastOutput, 0, sizeof(this->dcCutLastOutput));
        this->resetYm2612AnalogState();
    }

    void clampYm2612AnalogConfig()
    {
        this->ym2612AnalogConfig.hpAlpha = clampFloat(this->ym2612AnalogConfig.hpAlpha, 0.0f, 1.0f);
        this->ym2612AnalogConfig.lpAlpha = clampFloat(this->ym2612AnalogConfig.lpAlpha, 0.0f, 1.0f);
        this->ym2612AnalogConfig.asymPosGain = clampFloat(this->ym2612AnalogConfig.asymPosGain, 0.5f, 1.5f);
        this->ym2612AnalogConfig.asymNegGain = clampFloat(this->ym2612AnalogConfig.asymNegGain, 0.5f, 1.5f);
        this->ym2612AnalogConfig.asymPosCurve = clampFloat(this->ym2612AnalogConfig.asymPosCurve, 0.0f, 0.25f);
        this->ym2612AnalogConfig.asymNegCurve = clampFloat(this->ym2612AnalogConfig.asymNegCurve, 0.0f, 0.25f);
        this->ym2612AnalogConfig.postLpAlpha = clampFloat(this->ym2612AnalogConfig.postLpAlpha, 0.0f, 1.0f);
        this->ym2612AnalogConfig.notchFrequencyHz = clampFloat(this->ym2612AnalogConfig.notchFrequencyHz, 1000.0f, 12000.0f);
        this->ym2612AnalogConfig.notchQ = clampFloat(this->ym2612AnalogConfig.notchQ, 0.2f, 12.0f);
        this->ym2612AnalogConfig.notchMix = clampFloat(this->ym2612AnalogConfig.notchMix, 0.0f, 1.0f);
        this->ym2612AnalogConfig.saturatorDrive = clampFloat(this->ym2612AnalogConfig.saturatorDrive, 1.0f, 2.0f);
        this->ym2612AnalogConfig.outputGain = clampFloat(this->ym2612AnalogConfig.outputGain, 0.0f, 2.0f);
    }

    void updateYm2612AnalogNotchCoefficients()
    {
        constexpr float outputSampleRate = 44100.0f;
        constexpr float pi = 3.14159265358979323846f;
        const float omega = 2.0f * pi * this->ym2612AnalogConfig.notchFrequencyHz / outputSampleRate;
        const float alpha = sinf(omega) / (2.0f * this->ym2612AnalogConfig.notchQ);
        const float cosOmega = cosf(omega);
        const float a0 = 1.0f + alpha;

        this->ym2612AnalogNotch.b0 = 1.0f / a0;
        this->ym2612AnalogNotch.b1 = (-2.0f * cosOmega) / a0;
        this->ym2612AnalogNotch.b2 = 1.0f / a0;
        this->ym2612AnalogNotch.a1 = (-2.0f * cosOmega) / a0;
        this->ym2612AnalogNotch.a2 = (1.0f - alpha) / a0;
    }

    float applyYm2612AnalogNotch(size_t channel, float value)
    {
        if (!this->ym2612AnalogConfig.notchEnabled || this->ym2612AnalogConfig.notchMix <= 0.0f)
            return value;

        const Ym2612AnalogNotchCoefficients& notch = this->ym2612AnalogNotch;
        const float filtered = (notch.b0 * value) + (notch.b1 * this->ym2612AnalogState.notchInput1[channel]) + (notch.b2 * this->ym2612AnalogState.notchInput2[channel]) - (notch.a1 * this->ym2612AnalogState.notchOutput1[channel]) - (notch.a2 * this->ym2612AnalogState.notchOutput2[channel]);

        this->ym2612AnalogState.notchInput2[channel] = this->ym2612AnalogState.notchInput1[channel];
        this->ym2612AnalogState.notchInput1[channel] = value;
        this->ym2612AnalogState.notchOutput2[channel] = this->ym2612AnalogState.notchOutput1[channel];
        this->ym2612AnalogState.notchOutput1[channel] = filtered;

        return (value * (1.0f - this->ym2612AnalogConfig.notchMix)) + (filtered * this->ym2612AnalogConfig.notchMix);
    }

    float applyLegacyDcCut(size_t channel, float value)
    {
        const float filtered = value - this->dcCutLastInput[channel] + (0.995f * this->dcCutLastOutput[channel]);
        this->dcCutLastInput[channel] = value;
        this->dcCutLastOutput[channel] = filtered;
        return filtered;
    }

    float applyYm2612AnalogPipeline(size_t channel, float sample)
    {
        const Ym2612AnalogConfig& cfg = this->ym2612AnalogConfig;
        float value = sample - this->ym2612AnalogState.hpLastInput[channel] + (cfg.hpAlpha * this->ym2612AnalogState.hpLastOutput[channel]);
        this->ym2612AnalogState.hpLastInput[channel] = sample;
        this->ym2612AnalogState.hpLastOutput[channel] = value;

        if (value >= 0.0f) {
            value = (value * cfg.asymPosGain) + (cfg.asymPosCurve * value * value);
        } else {
            value = (value * cfg.asymNegGain) - (cfg.asymNegCurve * value * value);
        }
        value = clampUnit(value);

        float& lp = this->ym2612AnalogState.lpLastOutput[channel];
        lp += cfg.lpAlpha * (value - lp);
        value = lp;

        if (cfg.busSaturationEnabled) {
            const float driven = value * cfg.saturatorDrive;
            value = driven / (1.0f + ((cfg.saturatorDrive - 1.0f) * fabsf(driven)));
        }

        float& postLp = this->ym2612AnalogState.postLpLastOutput[channel];
        postLp += cfg.postLpAlpha * (value - postLp);
        value = this->applyYm2612AnalogNotch(channel, postLp);
        return clampUnit(value * cfg.outputGain);
    }

    void generateYm2612Output(int32_t sample[2])
    {
        ymfm::ym2612::output_data out;
        this->ym2612.generate(&out);
        sample[0] = out.data[0];
        sample[1] = out.data[1];
    }

    void primeYm2612Resampler()
    {
        this->generateYm2612Output(this->ym.previous_sample);
        this->ym.previous_time = 0;
        this->ym.pos = this->ym.step;
        this->generateYm2612Output(this->ym.next_sample);
        this->ym.next_time = this->ym.pos;
        this->ym.pos += this->ym.step;
        this->ym.resampler_primed = true;
    }

    void advanceYm2612Resampler(uint64_t targetTime)
    {
        if (!this->ym.resampler_primed)
            this->primeYm2612Resampler();
        while (this->ym.next_time <= targetTime) {
            this->ym.previous_time = this->ym.next_time;
            memcpy(this->ym.previous_sample, this->ym.next_sample, sizeof(this->ym.previous_sample));
            this->generateYm2612Output(this->ym.next_sample);
            this->ym.next_time = this->ym.pos;
            this->ym.pos += this->ym.step;
        }
    }

    void sampleYm2612At(uint64_t targetTime, int32_t mixed[2])
    {
        this->advanceYm2612Resampler(targetTime);
        const uint64_t interval = this->ym.next_time - this->ym.previous_time;
        double fraction = 0.0;
        if (interval > 0)
            fraction = static_cast<double>(targetTime - this->ym.previous_time) / static_cast<double>(interval);
        if (fraction < 0.0)
            fraction = 0.0;
        else if (1.0 < fraction)
            fraction = 1.0;

        for (int channel = 0; channel < 2; channel++) {
            const double previous = static_cast<double>(this->ym.previous_sample[channel]);
            const double next = static_cast<double>(this->ym.next_sample[channel]);
            const double value = previous + ((next - previous) * fraction);
            mixed[channel] += static_cast<int32_t>(value < 0.0 ? value - 0.5 : value + 0.5);
        }
    }

    int16_t postProcessYm2612Sample(size_t channel, int32_t sample)
    {
        float value = static_cast<float>(sample) * 1.55f;
        if (this->ym2612AnalogConfig.enabled) {
            value = this->applyYm2612AnalogPipeline(channel, value / 32768.0f) * 32768.0f;
        } else {
            value = this->applyLegacyDcCut(channel, value);
        }
        value = clampFloat(value, -32768.0f, 32767.0f);
        return static_cast<int16_t>(roundToInt(value));
    }

    void execute()
    {
        if (!vgm.data || vgm.end) {
            return;
        }
        while (vgm.wait < 1) {
            uint8_t cmd = vgm.data[vgm.cursor++];
            switch (cmd) {
                case 0x4F: // SN76489 GG I/O
                    emu.dcsg.writeGGIO(vgm.data[vgm.cursor++]);
                    break;
                case 0x50: // SN76489 register
                    emu.dcsg.writeIO(vgm.data[vgm.cursor++]);
                    break;
                case 0x31: // AY-3-8910 stereo mask (ignore)
                    vgm.cursor++;
                    // emu.psg.setMask(vgm.data[vgm.cursor++]);
                    break;
                case 0xA0: {
                    // AY-3-8910 reigster
                    uint8_t addr = vgm.data[vgm.cursor++];
                    uint8_t value = vgm.data[vgm.cursor++];
                    emu.psg.writeReg(addr, value);
                    break;
                }
                case 0xD2: {
                    // SCC1
                    uint8_t port = vgm.data[vgm.cursor++] & 0x7F;
                    uint8_t offset = vgm.data[vgm.cursor++];
                    uint8_t data = vgm.data[vgm.cursor++];
                    switch (port) {
                        case 0x00: emu.scc.write_waveform1(offset, data); break;
                        case 0x01: emu.scc.write_frequency(offset, data); break;
                        case 0x02: emu.scc.write_volume(offset, data); break;
                        case 0x03: emu.scc.write_keyoff(data); break;
                        case 0x04: emu.scc.write_waveform2(offset, data); break;
                        case 0x05: emu.scc.write_test(data); break;
                    }
                    break;
                }

                case 0x52:
                case 0xA2: {
                    // YM2612 port 0, write value dd to register aa
                    uint32_t reg = vgm.data[vgm.cursor++];
                    uint8_t data = vgm.data[vgm.cursor++];
                    ym2612_queue_push(reg, data);
                    break;
                }
                case 0x53:
                case 0xA3: {
                    // YM2612 port 1, write value dd to register aa
                    uint32_t reg = vgm.data[vgm.cursor++];
                    uint8_t data = vgm.data[vgm.cursor++];
                    ym2612_queue_push(reg | 0x100, data);
                    break;
                }

                case 0x61: {
                    // Wait nn samples
                    unsigned short nn;
                    memcpy(&nn, &vgm.data[vgm.cursor], 2);
                    vgm.cursor += 2;
                    vgm.wait += nn;
                    break;
                }
                case 0x62: vgm.wait += 735; break;
                case 0x63: vgm.wait += 882; break;
                case 0x66: {
                    // End of sound data
                    if (vgm.loopOffset) {
                        vgm.cursor = vgm.loopOffset;
                        break;
                    } else {
                        vgm.end = true;
                        return;
                    }
                }

                case 0xDD:
                case 0xDE:
                case 0xDF:
                case 0xFD:
                case 0xFE:
                case 0xFF:
                    // Skip: Furnace outputs thies unsupport commands (use for labels?)
                    break;

                case 0x90: // Setup Stream Control: 0x90 ss tt pp cc (ignore)
                case 0x91: // Set Stream Data: 0x91 ss dd ll bb (ignore)
                case 0x95: // Start Stream (fast call): 0x95 ss bb bb ff
                    // printf("[DAC]0x%02X: %02X %02X %02X %02X\n", cmd, vgm.data[0], vgm.data[1], vgm.data[2], vgm.data[3]);
                    vgm.cursor += 4;
                    break;

                case 0x92: // Set Stream Frequency: 0x92 ss ff ff ff ff (ignore)
                    // printf("[DAC]0x%02X: %02X %02X %02X %02X %02X\n", cmd, vgm.data[0], vgm.data[1], vgm.data[2], vgm.data[3], vgm.data[4]);
                    vgm.cursor += 5;
                    break;

                case 0x93: // Start Stream: 0x93 ss aa aa aa aa mm ll ll ll ll (ignored)
                    // printf("[DAC]0x%02X: %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X\n", cmd, vgm.data[0], vgm.data[1], vgm.data[2], vgm.data[3], vgm.data[4], vgm.data[5], vgm.data[6], vgm.data[7], vgm.data[8], vgm.data[9]);
                    vgm.cursor += 10;
                    break;

                case 0x94: // Stop Stream: 0x94 ss
                    // printf("[DAC]0x%02X: %02X\n", cmd, vgm.data[0]);
                    vgm.cursor++;
                    break;

                default:
                    // unsupported command
                    printf("Detected an unknown VGM command: %02X\n", cmd);
                    vgm.end = true;
                    return;
            }
        }
    }

    void ym2612_queue_reset()
    {
        ym2612_queue_head = 0;
        ym2612_queue_tail = 0;
    }

    bool ym2612_queue_pop(uint32_t* reg, uint8_t* data)
    {
        if (ym2612_queue_head == ym2612_queue_tail) {
            return false;
        }
        *reg = ym2612_queue[ym2612_queue_head].reg;
        *data = ym2612_queue[ym2612_queue_head].data;
        ym2612_queue_head = (ym2612_queue_head + 1) % YM2612_QUEUE_CAPACITY;
        return true;
    }

    void ym2612_queue_push(uint32_t reg, uint8_t data)
    {
        int next_tail = (ym2612_queue_tail + 1) % YM2612_QUEUE_CAPACITY;
        if (next_tail == ym2612_queue_head) {
            // Drop the oldest entry to prevent overflow
            ym2612_queue_head = (ym2612_queue_head + 1) % YM2612_QUEUE_CAPACITY;
        }
        ym2612_queue[ym2612_queue_tail].reg = reg;
        ym2612_queue[ym2612_queue_tail].data = data;
        ym2612_queue_tail = next_tail;
    }
};
