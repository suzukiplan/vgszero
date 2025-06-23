// VGM Manager
// Copyright (c) 2025, Yoji Suzuki
// License under GPLv3
#include "nesvgm.hpp"
#include "emu76489.hpp"
#include "emu2149.hpp"
#include "emu2212.hpp"

class VgmManager
{
  private:
    enum EmulatorType {
        ET_NES = 0,
        ET_DCSG,
        ET_PSG,
        ET_SCC,
        ET_Length
    };

    struct Emulator {
        xgm::NesVgmDriver* nes;
        EMU76489* dcsg;
        EMU2149* psg;
        EMU2212* scc;
    } emu;

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

  public:
    VgmManager()
    {
        emu.nes = new xgm::NesVgmDriver();
        emu.dcsg = new EMU76489(3579545, 44100);
        emu.psg = new EMU2149(3579545, 44100);
        emu.scc = new EMU2212(3579545, 44100);
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
            emu.psg->setQuality(1);
        }

        if (vgm.clocks[ET_SCC]) {
            emu.scc->set_type(EMU2212::Type::Standard);
            emu.scc->set_quality(1);
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

                default:
                    // unsupported command
                    printf("Detected an unknown VGM command: %02X\n", cmd);
                    vgm.end = true;
                    return;
            }
        }
    }
};
