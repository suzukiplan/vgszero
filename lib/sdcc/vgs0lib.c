/**
 * @file vgs0lib.c
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
#include "vgs0lib.h"
#pragma disable_warning 59 // no check none return (return at inline-asm)
#pragma disable_warning 85 // no check unused args (check at inline-asm)
#define STACK_ARG_HEAD 4

void vgs0_bank0_switch(uint8_t n) __z88dk_fastcall
{
__asm
    ld a, l
    out (#0xB0), a
__endasm;
}

void vgs0_bank1_switch(uint8_t n) __z88dk_fastcall
{
__asm
    ld a, l
    out (#0xB1), a
__endasm;
}

void vgs0_bank2_switch(uint8_t n) __z88dk_fastcall
{
__asm
    ld a, l
    out (#0xB2), a
__endasm;
}

void vgs0_bank3_switch(uint8_t n) __z88dk_fastcall
{
__asm
    ld a, l
    out (#0xB3), a
__endasm;
}

uint8_t vgs0_bank0_get(void) __z88dk_fastcall
{
__asm
    in a, (#0xB0)
    ld l, a
    ret
__endasm;
}

uint8_t vgs0_bank1_get(void) __z88dk_fastcall
{
__asm
    in a, (#0xB1)
    ld l, a
    ret
__endasm;
}

uint8_t vgs0_bank2_get(void) __z88dk_fastcall
{
__asm
    in a, (#0xB2)
    ld l, a
    ret
__endasm;
}

uint8_t vgs0_bank3_get(void) __z88dk_fastcall
{
__asm
    in a, (#0xB3)
    ld l, a
    ret
__endasm;
}

void vgs0_wait_vsync(void) __z88dk_fastcall
{
__asm
    ld hl, #0x9F07
wait_vblank_loop:
    ld a, (hl)
    and #0x80
    jp z, wait_vblank_loop
__endasm;
}

void vgs0_palette_set(uint8_t pn, uint8_t pi, uint8_t r, uint8_t g, uint8_t b) __smallc
{
    uint16_t col;
    uint16_t addr;
    col = r & 0x1F;
    col <<= 5;
    col |= g & 0x1F;
    col <<= 5;
    col |= b & 0x1F;
    addr = 0x9800;
    addr += pn << 5;
    addr += pi << 1;
    *((uint16_t*)addr) = col;
}

void vgs0_palette_set_rgb555(uint8_t pn, uint8_t pi, uint16_t rgb555) __smallc
{
    uint16_t addr;
    addr = 0x9800;
    addr += pn << 5;
    addr += pi << 1;
    *((uint16_t*)addr) = rgb555;
}

void vgs0_dma(uint8_t prg) __z88dk_fastcall
{
__asm
    ld a, l
    out (#0xC0), a
__endasm;
}

void vgs0_memset(uint16_t dst, uint8_t value, uint16_t cnt) __smallc
{
__asm
    push ix
    ld ix, #STACK_ARG_HEAD
    add ix, sp
    // cnt -> hl
    ld l, (ix)
    inc ix
    ld h, (ix)
    inc ix
    // value-> a
    ld a, (ix)
    inc ix
    inc ix
    // dst -> bc
    ld c, (ix)
    inc ix
    ld b, (ix)
    pop ix
    // execute DMA
    out (#0xC2), a
__endasm;
}

void vgs0_memcpy(uint16_t dst, uint16_t src, uint16_t cnt) __smallc
{
__asm
    push ix
    ld ix, #STACK_ARG_HEAD
    add ix, sp
    // cnt -> hl
    ld l, (ix)
    inc ix
    ld h, (ix)
    inc ix
    // src -> de
    ld e, (ix)
    inc ix
    ld d, (ix)
    inc ix
    // dst -> bc
    ld c, (ix)
    inc ix
    ld b, (ix)
    pop ix
    // execute DMA
    out (#0xC3), a
__endasm;
}

uint8_t vgs0_collision_check(uint16_t addr) __z88dk_fastcall
{
__asm
    in a, (#0xC4)
    ld l, a
    ret
__endasm;
}

uint16_t vgs0_mul(uint8_t h, uint8_t l) __smallc
{
__asm
    push ix
    ld ix, #STACK_ARG_HEAD
    add ix, sp
    // l -> l
    ld l, (ix)
    inc ix
    inc ix
    // h -> h
    ld h, (ix)
    pop ix
    // execute HAG
    ld a, #0x00
    out (#0xC5), a
    ret
__endasm;
}

uint16_t vgs0_div(uint8_t h, uint8_t l) __smallc
{
__asm
    push ix
    ld ix, #STACK_ARG_HEAD
    add ix, sp
    // l -> l
    ld l, (ix)
    inc ix
    inc ix
    // h -> h
    ld h, (ix)
    pop ix
    // execute HAG
    ld a, #0x01
    out (#0xC5), a
    ret
__endasm;
}

uint16_t vgs0_mod(uint8_t h, uint8_t l) __smallc
{
__asm
    push ix
    ld ix, #STACK_ARG_HEAD
    add ix, sp
    // l -> l
    ld l, (ix)
    inc ix
    inc ix
    // h -> h
    ld h, (ix)
    pop ix
    // execute HAG
    ld a, #0x02
    out (#0xC5), a
    ret
__endasm;
}

uint16_t vgs0_mul16(uint16_t hl, uint8_t c) __smallc
{
__asm
    push ix
    ld ix, #STACK_ARG_HEAD
    add ix, sp
    // c -> c
    ld c, (ix)
    inc ix
    inc ix
    // l -> l
    ld l, (ix)
    inc ix
    // h -> h
    ld h, (ix)
    pop ix
    // execute HAG
    ld a, #0x80
    out (#0xC5), a
    ret
__endasm;
}

uint16_t vgs0_div16(uint16_t hl, uint8_t c) __smallc
{
__asm
    push ix
    ld ix, #STACK_ARG_HEAD
    add ix, sp
    // c -> c
    ld c, (ix)
    inc ix
    inc ix
    // l -> l
    ld l, (ix)
    inc ix
    // h -> h
    ld h, (ix)
    pop ix
    // execute HAG
    ld a, #0x81
    out (#0xC5), a
    ret
__endasm;
}

uint16_t vgs0_mod16(uint16_t hl, uint8_t c) __smallc
{
__asm
    push ix
    ld ix, #STACK_ARG_HEAD
    add ix, sp
    // c -> c
    ld c, (ix)
    inc ix
    inc ix
    // l -> l
    ld l, (ix)
    inc ix
    // h -> h
    ld h, (ix)
    pop ix
    // execute HAG
    ld a, #0x82
    out (#0xC5), a
    ret
__endasm;
}

void vgs0_bg_putstr(uint8_t x, uint8_t y, uint8_t attr, const char* str) __smallc
{
    x &= 0x1F;
    y &= 0x1F;
    uint16_t addrC = 0x8000 + (y << 5) + x;
    uint16_t addrA = addrC + 0x400;
    while (*str) {
        *((uint8_t*)addrC) = *str;
        *((uint8_t*)addrA) = attr;
        addrC++;
        addrA++;
        str++;
    }
}

void vgs0_fg_putstr(uint8_t x, uint8_t y, uint8_t attr, const char* str) __smallc
{
    x &= 0x1F;
    y &= 0x1F;
    uint16_t addrC = 0x8800 + (y << 5) + x;
    uint16_t addrA = addrC + 0x400;
    while (*str) {
        *((uint8_t*)addrC) = *str;
        *((uint8_t*)addrA) = attr;
        addrC++;
        addrA++;
        str++;
    }
}

uint8_t vgs0_joypad_get(void) __z88dk_fastcall
{
__asm
    in a, (#0xA0)
    xor #0xFF
    ld l, a
    ret
__endasm;
}

void vgs0_bgm_play(uint8_t song) __z88dk_fastcall
{
__asm
    ld a, l
    out (#0xE0), a
__endasm;
}

void vgs0_bgm_pause(void) __z88dk_fastcall
{
__asm
    ld a, #0
    out (#0xE1), a
__endasm;
}

void vgs0_bgm_resume(void) __z88dk_fastcall
{
__asm
    ld a, #1
    out (#0xE1), a
__endasm;
}

void vgs0_bgm_fadeout(void) __z88dk_fastcall
{
__asm
    ld a, #2
    out (#0xE1), a
__endasm;
}

void vgs0_se_play(uint8_t se) __z88dk_fastcall
{
__asm
    ld a, l
    out (#0xF0), a
__endasm;
}

void vgs0_se_stop(uint8_t se) __z88dk_fastcall
{
__asm
    ld a, l
    out (#0xF1), a
__endasm;
}

uint8_t vgs0_se_playing(uint8_t se) __z88dk_fastcall
{
__asm
    ld a, l
    out (#0xF2), a
    ld l, a
    ret
__endasm;
}

uint8_t vgs0_save(uint16_t addr, uint16_t size) __smallc
{    
__asm
    push ix
    ld ix, #STACK_ARG_HEAD
    add ix, sp
    // size -> HL
    ld l, (ix)
    inc ix
    ld h, (ix)
    inc ix
    // addr -> BC
    ld c, (ix)
    inc ix
    ld b, (ix)
    pop ix
    // execute SAVE
    out (#0xDA), a
    ld l, a
    ret
__endasm;
}

uint8_t vgs0_load(uint16_t addr, uint16_t size) __smallc
{
__asm
    push ix
    ld ix, #STACK_ARG_HEAD
    add ix, sp
    // size -> HL
    ld l, (ix)
    inc ix
    ld h, (ix)
    inc ix
    // addr -> BC
    ld c, (ix)
    inc ix
    ld b, (ix)
    pop ix
    // execute LOAD
    in  a, (#0xDA)
    ld l, a
    ret
__endasm;
}
