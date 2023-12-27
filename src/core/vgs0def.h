/**
 * VGS0 - Literal value definition
 * License under GPLv3: https://github.com/suzukiplan/vgszero/blob/master/LICENSE-VGS0.txt
 * (C)2023, SUZUKI PLAN
 */
#ifndef INCLUDE_VGS0DEF_H
#define INCLUDE_VGS0DEF_H

#define VGS0_CPU_CLOCK_PER_SEC 16777216 /* 16MHz */
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
