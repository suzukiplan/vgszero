/**
 * VGS-Zero - Video Display Processor
 * License under GPLv3: https://github.com/suzukiplan/vgszero/blob/master/LICENSE-VGS0.txt
 * (C)2023, SUZUKI PLAN
 */

#ifndef INCLUDE_VDP_HPP
#define INCLUDE_VDP_HPP
#include <stdint.h>
#include <stdio.h>
#include <string.h>

class VDP
{
  public:
    enum class ColorMode {
        RGB555,
        RGB565
    };
    void (*externalRedneringCallback)(void* arg);

  private:
    const unsigned char* rom;
    int romSize;
    void (*detectEndOfFrame)(void* arg);
    void (*detectIRQ)(void* arg);
    void* arg;
    ColorMode colorMode;
    inline unsigned char* getBgNameTableAddr() { return &this->ctx.ram0[0x0000]; }
    inline unsigned char* getBgAttrTableAddr() { return &this->ctx.ram0[0x0400]; }
    inline unsigned char* getFgNameTableAddr() { return &this->ctx.ram0[0x0800]; }
    inline unsigned char* getFgAttrTableAddr() { return &this->ctx.ram0[0x0C00]; }
    inline unsigned short* getColorTableAddr() { return (unsigned short*)&this->ctx.ram0[0x1800]; }
    inline unsigned char* getPatternTableAddr() { return &this->ctx.ram1[this->ctx.bank][0]; }
    inline unsigned char getRegisterBgScrollX() { return this->ctx.ram0[0x1F02]; }
    inline unsigned char getRegisterBgScrollY() { return this->ctx.ram0[0x1F03]; }
    inline unsigned char getRegisterFgScrollX() { return this->ctx.ram0[0x1F04]; }
    inline unsigned char getRegisterFgScrollY() { return this->ctx.ram0[0x1F05]; }
    inline int getBgDPM() { return ((int)this->ctx.ram0[0x1F08]) * 0x2000 % this->romSize; }
    inline int getFgDPM() { return ((int)this->ctx.ram0[0x1F09]) * 0x2000 % this->romSize; }
    inline int getSpriteDPM() { return ((int)this->ctx.ram0[0x1F0A]) * 0x2000 % this->romSize; }
    inline bool isBG1024() { return this->ctx.ram0[0x1F0B] & 0x01; }
    inline bool isFG1024() { return this->ctx.ram0[0x1F0B] & 0x02; }
    inline unsigned char getRegisterIRQ() { return this->ctx.ram0[0x1F06]; }
    inline unsigned char* getOamAddr() { return &this->ctx.ram0[0x1000]; }
    inline bool isAttrVisible(unsigned char attr) { return attr & 0x80; }
    inline bool isAttrFlipH(unsigned char attr) { return attr & 0x40; }
    inline bool isAttrFlipV(unsigned char attr) { return attr & 0x20; }
    inline int paletteFromAttr(unsigned char attr) { return (attr & 0x0F) << 4; }
    unsigned short paletteCache[256];

    inline void updatePaletteCache(unsigned char idx)
    {
        unsigned short* ptr = this->getColorTableAddr();
        ptr += idx;
        if (ColorMode::RGB555 == this->colorMode) {
            this->paletteCache[idx] = *ptr;
        } else {
            // RGB555 -> RGB565 (Green bit 0 = 0)
            this->paletteCache[idx] = ((*ptr) & 0x7C00) << 1;
            this->paletteCache[idx] |= ((*ptr) & 0x03E0) << 1;
            this->paletteCache[idx] |= (*ptr) & 0x001F;
        }
    }

    void resetPaletteCache()
    {
        for (int idx = 0; idx < 256; idx++) {
            this->updatePaletteCache((unsigned char)idx);
        }
    }

  public:
    unsigned short display[240 * 192];
    struct Context {
        int64_t bobo;
        int countV;
        int countH;
        unsigned char status;
        unsigned char bank;
        unsigned char cReserved[2];
        unsigned char ram0[0x2000];
        unsigned char ram1[256][0x2000];
    } ctx;

    VDP(ColorMode colorMode_, void* arg, void (*detectEndOfFrame)(void* arg), void (*detectIRQ)(void* arg))
    {
        this->colorMode = colorMode_;
        this->detectEndOfFrame = detectEndOfFrame;
        this->detectIRQ = detectIRQ;
        this->arg = arg;
        this->rom = nullptr;
        this->romSize = 0;
        this->externalRedneringCallback = nullptr;
    }

    void setROM(const void* rom_, size_t romSize_)
    {
        this->rom = (const unsigned char*)rom_;
        this->romSize = (int)romSize_;
    }

