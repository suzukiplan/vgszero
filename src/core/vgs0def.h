/**
 * VGS0 - Literal value definition
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
#ifndef INCLUDE_VGS0DEF_H
#define INCLUDE_VGS0DEF_H

#define VGS0_CPU_CLOCK_PER_SEC 3579545
#define VGS0_VDP_CLOCK_PER_SEC 5376240 /* 342 * 262 * 60 Hz */
#define VGS0_PSG_CLOCK_PER_SEC 44100

#define VGS0_JOYPAD_UP 0b10000000
#define VGS0_JOYPAD_DW 0b01000000
#define VGS0_JOYPAD_LE 0b00100000
#define VGS0_JOYPAD_RI 0b00010000
#define VGS0_JOYPAD_ST 0b00001000
#define VGS0_JOYPAD_SE 0b00000100
#define VGS0_JOYPAD_T1 0b00000010
#define VGS0_JOYPAD_T2 0b00000001

#endif
