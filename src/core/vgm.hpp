// VGM Manager
// Copyright (c) 2025, Yoji Suzuki
// License under GPLv3
#include "nesvgm.hpp"
#include "emu76489.hpp"

class VgmManager
{
  private:
    enum EmulatorType {
        ET_NES = 0,
        ET_DCSG,
        ET_Length
    };

    struct Emulator {
        xgm::NesVgmDriver* nes;
        EMU76489* dcsg;
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
    }

    ~VgmManager()
    {
        delete emu.nes;
        delete emu.dcsg;
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

        if (vgm.clocks[ET_NES]) {
            emu.nes->Load(data, size);
            emu.nes->SetPlayFreq(44100);
            emu.nes->SetChannels(1);
            emu.nes->Reset();
        } else {
            memcpy(&vgm.cursor, &data[0x34], 4);
            vgm.cursor += 0x40 - 0x0C;
            memcpy(&vgm.loopOffset, &data[0x1C], 4);
            vgm.loopOffset += vgm.loopOffset ? 0x1C : 0;
        }
        return true;
    }

    void reset()
    {
        memset(&vgm, 0, sizeof(vgm));
        emu.nes->Reset();
        emu.dcsg->reset();
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
                case 0x4F:
                    emu.dcsg->writeGGIO(vgm.data[vgm.cursor++]);
                    break;
                case 0x50:
                    emu.dcsg->writeIO(vgm.data[vgm.cursor++]);
                    break;
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
