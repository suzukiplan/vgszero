/**
 * VGS-Zero - Core Emulator
 * License under GPLv3: https://github.com/suzukiplan/vgszero/blob/master/LICENSE-VGS0.txt
 * (C)2023, SUZUKI PLAN
 */

#ifndef INCLUDE_VGS0_HPP
#define INCLUDE_VGS0_HPP
#include "vdp.hpp"
#include "vgs0def.h"
#include "vgsdecv.hpp"
#include "z80.hpp"

class VGS0
{
  private:
    class Binary
    {
      public:
        const unsigned char* data;
        size_t size;

        Binary()
        {
            this->data = nullptr;
            this->size = 0;
            ;
        }
    };

    class SoundEffect
    {
      public:
        short* data;
        size_t count;

        SoundEffect()
        {
            this->data = nullptr;
            this->count = 0;
        }
    };

    Binary rom;
    Binary bgm[256];
    SoundEffect se[256];

  public:
    Z80* cpu;
    VDP* vdp;
    VGSDecoder* vgsdec;

    struct Context {
        int64_t bobo;
        unsigned char ram[0x4000];
        unsigned char romBank[4];
        unsigned char pad;
        unsigned char reserved[3];
        struct BgmContext {
            bool playing;
            bool fadeout;
            int playingIndex;
            unsigned int seekPosition;
        } bgm;
        struct SoundEffectContext {
            bool playing;
            int playingIndex;
        } se[256];
    } ctx;

    VGS0(VDP::ColorMode colorMode = VDP::ColorMode::RGB555)
    {
        this->cpu = new Z80([](void* arg, unsigned short addr) { return ((VGS0*)arg)->readMemory(addr); }, [](void* arg, unsigned short addr, unsigned char value) { ((VGS0*)arg)->writeMemory(addr, value); }, [](void* arg, unsigned short port) { return ((VGS0*)arg)->inPort(port); }, [](void* arg, unsigned short port, unsigned char value) { ((VGS0*)arg)->outPort(port, value); }, this);
        this->cpu->setConsumeClockCallback([](void* arg, int clocks) { ((VGS0*)arg)->consumeClock(clocks); });
        this->vdp = new VDP(
            colorMode, this, [](void* arg) { ((VGS0*)arg)->cpu->requestBreak(); }, [](void* arg) { ((VGS0*)arg)->cpu->generateIRQ(0x07); });
        this->vgsdec = new VGSDecoder();
        this->reset();
    }

    ~VGS0()
    {
        delete this->vgsdec;
        delete this->vdp;
        delete this->cpu;
    }

    void setExternalRenderingCallback(void (*externalRedneringCallback)(void* arg))
    {
        this->vdp->externalRedneringCallback = externalRedneringCallback;
    }

    void executeExternalRendering()
    {
        this->vdp->externalRendering();
    }

    void reset()
    {
        memset(&this->ctx, 0, sizeof(this->ctx));
        for (int i = 0; i < 4; i++) {
            this->ctx.romBank[i] = i;
        }
        this->vdp->reset();
        memset(&this->cpu->reg, 0, sizeof(this->cpu->reg));
        this->cpu->reg.SP = 0xFFFF;
    }

    void loadRom(const void* data, size_t size)
    {
        this->rom.data = (const unsigned char*)data;
        this->rom.size = size & 0x001FFFFF; // max 2MB
        this->rom.size -= size % 0x2000;    // ignore additional not 8KB data
    }

    void loadBgm(const void* buffer, size_t size)
    {
        memset(&this->bgm, 0, sizeof(this->bgm));
        const unsigned char* ptr = (const unsigned char*)buffer;
        if (0 != memcmp(ptr, "VGS0BGM", 8)) {
            return; // invalid eye-catch
        }
        ptr += 8;
        int count;
        memcpy(&count, ptr, 4);
        ptr += 4;
        if (count < 0 || 256 < count) {
            return; // invalid count
        }
        int sizes[256];
        for (int i = 0; i < count; i++) {
            memcpy(&sizes[i], ptr, 4);
            ptr += 4;
        }
        for (int i = 0; i < count; i++) {
            this->bgm[i].data = ptr;
            this->bgm[i].size = sizes[i];
            ptr += sizes[i];
        }
    }

