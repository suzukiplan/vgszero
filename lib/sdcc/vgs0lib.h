/**
 * @file vgs0lib.h
 * @brief VGS-Zero API Library for Z80 (SDCC version)
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
 * https://github.com/suzukiplan/vgszero/blob/master/README.md#joypad
 */
#define VGS0_JOYPAD_UP 0b10000000

/** @def
 * Joypad bit mask (Cursor: Down)
 * https://github.com/suzukiplan/vgszero/blob/master/README.md#joypad
 */
#define VGS0_JOYPAD_DW 0b01000000

/** @def
 * Joypad bit mask (Cursor: Left)
 * https://github.com/suzukiplan/vgszero/blob/master/README.md#joypad
 */
#define VGS0_JOYPAD_LE 0b00100000

/** @def
 * Joypad bit mask (Cursor: Right)
 * https://github.com/suzukiplan/vgszero/blob/master/README.md#joypad
 */
#define VGS0_JOYPAD_RI 0b00010000

/** @def
 * Joypad bit mask (START button)
 * https://github.com/suzukiplan/vgszero/blob/master/README.md#joypad
 */
#define VGS0_JOYPAD_ST 0b00001000

/** @def
 * Joypad bit mask (SELECT button)
 * https://github.com/suzukiplan/vgszero/blob/master/README.md#joypad
 */
#define VGS0_JOYPAD_SE 0b00000100

/** @def
 * Joypad bit mask (A button: Jump/Bomber/Enter)
 * https://github.com/suzukiplan/vgszero/blob/master/README.md#joypad
 */
#define VGS0_JOYPAD_T1 0b00000010

/** @def
 * Joypad bit mask (B button: Fire/Shot/Cancel)
 * https://github.com/suzukiplan/vgszero/blob/master/README.md#joypad
 */
#define VGS0_JOYPAD_T2 0b00000001

/** @def
 * BG name table
 * - BG: https://github.com/suzukiplan/vgszero/blob/master/README.md#bg
 * - NameTable: https://github.com/suzukiplan/vgszero/blob/master/README.md#name-table
 */
#define VGS0_ADDR_BG ((NameTable*)0x8000)

/** @def
 * FG name table
 * - FG: https://github.com/suzukiplan/vgszero/blob/master/README.md#fg
 * - NameTable: https://github.com/suzukiplan/vgszero/blob/master/README.md#name-table
 */
#define VGS0_ADDR_FG ((NameTable*)0x8800)

/** @def
 * Object Attribute Memory of the Sprites
 * - Sprite: https://github.com/suzukiplan/vgszero/blob/master/README.md#sprite
 * - OAM: https://github.com/suzukiplan/vgszero/blob/master/README.md#oam
 */
#define VGS0_ADDR_OAM ((OAM*)0x9000)

/**
 * Set an object attribute memory record macro
 * - Sprite: https://github.com/suzukiplan/vgszero/blob/master/README.md#sprite
 * - OAM: https://github.com/suzukiplan/vgszero/blob/master/README.md#oam
 */
#define vgs0_oam_set(NUM, X, Y, ATTR, PTN) \
    VGS0_ADDR_OAM[NUM].x = X;              \
    VGS0_ADDR_OAM[NUM].y = Y;              \
    VGS0_ADDR_OAM[NUM].attr = ATTR;        \
    VGS0_ADDR_OAM[NUM].ptn = PTN

/** @def
 * Palette table
 * https://github.com/suzukiplan/vgszero/blob/master/README.md#palette
 */
#define VGS0_ADDR_PALETTE ((uint16_t*)0x9400)

/** @def
 * Vertical rendering counter
 * https://github.com/suzukiplan/vgszero/blob/master/README.md#scanline-counter
 */
#define VGS0_ADDR_COUNT_V ((uint8_t*)0x9600)

/** @def
 * Horizontal rendering counter
 * https://github.com/suzukiplan/vgszero/blob/master/README.md#scanline-counter
 */
#define VGS0_ADDR_COUNT_H ((uint8_t*)0x9601)

/** @def
 * BG scroll X
 * https://github.com/suzukiplan/vgszero/blob/master/README.md#hardware-scroll
 */
#define VGS0_ADDR_BG_SCROLL_X ((uint8_t*)0x9602)

/** @def
 * BG scroll Y
 * https://github.com/suzukiplan/vgszero/blob/master/README.md#hardware-scroll
 */
#define VGS0_ADDR_BG_SCROLL_Y ((uint8_t*)0x9603)

