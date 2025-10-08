// VGM Manager
// Copyright (c) 2025, Yoji Suzuki
// License under GPLv3
#include "nesvgm.hpp"
#include "emu76489.hpp"
#include "emu2149.hpp"
#include "emu2212.hpp"
#include "ymfm_opn2.hpp"

class VgmManager : public ymfm::ymfm_interface
{
  private:
    enum EmulatorType {
        ET_NES = 0,
        ET_DCSG,
        ET_PSG,
        ET_SCC,
        ET_OPN2,
        ET_Length
    };

    struct Emulator {
        xgm::NesVgmDriver* nes;
        EMU76489* dcsg;
        EMU2149* psg;
        EMU2212* scc;
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
    } ym;
    struct YM2612QueueEntry {
        uint32_t reg;
        uint8_t data;
    };
    enum { YM2612_QUEUE_CAPACITY = 1024 };
    YM2612QueueEntry ym2612_queue[YM2612_QUEUE_CAPACITY];
    int ym2612_queue_head;
    int ym2612_queue_tail;

  public:
    VgmManager() : ym2612(*this)
    {
        emu.nes = new xgm::NesVgmDriver();
        emu.dcsg = new EMU76489(3579545, 44100);
        emu.psg = new EMU2149(3579545, 44100);
        emu.scc = new EMU2212(3579545, 44100);
        ym2612_queue_reset();
    }

    ~VgmManager()
    {
        delete emu.nes;
        delete emu.dcsg;
        delete emu.psg;
        delete emu.scc;
    }

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
            emu.nes->Load(data, size);
            emu.nes->SetPlayFreq(44100);
            emu.nes->SetChannels(1);
            emu.nes->Reset();
            return true;
        }

        if (vgm.clocks[ET_PSG]) {
            emu.psg->setVolumeMode(2);
            emu.psg->setClockDivider(1);
        }

        if (vgm.clocks[ET_SCC]) {
            emu.scc->set_type(EMU2212::Type::Standard);
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
        emu.nes->Reset();
        emu.dcsg->reset();
        emu.psg->reset();
        emu.scc->reset();
        ym2612.reset();
        ym2612_queue_reset();
    }

    void render(int16_t* buf, int samples)
    {
        if (!vgm.data) {
            memset(buf, 0, samples * 2);
            return;
        }
        if (vgm.clocks[ET_NES]) {
            // Execute NES only
            emu.nes->Render(buf, samples);
            return;
        }
        int cursor = 0;
        while (cursor < samples) {
            if (vgm.wait < 1) {
                this->execute();
            }
            vgm.wait--;
            buf[cursor] = 0;
            if (vgm.clocks[ET_DCSG]) {
                buf[cursor] += emu.dcsg->calc() << 1;
            }
            if (vgm.clocks[ET_PSG]) {
                buf[cursor] += emu.psg->calc() << 1;
            }
            if (vgm.clocks[ET_SCC]) {
                buf[cursor] += emu.scc->calc() << 1;
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
                ymfm::ym2612::output_data out;
                for (; ym.pos <= ym.output_start; ym.pos += ym.step) {
                    ym2612.generate(&out);
                }
                ym.output_start += 0x100000000ull / 44100;
                buf[cursor] += out.data[0];
            }
            cursor++;
        }
    }

  private:
    void execute()
    {
        if (!vgm.data || vgm.end) {
            return;
        }
        while (vgm.wait < 1) {
            uint8_t cmd = vgm.data[vgm.cursor++];
            switch (cmd) {
                case 0x4F: // SN76489 GG I/O
                    emu.dcsg->writeGGIO(vgm.data[vgm.cursor++]);
                    break;
                case 0x50: // SN76489 register
                    emu.dcsg->writeIO(vgm.data[vgm.cursor++]);
                    break;
                case 0x31: // AY-3-8910 stereo mask (ignore)
                    vgm.cursor++;
                    // emu.psg->setMask(vgm.data[vgm.cursor++]);
                    break;
                case 0xA0: {
                    // AY-3-8910 reigster
                    uint8_t addr = vgm.data[vgm.cursor++];
                    uint8_t value = vgm.data[vgm.cursor++];
                    emu.psg->writeReg(addr, value);
                    break;
                }
                case 0xD2: {
                    // SCC1
                    uint8_t port = vgm.data[vgm.cursor++] & 0x7F;
                    uint8_t offset = vgm.data[vgm.cursor++];
                    uint8_t data = vgm.data[vgm.cursor++];
                    switch (port) {
                        case 0x00: emu.scc->write_waveform1(offset, data); break;
                        case 0x01: emu.scc->write_frequency(offset, data); break;
                        case 0x02: emu.scc->write_volume(offset, data); break;
                        case 0x03: emu.scc->write_keyoff(data); break;
                        case 0x04: emu.scc->write_waveform2(offset, data); break;
                        case 0x05: emu.scc->write_test(data); break;
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
