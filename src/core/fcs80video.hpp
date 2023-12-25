/**
 * FAIRY COMPUTER SYSTEM 80 - Video Display Processor Emulator
 * -----------------------------------------------------------------------------
 * The MIT License (MIT)
 *
 * Copyright (c) 2021 Yoji Suzuki.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 * -----------------------------------------------------------------------------
 */

#ifndef INCLUDE_FCS80VIDEO_HPP
#define INCLUDE_FCS80VIDEO_HPP
#include <stdio.h>
#include <string.h>

class FCS80Video
{
  public:
    enum class ColorMode {
        RGB555,
        RGB565
    };

  private:
    void (*detectEndOfFrame)(void* arg);
    void (*detectIRQ)(void* arg);
    void* arg;
    ColorMode colorMode;
    inline unsigned char* getBgNameTableAddr() { return &this->ctx.ram[0x0000]; }
    inline unsigned char* getBgAttrTableAddr() { return &this->ctx.ram[0x0400]; }
    inline unsigned char* getFgNameTableAddr() { return &this->ctx.ram[0x0800]; }
    inline unsigned char* getFgAttrTableAddr() { return &this->ctx.ram[0x0C00]; }
    inline unsigned short* getColorTableAddr() { return (unsigned short*)&this->ctx.ram[0x1400]; }
    inline unsigned char* getPatternTableAddr() { return &this->ctx.ram[0x2000]; }
    inline unsigned char getRegisterBgScrollX() { return this->ctx.ram[0x1602]; }
    inline unsigned char getRegisterBgScrollY() { return this->ctx.ram[0x1603]; }
    inline unsigned char getRegisterFgScrollX() { return this->ctx.ram[0x1604]; }
    inline unsigned char getRegisterFgScrollY() { return this->ctx.ram[0x1605]; }
    inline unsigned char getRegisterIRQ() { return this->ctx.ram[0x1606]; }
    inline unsigned char* getOamAddr() { return &this->ctx.ram[0x1000]; }
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
        int bobo;
        int countV;
        int countH;
        unsigned char status;
        unsigned char cReserved[3];
        unsigned char ram[0x4000];
    } ctx;

    FCS80Video(ColorMode colorMode_, void* arg, void (*detectEndOfFrame)(void* arg), void (*detectIRQ)(void* arg))
    {
        this->colorMode = colorMode_;
        this->detectEndOfFrame = detectEndOfFrame;
        this->detectIRQ = detectIRQ;
        this->arg = arg;
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
            case 0x1600: return this->ctx.countV < 200 ? this->ctx.countV : 0xFF;
            case 0x1601: return this->ctx.countH < 256 ? this->ctx.countH : 0xFF;
            case 0x1607: {
                unsigned char result = this->ctx.status;
                this->ctx.status = 0;
                return result;
            }
            default: return this->ctx.ram[addr];
        }
    }

    inline void write(unsigned short addr, unsigned char value)
    {
        addr &= 0x3FFF;
        this->ctx.ram[addr] = value;
        if (0x1400 <= addr && addr < 0x1600) {
            this->updatePaletteCache((addr - 0x1400) / 2);
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

  private:
    inline void renderScanline(int scanline)
    {
        if (scanline < 8 || 200 <= scanline) return; // blanking
        this->renderBG(scanline - 8);
        this->renderSprites(scanline);
        this->renderFG(scanline - 8);
    }

    inline void renderBG(int scanline)
    {
        int y = scanline + this->getRegisterBgScrollY();
        unsigned short* display = &this->display[scanline * 240];
        int offset = (((y + 8) / 8) & 0x1F) * 32;
        unsigned char* nametbl = this->getBgNameTableAddr() + offset;
        unsigned char* attrtbl = this->getBgAttrTableAddr() + offset;
        for (int x = this->getRegisterBgScrollX() + 8, xx = 0; xx < 240; x++, xx++, display++) {
            offset = (x >> 3) & 0x1F;
            unsigned char ptn = nametbl[offset];
            unsigned char attr = attrtbl[offset];
            unsigned char* chrtbl = this->getPatternTableAddr();
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
        for (int x = this->getRegisterFgScrollX() + 8, xx = 0; xx < 240; x++, xx++, display++) {
            offset = (x >> 3) & 0x1F;
            unsigned char ptn = nametbl[offset];
            unsigned char attr = attrtbl[offset];
            if (!this->isAttrVisible(attr)) continue;
            unsigned char* chrtbl = this->getPatternTableAddr();
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
        oam += 255 * 4;
        for (int i = 0; i < 256; i++, oam -= 4) {
            if (!this->isAttrVisible(oam[3])) continue;
            if (0 == oam[1] || 248 <= oam[1] || scanline < oam[0] || oam[0] + 8 <= scanline) continue;
            unsigned char* chrtbl = this->getPatternTableAddr();
            chrtbl += oam[2] << 5;
            int dy = scanline - oam[0];
            chrtbl += (this->isAttrFlipV(oam[3]) ? 7 - (dy & 7) : dy & 7) << 2;
            bool flipH = this->isAttrFlipH(oam[3]);
            for (int j = 0, x = oam[1]; j < 8; j++, x++) {
                if (x < 8 || 248 <= x) continue;
                int pal;
                if (flipH) {
                    int jj = 7 - j;
                    pal = jj & 1 ? (chrtbl[jj >> 1] & 0xF0) >> 4 : chrtbl[jj >> 1] & 0x0F;
                } else {
                    pal = j & 1 ? chrtbl[j >> 1] & 0x0F : (chrtbl[j >> 1] & 0xF0) >> 4;
                }
                if (pal) {
                    display[x - 8] = this->paletteCache[pal + this->paletteFromAttr(oam[3])];
                }
            }
        }
    }
};

#endif
