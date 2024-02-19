#include "../../lib/sdcc/vgs0lib.h"

void main(void)
{
    // Bank 2 を Character Pattern Table (0xA000) に転送 (DMA) してパレットに設定
    vgs0_dma(2);
    vgs0_memcpy((uint16_t)VGS0_ADDR_PALETTE, (uint16_t)VGS0_ADDR_CHARACTER, 512);

    // BG の DPM を Bank 3 に設定して BG を 1024 パターンモードに設定
    *VGS0_ADDR_BG_DPM = 3;
    *VGS0_ADDR_PTN1024 = 0x01;

    // ネームテーブルを設定
    uint8_t n = 0;
    for (uint8_t y = 0; y < 32; y++) {
        for (uint8_t x = 0; x < 32; x++, n++) {
            VGS0_ADDR_BG->ptn[y][x] = n;
        }
    }

    // ループ
    while (1) {
        // 垂直動機待ち
        vgs0_wait_vsync();

        // カーソル入力でスクロール
        uint8_t pad = vgs0_joypad_get();
        if (pad & VGS0_JOYPAD_LE) {
            (*VGS0_ADDR_BG_SCROLL_X) -= 1;
        } else if (pad & VGS0_JOYPAD_RI) {
            (*VGS0_ADDR_BG_SCROLL_X) += 1;
        }
        if (pad & VGS0_JOYPAD_UP) {
            (*VGS0_ADDR_BG_SCROLL_Y) -= 1;
        } else if (pad & VGS0_JOYPAD_DW) {
            (*VGS0_ADDR_BG_SCROLL_Y) += 1;
        }
    }
}
