/**
 * @file vgs0lib.h
 * @brief VGS0 API Library for Z80 (SDCC version)
 * -----------------------------------------------------------------------------
 * The MIT License (MIT)
 *
 * Copyright (c) 2023 Yoji Suzuki.
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
#ifndef INCLUDE_VGS0LIB_H
#define INCLUDE_VGS0LIB_H

/** @def
 * Joypad bit mask (Cursor: Up)
 */
#define VGS0_JOYPAD_UP 0b10000000

/** @def
 * Joypad bit mask (Cursor: Down)
 */
#define VGS0_JOYPAD_DW 0b01000000

/** @def
 * Joypad bit mask (Cursor: Left)
 */
#define VGS0_JOYPAD_LE 0b00100000

/** @def
 * Joypad bit mask (Cursor: Right)
 */
#define VGS0_JOYPAD_RI 0b00010000

/** @def
 * Joypad bit mask (START button)
 */
#define VGS0_JOYPAD_ST 0b00001000

/** @def
 * Joypad bit mask (SELECT button)
 */
#define VGS0_JOYPAD_SE 0b00000100

/** @def
 * Joypad bit mask (A button: Jump/Bomber/Enter)
 */
#define VGS0_JOYPAD_T1 0b00000010

/** @def
 * Joypad bit mask (B button: Fire/Shot/Cancel)
 */
#define VGS0_JOYPAD_T2 0b00000001

/** @def
 * BG name table
 */
#define VGS0_ADDR_BG ((volatile NameTable*)0x8000)

/** @def
 * FG name table
 */
#define VGS0_ADDR_FG ((volatile NameTable*)0x8800)

/** @def
 * Object Attribute Memory
 */
#define VGS0_ADDR_OAM ((volatile OAM*)0x9000)

/**
 * Set an object attribute memory record
 */
#define vgs0_oam_set(NUM, X, Y, ATTR, PTN) VGS0_ADDR_OAM[NUM].x = X; \
                                           VGS0_ADDR_OAM[NUM].y = Y; \
                                           VGS0_ADDR_OAM[NUM].attr = ATTR; \
                                           VGS0_ADDR_OAM[NUM].ptn = PTN

/** @def
 * Palette table
 */
#define VGS0_ADDR_PALETTE ((volatile uint16_t*)0x9400)

/** @def
 * Vertical rendering counter
 */
#define VGS0_ADDR_COUNT_V ((volatile uint8_t*)0x9600)

/** @def
 * Horizontal rendering counter
 */
#define VGS0_ADDR_COUNT_H ((volatile uint8_t*)0x9601)

/** @def
 * BG scroll X
 */
#define VGS0_ADDR_BG_SCROLL_X ((volatile uint8_t*)0x9602)

/** @def
 * BG scroll Y
 */
#define VGS0_ADDR_BG_SCROLL_Y ((volatile uint8_t*)0x9603)

/** @def
 * FG scroll X
 */
#define VGS0_ADDR_FG_SCROLL_X ((volatile uint8_t*)0x9604)

/** @def
 * FG scroll Y
 */
#define VGS0_ADDR_FG_SCROLL_Y ((volatile uint8_t*)0x9605)

/** @def
 * IRQ scanline position
 */
#define VGS0_ADDR_IRQ_SCANLINE ((volatile uint8_t*)0x9606)

/** @def
 * VDP status
 */
#define VGS0_ADDR_STATUS ((volatile uint8_t*)0x9607)

/** @def
 * Character pattern table
 */
#define VGS0_ADDR_CHARACTER ((volatile uint8_t*)0xA000)

//! stdint compatible (8bit unsigned)
typedef unsigned char uint8_t;

//! stdint compatible (8bit signed)
typedef signed char int8_t;

//! stdint compatible (16bit unsigned)
typedef unsigned short uint16_t;

//! stdint compatible (16bit signed)
typedef signed short int16_t;

//! stdint compatible (32bit unsigned)
typedef unsigned long uint32_t;

//! stdint compatible (32bit signed)
typedef signed long int32_t;

//! stdint compatible (64bit unsigned)
typedef unsigned long long uint64_t;