    void loadSoundEffect(const void* buffer, size_t size)
    {
        memset(&this->se, 0, sizeof(this->se));
        const unsigned char* ptr = (const unsigned char*)buffer;
        if (0 != memcmp(ptr, "VGS0EFF", 8)) {
            return; // invalid eye-catch
        }
        ptr += 8;
        int count;
        memcpy(&count, ptr, 4);
        ptr += 4;
        if (count < 0 || 256 < count) {
            return; // invalid count
        }
        int sizes[256];
        for (int i = 0; i < count; i++) {
            memcpy(&sizes[i], ptr, 4);
            ptr += 4;
        }
        for (int i = 0; i < count; i++) {
            this->se[i].data = (short*)ptr;
            this->se[i].count = sizes[i] / 2;
            ptr += sizes[i];
        }
    }

    void tick(unsigned char pad)
    {
        this->ctx.pad = 0xFF ^ pad;
        this->cpu->execute(0x7FFFFFFF);
    }

    unsigned short* getDisplay() { return this->vdp->display; }
    size_t getDisplaySize() { return 240 * 192 * 2; }

    short* tickSound(size_t size)
    {
        static short buf[8192];
        if (sizeof(buf) < size) {
            return nullptr; // invalid size
        }
        if (this->ctx.bgm.playing) {
            this->vgsdec->execute(buf, size);
            this->ctx.bgm.playing = !this->vgsdec->isPlayEnd();
            if (!this->ctx.bgm.playing) {
                this->ctx.bgm.fadeout = false;
            } else {
                this->ctx.bgm.seekPosition = (unsigned int)this->vgsdec->getDurationTime();
            }
        } else {
            memset(buf, 0, size);
        }
        for (int i = 0; i < 256; i++) {
            if (this->ctx.se[i].playing) {
                for (int j = 0; j < (int)size / 2; j++) {
                    int wav = buf[j];
                    wav += this->se[i].data[this->ctx.se[i].playingIndex++];
                    if (32767 < wav) {
                        wav = 32767;
                    } else if (wav < -32768) {
                        wav = -32768;
                    }
                    buf[j] = (short)wav;
                    if ((int)this->se[i].count <= this->ctx.se[i].playingIndex) {
                        this->ctx.se[i].playingIndex = 0;
                        this->ctx.se[i].playing = false;
                        break;
                    }
                }
            }
        }
        return buf;
    }

    size_t getStateSize()
    {
        size_t result = sizeof(this->ctx);
        result += sizeof(this->cpu->reg);
        result += sizeof(this->vdp->ctx);
        return result;
    }

    void saveState(void* buffer)
    {
        unsigned char* bufferPtr = (unsigned char*)buffer;
        memcpy(bufferPtr, &this->ctx, sizeof(this->ctx));
        bufferPtr += sizeof(this->ctx);
        memcpy(bufferPtr, &this->cpu->reg, sizeof(this->cpu->reg));
        bufferPtr += sizeof(this->cpu->reg);
        memcpy(bufferPtr, &this->vdp->ctx, sizeof(this->vdp->ctx));
    }

    void loadState(const void* buffer)
    {
        const unsigned char* bufferPtr = (const unsigned char*)buffer;
        this->reset();
        memcpy(&this->ctx, bufferPtr, sizeof(this->ctx));
        bufferPtr += sizeof(this->ctx);
        memcpy(&this->cpu->reg, bufferPtr, sizeof(this->cpu->reg));
        bufferPtr += sizeof(this->cpu->reg);
        memcpy(&this->vdp->ctx, bufferPtr, sizeof(this->vdp->ctx));
        this->vdp->refreshDisplay();
        if (this->bgm[this->ctx.bgm.playingIndex].data) {
            this->vgsdec->load(this->bgm[this->ctx.bgm.playingIndex].data, this->bgm[this->ctx.bgm.playingIndex].size);
            this->vgsdec->seekTo(this->ctx.bgm.seekPosition);
        }
    }

  private:
    inline void consumeClock(int clocks)
    {
        this->vdp->ctx.bobo += clocks * VGS0_VDP_CLOCK_PER_SEC;
        while (0 < this->vdp->ctx.bobo) {
            this->vdp->tick();
            this->vdp->ctx.bobo -= VGS0_CPU_CLOCK_PER_SEC;
        }
    }

    inline unsigned char readMemory(unsigned short addr)
    {
        if (addr < 0x8000) {
            int ptr = this->ctx.romBank[addr / 0x2000] * 0x2000 + (addr & 0x1FFF);
            return ((int)this->rom.size <= ptr) ? 0xFF : this->rom.data[ptr];
        } else if (addr < 0xC000) {
            return this->vdp->read(addr);
        } else {
            return this->ctx.ram[addr & 0x3FFF];
        }
    }

