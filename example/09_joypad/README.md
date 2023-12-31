# Joypad

ジョイパッドの入力状態をチェックします

## How to build

### Pre-request

- GNU make and GNU Compiler Collection
  - macOS: install XCODE
  - Linux: `sudo apt install build-essential`
- SDCC version 4.1.0
  - macOS(x64): [https://sourceforge.net/projects/sdcc/files/sdcc-macos-amd64/4.1.0/](https://sourceforge.net/projects/sdcc/files/sdcc-macos-amd64/4.1.0/)
  - Linux(x64): [https://sourceforge.net/projects/sdcc/files/sdcc-linux-amd64/4.1.0/](https://sourceforge.net/projects/sdcc/files/sdcc-linux-amd64/4.1.0/)

### Build

```zsh
git clone https://github.com/suzukiplan/vgszero
cd vgszero/example/09_joypad
make
```

## ROM structure

```
8KB x 4 banks = 24KB ROM
```

- Bank 0: program
- Bank 1: program
- Bank 2: image.chr

## Program Code

```c
#include "../../lib/sdcc/vgs0lib.h"
#include "palette.h"

// グローバル変数
typedef struct {
    uint8_t n; // 使用スプライト数
} GlobalVariables;
#define GV ((GlobalVariables*)0xC000)

// ジョイパッドのタイルパターン
static const uint8_t joypad[10 * 23] = {
    0x60, 0x60, 0x60, 0x60, 0x60, 0x60, 0x60, 0x60, 0x60, 0x60, 0x60, 0x60, 0x61, 0x62, 0x63, 0x64, 0x65, 0x66, 0x67, 0x68, 0x69, 0x6A, 0x6B,
    0x6C, 0x6D, 0x6D, 0x6D, 0x6D, 0x6D, 0x6D, 0x6E, 0x60, 0x60, 0x60, 0x60, 0x60, 0x60, 0x60, 0x60, 0x60, 0x60, 0x60, 0x60, 0x60, 0x60, 0x6F,
    0x70, 0x71, 0x71, 0x71, 0x71, 0x71, 0x71, 0x72, 0x60, 0x60, 0x60, 0x60, 0x60, 0x60, 0x60, 0x60, 0x60, 0x60, 0x60, 0x60, 0x60, 0x60, 0x6F,
    0x70, 0x71, 0x73, 0x74, 0x75, 0x71, 0x71, 0x72, 0x60, 0x60, 0x60, 0x60, 0x60, 0x60, 0x60, 0x60, 0x60, 0x60, 0x60, 0x60, 0x60, 0x60, 0x6F,
    0x70, 0x71, 0x76, 0x77, 0x78, 0x71, 0x71, 0x79, 0x7A, 0x7A, 0x7A, 0x7A, 0x7A, 0x7A, 0x7A, 0x7A, 0x7A, 0x7A, 0x7A, 0x7A, 0x7A, 0x7A, 0x7B,
    0x70, 0x7C, 0x7D, 0x77, 0x7E, 0x7F, 0x80, 0x71, 0x71, 0x71, 0x71, 0x71, 0x71, 0x71, 0x71, 0x71, 0x81, 0x71, 0x71, 0x82, 0x83, 0x71, 0x84,
    0x70, 0x84, 0x77, 0x77, 0x77, 0x77, 0x85, 0x86, 0x87, 0x88, 0x89, 0x8A, 0x8B, 0x8C, 0x71, 0x8D, 0x8E, 0x8F, 0x90, 0x91, 0x92, 0x93, 0x84,
    0x94, 0x95, 0x96, 0x77, 0x97, 0x98, 0x99, 0x9A, 0x9B, 0x9B, 0x9C, 0x9D, 0x9B, 0x9E, 0x9F, 0xA0, 0xA1, 0xA2, 0xA3, 0xA4, 0xA5, 0xA6, 0xA7,
    0xA8, 0xA9, 0xAA, 0xAB, 0xAC, 0xAD, 0xAD, 0xAE, 0xAF, 0xAF, 0xB0, 0xB1, 0xAF, 0xB2, 0xB3, 0xB4, 0xB5, 0xB6, 0xB7, 0xB8, 0xB9, 0xBA, 0xBB,
    0xBC, 0xBD, 0xBD, 0xBD, 0xBD, 0xBD, 0xBD, 0xBD, 0xBD, 0xBD, 0xBD, 0xBD, 0xBD, 0xBD, 0xBD, 0xBD, 0xBD, 0xBD, 0xBD, 0xBD, 0xBD, 0xBD, 0xBE
};

// カーソルのスプライト初期化
inline void set_sprite_cursor(int x, int y)
{
    vgs0_oam_set(GV->n, x + 36, y + 36, 0x00, 0x01);
    GV->n += 1;
    vgs0_oam_set(GV->n, x + 36 + 8, y + 36, 0x00, 0x02);
    GV->n += 1;
    vgs0_oam_set(GV->n, x + 36, y + 36 + 8, 0x00, 0x03);
    GV->n += 1;
    vgs0_oam_set(GV->n, x + 36 + 8, y + 36 + 8, 0x00, 0x04);
    GV->n += 1;
}

// SELECT/STARTのスプライト初期化
inline void set_sprite_ctrl(int x, int y)
{
    vgs0_oam_set(GV->n, x + 36, y + 36, 0x00, 0x05);
    GV->n += 1;
    vgs0_oam_set(GV->n, x + 36 + 8, y + 36, 0x00, 0x06);
    GV->n += 1;
    vgs0_oam_set(GV->n, x + 36 + 16, y + 36, 0x00, 0x07);
    GV->n += 1;
}

// A/Bのスプライト初期化
inline void set_sprite_button(int x, int y)
{
    vgs0_oam_set(GV->n, x + 36, y + 36, 0x00, 0xC0);
    GV->n += 1;
    vgs0_oam_set(GV->n, x + 36 + 8, y + 36, 0x00, 0xC1);
    GV->n += 1;
    vgs0_oam_set(GV->n, x + 36 + 16, y + 36, 0x00, 0xC2);
    GV->n += 1;
    vgs0_oam_set(GV->n, x + 36, y + 36 + 8, 0x00, 0xD0);
    GV->n += 1;
    vgs0_oam_set(GV->n, x + 36 + 8, y + 36 + 8, 0x00, 0xD1);
    GV->n += 1;
    vgs0_oam_set(GV->n, x + 36 + 16, y + 36 + 8, 0x00, 0xD2);
    GV->n += 1;
    vgs0_oam_set(GV->n, x + 36, y + 36 + 16, 0x00, 0xE0);
    GV->n += 1;
    vgs0_oam_set(GV->n, x + 36 + 8, y + 36 + 16, 0x00, 0xE1);
    GV->n += 1;
}

// スプライト番号 s から n 個の表示属性を 表示 (on != 0) または 非表示 (on == 0) に設定
void update_sprite_attr(uint8_t s, uint8_t n, uint8_t on)
{
    if (0 != on) {
        on = 0x80;
    } else {
        on = 0x00;
    }
    for (uint8_t i = 0; i < n; i++) {
        VGS0_ADDR_OAM[s + i].attr = on;
    }
}

void main(void)
{
    // パレットを初期化
    init_palette();

    // Bank 2 を Character Pattern Table ($A000) に転送 (DMA)
    vgs0_dma(2);

    // BGにジョイパッドのタイルパターンを描画
    int x, y;
    int n = 0;
    for (y = 0; y < 10; y++) {
        for (x = 0; x < 23; x++) {
            VGS0_ADDR_BG->ptn[4 + y][4 + x] = joypad[n++];
        }
    }

    *VGS0_ADDR_BG_SCROLL_X -= 4;
    *VGS0_ADDR_BG_SCROLL_Y -= 4;

    // スプライト準備
    GV->n = 0;
    set_sprite_cursor(24, 32); // 上カーソル (0~3)
    set_sprite_cursor(24, 56); // 下カーソル (4~7)
    set_sprite_cursor(12, 44); // 左カーソル (8~11)
    set_sprite_cursor(36, 44); // 右カーソル (12~15)
    set_sprite_ctrl(60, 61); // SELECT (16~18)
    set_sprite_ctrl(88, 61); // START (19~21)
    set_sprite_button(124, 50); // B (22~29)
    set_sprite_button(150, 50); // B (30~37)

    while (1) {
        // 垂直同期待ち
        vgs0_wait_vsync();

        // ボタンの押下状態によって表示を更新
        uint8_t pad = vgs0_joypad_get();
        update_sprite_attr(0, 4, pad & VGS0_JOYPAD_UP);
        update_sprite_attr(4, 4, pad & VGS0_JOYPAD_DW);
        update_sprite_attr(8, 4, pad & VGS0_JOYPAD_LE);
        update_sprite_attr(12, 4, pad & VGS0_JOYPAD_RI);
        update_sprite_attr(16, 3, pad & VGS0_JOYPAD_SE);
        update_sprite_attr(19, 3, pad & VGS0_JOYPAD_ST);
        update_sprite_attr(22, 8, pad & VGS0_JOYPAD_T2);
        update_sprite_attr(30, 8, pad & VGS0_JOYPAD_T1);
    }
}
```