    void reset()
    {
        memset(&this->ctx, 0, sizeof(this->ctx));
        this->resetPaletteCache();
    }

    inline unsigned char read(unsigned short addr)
    {
        addr &= 0x3FFF;
        switch (addr) {
            case 0x1F00: return this->ctx.countV < 200 ? this->ctx.countV : 0xFF;
            case 0x1F01: return this->ctx.countH < 256 ? this->ctx.countH : 0xFF;
            case 0x1F07: {
                unsigned char result = this->ctx.status;
                this->ctx.status = 0;
                return result;
            }
            default:
                if (addr < 0x2000) {
                    return this->ctx.ram0[addr];
                } else {
                    return this->ctx.ram1[this->ctx.bank][addr & 0x1FFF];
                }
        }
    }

    inline void write(unsigned short addr, unsigned char value)
    {
        addr &= 0x3FFF;
        if (addr < 0x2000) {
            this->ctx.ram0[addr] = value;
            if (0x1800 <= addr && addr < 0x1A00) {
                this->updatePaletteCache((addr - 0x1800) / 2);
            }
        } else {
            this->ctx.ram1[this->ctx.bank][addr & 0x1FFF] = value;
        }
    }

    inline void tick()
    {
        this->ctx.countH++;
        this->ctx.countH %= 342;
        if (0 == this->ctx.countH) {
            this->renderScanline(this->ctx.countV);
            this->ctx.countV++;
            this->ctx.countV %= 262;
            this->ctx.status |= (200 == this->ctx.countV ? 0x80 : 0x00);
            if (0 == this->ctx.countV) {
                this->detectEndOfFrame(this->arg);
            } else if (this->ctx.countV == this->getRegisterIRQ()) {
                this->detectIRQ(this->arg);
            }
        }
    }

    void refreshDisplay()
    {
        this->resetPaletteCache();
        for (int i = 8; i < 200; i++) this->renderScanline(i);
    }

    bool externalRendering()
    {
        static int scanline = 8;
        this->renderBG(scanline - 8);
        this->renderSprites(scanline);
        this->renderFG(scanline - 8);
        scanline++;
        if (200 <= scanline) {
            scanline = 8;
            return true; // EOL
        } else {
            return false;
        }
    }

  private:
    inline void renderScanline(int scanline)
    {
        if (scanline < 8 || 200 <= scanline) return; // blanking
        if (this->externalRedneringCallback) {
            this->externalRedneringCallback(this->arg);
        } else {
            this->renderBG(scanline - 8);
            this->renderSprites(scanline);
            this->renderFG(scanline - 8);
        }
    }

    inline void renderBG(int scanline)
    {
        int y = scanline + this->getRegisterBgScrollY();
        unsigned short* display = &this->display[scanline * 240];
        int offset = (((y + 8) / 8) & 0x1F) * 32;
        unsigned char* nametbl = this->getBgNameTableAddr() + offset;
        unsigned char* attrtbl = this->getBgAttrTableAddr() + offset;
        const unsigned char* ptntbl0;
        const unsigned char* ptntbl1;
        int dpm = this->getBgDPM();
        if (dpm) {
            if (this->isBG1024()) {
                dpm += offset / 256 * 0x2000;
                dpm %= this->romSize;
            }
            ptntbl0 = &this->rom[dpm];
            ptntbl1 = &this->rom[(dpm + 0x2000) % this->romSize];
        } else {
            ptntbl0 = this->getPatternTableAddr();
            ptntbl1 = ptntbl0;
        }
        const char* ptntbl[2] = {ptntbl0, ptntbl1};
        for (int x = this->getRegisterBgScrollX() + 8, xx = 0; xx < 240; x++, xx++, display++) {
            offset = (x >> 3) & 0x1F;
            unsigned char ptn = nametbl[offset];
            unsigned char attr = attrtbl[offset];
            const unsigned char* chrtbl = ptntbl[(attr & 0b00010000) >> 4];
            chrtbl += ptn << 5;
            chrtbl += (this->isAttrFlipV(attr) ? 7 - (y & 7) : y & 7) << 2;
            int pal;
            if (this->isAttrFlipH(attr)) {
                chrtbl += (7 - (x & 7)) >> 1;
                pal = x & 1 ? ((*chrtbl) & 0xF0) >> 4 : (*chrtbl) & 0x0F;
            } else {
                chrtbl += (x & 7) >> 1;
                pal = x & 1 ? (*chrtbl) & 0x0F : ((*chrtbl) & 0xF0) >> 4;
            }
            *display = this->paletteCache[pal + this->paletteFromAttr(attr)];
        }
    }