/** @def
 * FG scroll X
 * https://github.com/suzukiplan/vgszero/blob/master/README.md#hardware-scroll
 */
#define VGS0_ADDR_FG_SCROLL_X ((uint8_t*)0x9604)

/** @def
 * FG scroll Y
 * https://github.com/suzukiplan/vgszero/blob/master/README.md#hardware-scroll
 */
#define VGS0_ADDR_FG_SCROLL_Y ((uint8_t*)0x9605)

/** @def
 * IRQ scanline position
 */
#define VGS0_ADDR_IRQ_SCANLINE ((uint8_t*)0x9606)

/** @def
 * VDP status
 * https://github.com/suzukiplan/vgszero/blob/master/README.md#vdp-status
 */
#define VGS0_ADDR_STATUS ((uint8_t*)0x9607)

/** @def
 * Character pattern table
 * https://github.com/suzukiplan/vgszero/blob/master/README.md#character-pattern-table
 */
#define VGS0_ADDR_CHARACTER ((uint8_t*)0xA000)

/** @def
 * BG Direct Pattern Mapping
 * https://github.com/suzukiplan/vgszero/blob/master/README.md#direct-pattern-mapping
 */
#define VGS0_ADDR_BG_DPM ((uint8_t*)0x9608)

/** @def
 * FG Direct Pattern Mapping
 * https://github.com/suzukiplan/vgszero/blob/master/README.md#direct-pattern-mapping
 */
#define VGS0_ADDR_FG_DPM ((uint8_t*)0x9609)

/** @def
 * Sprite Direct Pattern Mapping
 * https://github.com/suzukiplan/vgszero/blob/master/README.md#direct-pattern-mapping
 */
#define VGS0_ADDR_SPRITE_DPM ((uint8_t*)0x960A)

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
 * Object Attribute Memory
 * https://github.com/suzukiplan/vgszero/blob/master/README.md#oam
 */
typedef struct {
    //! Y-coordinate
    uint8_t y;
    //! X-coordinate
    uint8_t x;
    //! Character pattern number: https://github.com/suzukiplan/vgszero/blob/master/README.md#character-pattern-table
    uint8_t ptn;
    //! Attribute value: https://github.com/suzukiplan/vgszero/blob/master/README.md#attribute
    uint8_t attr;
} OAM;

/**
 * Name Table
 */
typedef struct {
    //! Character pattern number: https://github.com/suzukiplan/vgszero/blob/master/README.md#character-pattern-table
    uint8_t ptn[32][32];
    //! Attribute value: https://github.com/suzukiplan/vgszero/blob/master/README.md#attribute
    uint8_t attr[32][32];
} NameTable;

/**
 * @brief Switch ROM bank 0
 * @param n number of the bank
 */
void vgs0_bank0_switch(uint8_t n) __z88dk_fastcall;

/**
 * @brief Switch ROM bank 1
 * @param n number of the bank
 */
void vgs0_bank1_switch(uint8_t n) __z88dk_fastcall;

/**
 * @brief Switch ROM bank 2
 * @param n number of the bank
 */
void vgs0_bank2_switch(uint8_t n) __z88dk_fastcall;

/**
 * @brief Switch ROM bank 3
 * @param n number of the bank
 */
void vgs0_bank3_switch(uint8_t n) __z88dk_fastcall;

/**
 * @brief Get current bank number of ROM bank 0
 * @return number of the bank
 */
uint8_t vgs0_bank0_get(void) __z88dk_fastcall;

/**
 * @brief Get current bank number of ROM bank 1
 * @return number of the bank
 */
uint8_t vgs0_bank1_get(void) __z88dk_fastcall;

/**
 * @brief Get current bank number of ROM bank 2
 * @return number of the bank
 */
uint8_t vgs0_bank2_get(void) __z88dk_fastcall;

/**
 * @brief Get current bank number of ROM bank 3
 * @return number of the bank
 */
uint8_t vgs0_bank3_get(void) __z88dk_fastcall;

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
 * @note https://github.com/suzukiplan/vgszero/blob/master/README.md#palette
 */
void vgs0_palette_set(uint8_t pn, uint8_t pi, uint8_t r, uint8_t g, uint8_t b) __smallc;

/**
 * @brief Set palette (RGB555)
 * @param pn number of palette (0-15)
 * @param pi index of palette (0-15)
 * @param rgb555 color depth of RGB555 (0b0rrrrrgggggbbbbb)
 * @note https://github.com/suzukiplan/vgszero/blob/master/README.md#palette
 */
