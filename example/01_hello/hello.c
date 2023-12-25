#include "../../lib/sdcc/vgs0lib.h"

void main(void)
{
    // パレットを初期化
    vgs0_palette_set(0, 0, 0, 0, 0);    // black
    vgs0_palette_set(0, 1, 7, 7, 7);    // dark gray
    vgs0_palette_set(0, 2, 24, 24, 24); // light gray
    vgs0_palette_set(0, 3, 31, 31, 31); // white

    // Bank 2 を Character Pattern Table ($A000) に転送 (DMA)
    vgs0_dma(2);

    // 画面中央付近 (10,12) に "HELLO,WORLD!" を描画
    vgs0_bg_putstr(10, 12, 0x80, "HELLO,WORLD!");

    while (1) {
        vgs0_wait_vsync();
        uint8_t pad = vgs0_joypad_get();
        if (pad & VGS0_JOYPAD_LE) {
            *VGS0_ADDR_BG_SCROLL_X += 1;
        } else if (pad & VGS0_JOYPAD_RI) {
            *VGS0_ADDR_BG_SCROLL_X -= 1;
        }
        if (pad & VGS0_JOYPAD_UP) {
            *VGS0_ADDR_BG_SCROLL_Y += 1;
        } else if (pad & VGS0_JOYPAD_DW) {
            *VGS0_ADDR_BG_SCROLL_Y -= 1;
        }
    }
}