    inline void renderFG(int scanline)
    {
        int y = scanline + this->getRegisterFgScrollY();
        unsigned short* display = &this->display[scanline * 240];
        int offset = (((y + 8) / 8) & 0x1F) * 32;
        unsigned char* nametbl = this->getFgNameTableAddr() + offset;
        unsigned char* attrtbl = this->getFgAttrTableAddr() + offset;
        const unsigned char* ptntbl0;
        const unsigned char* ptntbl1;
        int dpm = this->getFgDPM();
        if (dpm) {
            if (this->isFG1024()) {
                dpm += offset / 256 * 0x2000;
                dpm %= this->romSize;
            }
            ptntbl0 = &this->rom[dpm];
            ptntbl1 = &this->rom[(dpm + 0x2000) % this->romSize];
        } else {
            ptntbl0 = this->getPatternTableAddr();
            ptntbl1 = ptntbl0;
        }
        const char* ptntbl[2] = {ptntbl0, ptntbl1};
        for (int x = this->getRegisterFgScrollX() + 8, xx = 0; xx < 240; x++, xx++, display++) {
            offset = (x >> 3) & 0x1F;
            unsigned char ptn = nametbl[offset];
            unsigned char attr = attrtbl[offset];
            if (!this->isAttrVisible(attr)) continue;
            const unsigned char* chrtbl = ptntbl[(attr & 0b00010000) >> 4];
            chrtbl += ptn << 5;
            chrtbl += (this->isAttrFlipV(attr) ? 7 - (y & 7) : y & 7) << 2;
            int pal;
            if (this->isAttrFlipH(attr)) {
                chrtbl += (7 - (x & 7)) >> 1;
                pal = x & 1 ? ((*chrtbl) & 0xF0) >> 4 : (*chrtbl) & 0x0F;
            } else {
                chrtbl += (x & 7) >> 1;
                pal = x & 1 ? (*chrtbl) & 0x0F : ((*chrtbl) & 0xF0) >> 4;
            }
            if (pal) {
                *display = this->paletteCache[pal + this->paletteFromAttr(attr)];
            }
        }
    }

    inline void renderSprites(int scanline)
    {
        unsigned char* oam = this->getOamAddr();
        unsigned short* display = &this->display[(scanline - 8) * 240];
        oam += 255 * 8;
        const unsigned char* ptntbl;
        int dpm = this->getSpriteDPM();
        for (int i = 0; i < 256; i++, oam -= 8) {
            if (!this->isAttrVisible(oam[3])) continue;
            int height = (oam[4] & 0x0F) + 1;
            int width = (oam[5] & 0x0F) + 1;
            bool flipH = this->isAttrFlipH(oam[3]);
            bool flipV = this->isAttrFlipV(oam[3]);
            if (oam[6]) {
                ptntbl = &this->rom[oam[6] * 0x2000 % this->romSize];
            } else if (dpm) {
                if (oam[3] & 0b00010000) {
                    ptntbl = &this->rom[(dpm + 0x2000) % this->romSize];
                } else {
                    ptntbl = &this->rom[dpm];
                }
            } else {
                ptntbl = this->getPatternTableAddr();
            }
            for (int cy = 0; cy < height; cy++) {
                for (int cx = 0; cx < width; cx++) {
                    unsigned char y = (unsigned char)(oam[0] + cy * 8);
                    unsigned char x = (unsigned char)(oam[1] + cx * 8);
                    if (x == 0 || 248 < x || scanline < y || y + 8 <= scanline) {
                        continue;
                    }
                    const unsigned char* chrtbl = ptntbl;
                    chrtbl += (oam[2] + (flipV ? height - cy - 1 : cy) * 0x10 + (flipH ? width - cx - 1 : cx)) << 5;
                    int dy = scanline - y;
                    chrtbl += (flipV ? 7 - (dy & 7) : dy & 7) << 2;
                    for (int j = 0; j < 8; j++, x++) {
                        if (x < 8 || 248 <= x) continue;
                        int pal;
                        if (flipH) {
                            int jj = 7 - j;
                            pal = jj & 1 ? chrtbl[jj >> 1] & 0x0F : (chrtbl[jj >> 1] & 0xF0) >> 4;
                        } else {
                            pal = j & 1 ? chrtbl[j >> 1] & 0x0F : (chrtbl[j >> 1] & 0xF0) >> 4;
                        }
                        if (pal) {
                            display[x - 8] = this->paletteCache[pal + this->paletteFromAttr(oam[3])];
                        }
                    }
                }
            }
        }
    }
};

#endif