    inline void writeMemory(unsigned short addr, unsigned char value)
    {
        if (addr < 0x8000) {
            this->ctx.romBank[addr / 0x2000] = value;
        } else if (addr < 0xC000) {
            this->vdp->write(addr, value);
        } else {
            this->ctx.ram[addr & 0x3FFF] = value;
        }
    }

    inline unsigned char inPort(unsigned char port)
    {
        switch (port) {
            case 0xA0: return this->ctx.pad;
            case 0xB0: return this->ctx.romBank[0];
            case 0xB1: return this->ctx.romBank[1];
            case 0xB2: return this->ctx.romBank[2];
            case 0xB3: return this->ctx.romBank[3];
            default: return 0xFF;
        }
    }

    inline void outPort(unsigned char port, unsigned char value)
    {
        switch (port) {
            case 0xB0: this->ctx.romBank[0] = value; break;
            case 0xB1: this->ctx.romBank[1] = value; break;
            case 0xB2: this->ctx.romBank[2] = value; break;
            case 0xB3: this->ctx.romBank[3] = value; break;
            case 0xC0: {
                int addr = this->cpu->reg.pair.A;
                addr *= 0x2000;
                if (addr + 0x2000 <= (int)this->rom.size) {
                    // printf("DMA: rom[%06X] bank(%d) -> cptn\n", addr, this->cpu->reg.pair.A);
                    memcpy(&this->vdp->ctx.ram[0x2000], &this->rom.data[addr], 0x2000);
                } else {
                    // printf("DMA-error: rom[%06X] bank(%d) -> cptn\n", addr, this->cpu->reg.pair.A);
                    memset(&this->vdp->ctx.ram[0x2000], 0xFF, 0x2000);
                }
                break;
            }
            case 0xC2: {
                unsigned short addrTo = this->cpu->reg.pair.B;
                addrTo <<= 8;
                addrTo |= this->cpu->reg.pair.C;
                unsigned short count = this->cpu->reg.pair.H;
                count <<= 8;
                count |= this->cpu->reg.pair.L;
                // printf("DMA: memset(%04X,%02X,%d)\n",addrTo,cpu->reg.pair.A,count);
                for (int i = 0; i < count; i++, addrTo++) {
                    this->writeMemory(addrTo, this->cpu->reg.pair.A);
                }
                break;
            }
            case 0xC3: {
                unsigned short addrTo = this->cpu->reg.pair.B;
                addrTo <<= 8;
                addrTo |= this->cpu->reg.pair.C;
                unsigned short addrFrom = this->cpu->reg.pair.D;
                addrFrom <<= 8;
                addrFrom |= this->cpu->reg.pair.E;
                unsigned short count = this->cpu->reg.pair.H;
                count <<= 8;
                count |= this->cpu->reg.pair.L;
                // printf("DMA: memcpy(%04X,%04X,%d)\n",addrTo,addrFrom,count);
                for (int i = 0; i < count; i++, addrTo++, addrFrom++) {
                    this->writeMemory(addrTo, this->readMemory(addrFrom));
                }
                break;
            }
            case 0xE0:
                if (this->bgm[value].data) {
                    this->ctx.bgm.playing = true;
                    this->ctx.bgm.fadeout = false;
                    this->ctx.bgm.seekPosition = 0;
                    this->ctx.bgm.playingIndex = value;
                    this->vgsdec->load(this->bgm[value].data, this->bgm[value].size);
                }
                break;
            case 0xE1:
                switch (value) {
                    case 0: // pause
                        this->ctx.bgm.playing = false;
                        break;
                    case 1: // resume
                        this->ctx.bgm.playing = true;
                        break;
                    case 2: // fadeout
                        this->ctx.bgm.fadeout = true;
                        this->vgsdec->fadeout();
                        break;
                }
                break;
            case 0xF0:
                if (this->se[value].data) {
                    this->ctx.se[value].playing = true;
                    this->ctx.se[value].playingIndex = 0;
                }
                break;
            case 0xF1:
                this->ctx.se[value].playing = false;
                this->ctx.se[value].playingIndex = 0;
                break;
            case 0xF2:
                this->cpu->reg.pair.A = this->ctx.se[value].playing ? 1 : 0;
                break;
        }
    }
};

#endif