//! stdint compatible (64bit signed)
typedef signed long long int64_t;

/**
 * Object Attribute Memory record
 */
typedef struct {
    //! Y-coordinate
    uint8_t y;
    //! X-coordinate
    uint8_t x;
    //! Character pattern number
    uint8_t ptn;
    //! Attribute value https://github.com/suzukiplan/vgs0/blob/master/README.md#attribute-bit-layout-bgfgsprite
    uint8_t attr;
} OAM;

/**
 * Name Table
 */
typedef struct {
    //! Character pattern number
    uint8_t ptn[32][32];
    //! Attribute value https://github.com/suzukiplan/vgs0/blob/master/README.md#attribute-bit-layout-bgfgsprite
    uint8_t attr[32][32];
} NameTable;

/**
 * @brief Wait until V-Blank is detected
 */
void vgs0_wait_vsync(void) __z88dk_fastcall;

/**
 * @brief Set palette
 * @param pn number of palette (0-15)
 * @param pi index of palette (0-15)
 * @param r color depth of red (0-31)
 * @param g color depth of green (0-31)
 * @param b color depth of blue (0-31)
 */
void vgs0_palette_set(uint8_t pn, uint8_t pi, uint8_t r, uint8_t g, uint8_t b) __smallc;

/**
 * @brief Set palette (RGB555)
 * @param pn number of palette (0-15)
 * @param pi index of palette (0-15)
 * @param rgb555 color depth of RGB555 (0b0rrrrrgggggbbbbb)
 */
void vgs0_palette_set_rgb555(uint8_t pn, uint8_t pi, uint16_t rgb555) __smallc;

/**
 * @brief High-speed DMA transfer of the contents of the specified ROM Bank to the Character Pattern Table in VRAM
 * @param prg ROM Bank number (0-255)
 */
void vgs0_dma(uint8_t prg) __z88dk_fastcall;

/**
 * @brief DMA equivalent to `memset` in C language
 * @param dst destination address
 * @param value set value
 * @param cnt transfer count in byte
 */
void vgs0_memset(uint16_t dst, uint8_t value, uint16_t cnt) __smallc;

/**
 * @brief DMA equivalent to `memcpy` in C language
 * @param dst destination address
 * @param src source address
 * @param cnt transfer count in byte
 */
void vgs0_memcpy(uint16_t dst, uint16_t src, uint16_t cnt) __smallc;

/**
 * @brief Continuously writes the specified ASCII code and attribute values to BG's NameTable
 * @param x X-coordinate (0-31)
 * @param y Y-coordinate (0-31)
 * @param attr attribute value https://github.com/suzukiplan/vgs0/blob/master/README.md#attribute-bit-layout-bgfgsprite
 * @param str '\0' terminated string
 */
void vgs0_bg_putstr(uint8_t x, uint8_t y, uint8_t attr, const char* str) __smallc;

/**
 * @brief Continuously writes the specified ASCII code and attribute values to FG's NameTable
 * @param x X-coordinate (0-31)
 * @param y Y-coordinate (0-31)
 * @param attr attribute value https://github.com/suzukiplan/vgs0/blob/master/README.md#attribute-bit-layout-bgfgsprite
 * @param str '\0' terminated string
 */
void vgs0_fg_putstr(uint8_t x, uint8_t y, uint8_t attr, const char* str) __smallc;

/**
 * @brief Acquire joypad input status
 * @return joypad input status
 * @note that the input bit is originally reset (0) and the uninput bit is set (1), but when this function is used, it is inverted.
 */
uint8_t vgs0_joypad_get(void) __z88dk_fastcall;

/**
 * @brief VGS: Play BGM
 * @param song number of the song to play
 */
void vgs0_bgm_play(uint8_t song) __z88dk_fastcall;

/**
 * @brief VGS: Pause BGM
 */
void vgs0_bgm_pause(void) __z88dk_fastcall;

/**
 * @brief VGS: Resume BGM
 */
void vgs0_bgm_resume(void) __z88dk_fastcall;

/**
 * @brief VGS: Fadeout BGM
 */
void vgs0_bgm_fadeout(void) __z88dk_fastcall;

#endif