void vgs0_palette_set_rgb555(uint8_t pn, uint8_t pi, uint16_t rgb555) __smallc;

/**
 * @brief High-speed DMA transfer of the contents of the specified ROM Bank to the Character Pattern Table in VRAM
 * @param prg ROM Bank number (0-255)
 * @note https://github.com/suzukiplan/vgszero/blob/master/README.md#rom-to-character-dma
 */
void vgs0_dma(uint8_t prg) __z88dk_fastcall;

/**
 * @brief DMA equivalent to `memset` in C language
 * @param dst destination address
 * @param value set value
 * @param cnt transfer count in byte
 * @note https://github.com/suzukiplan/vgszero/blob/master/README.md#memset-dma
 */
void vgs0_memset(uint16_t dst, uint8_t value, uint16_t cnt) __smallc;

/**
 * @brief DMA equivalent to `memcpy` in C language
 * @param dst destination address
 * @param src source address
 * @param cnt transfer count in byte
 * @note https://github.com/suzukiplan/vgszero/blob/master/README.md#memcpy-dma
 */
void vgs0_memcpy(uint16_t dst, uint16_t src, uint16_t cnt) __smallc;

/**
 * @brief Continuously writes the specified ASCII code and attribute values to BG's NameTable
 * @param x X-coordinate (0-31)
 * @param y Y-coordinate (0-31)
 * @param attr attribute value: https://github.com/suzukiplan/vgszero/blob/master/README.md#attribute
 * @param str '\0' terminated string
 */
void vgs0_bg_putstr(uint8_t x, uint8_t y, uint8_t attr, const char* str) __smallc;

/**
 * @brief Continuously writes the specified ASCII code and attribute values to FG's NameTable
 * @param x X-coordinate (0-31)
 * @param y Y-coordinate (0-31)
 * @param attr attribute value https://github.com/suzukiplan/vgszero/blob/master/README.md#attribute
 * @param str '\0' terminated string
 */
void vgs0_fg_putstr(uint8_t x, uint8_t y, uint8_t attr, const char* str) __smallc;

/**
 * @brief Acquire joypad input status
 * @return joypad input status
 * @note https://github.com/suzukiplan/vgszero/blob/master/README.md#joypad
 * @note that the input bit is originally reset (0) and the uninput bit is set (1), but when this function is used, it is inverted.
 */
uint8_t vgs0_joypad_get(void) __z88dk_fastcall;

/**
 * @brief VGS: Play BGM
 * @param song number of the song to play
 * @note https://github.com/suzukiplan/vgszero/blob/master/README.md#play-bgm
 */
void vgs0_bgm_play(uint8_t song) __z88dk_fastcall;

/**
 * @brief VGS: Pause BGM
 * @note https://github.com/suzukiplan/vgszero/blob/master/README.md#pause-bgm
 */
void vgs0_bgm_pause(void) __z88dk_fastcall;

/**
 * @brief VGS: Resume BGM
 * @note https://github.com/suzukiplan/vgszero/blob/master/README.md#resume-bgm
 */
void vgs0_bgm_resume(void) __z88dk_fastcall;

/**
 * @brief VGS: Fadeout BGM
 * @note https://github.com/suzukiplan/vgszero/blob/master/README.md#fadeout-bgm
 */
void vgs0_bgm_fadeout(void) __z88dk_fastcall;

/**
 * @brief SE: Play Sound Effect
 * @note https://github.com/suzukiplan/vgszero/blob/master/README.md#play-sound-effect
 */
void vgs0_se_play(uint8_t se) __z88dk_fastcall;

/**
 * @brief SE: Stop Sound Effect
 * @note https://github.com/suzukiplan/vgszero/blob/master/README.md#stop-sound-effect
 */
void vgs0_se_stop(uint8_t se) __z88dk_fastcall;

/**
 * @brief SE: Check Sound Effect
 * @note https://github.com/suzukiplan/vgszero/blob/master/README.md#check-sound-effect
 */
uint8_t vgs0_se_playing(uint8_t se) __z88dk_fastcall;

/**
 * @brief Save data to save.dat
 * @param addr Save start RAM address
 * @param size Save size
 * @return 0: Sucess, !0: Failed
 */
uint8_t vgs0_save(uint16_t addr, uint16_t size) __smallc;

/**
 * @brief Load data from save.dat
 * @param addr Load start RAM address
 * @param size Load size
 * @return 0: Sucess, !0: Failed
 */
uint8_t vgs0_load(uint16_t addr, uint16_t size) __smallc;

#endif
