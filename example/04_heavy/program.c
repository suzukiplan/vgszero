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

    // NameTableを初期化
    for (int y = 0; y < 32; y++) {
        for (int x = 0; x < 32; x++) {
            VGS0_ADDR_BG->ptn[y][x] = '#';
            VGS0_ADDR_BG->attr[y][x] = 0b11100000;
            VGS0_ADDR_FG->ptn[y][x] = '.';
            VGS0_ADDR_FG->attr[y][x] = 0x80;
        }
    }

    // OAM を初期化
    uint8_t n = 0;
    for (int y = 0; y < 16; y++) {
        for (int x = 0; x < 16; x++) {
            vgs0_oam_set(n, 8 + x * 4, 8 + y * 4, 0x80, 'X');
            n++;
        }
    }

    // BGM を再生
    vgs0_bgm_play(0);

    uint8_t cnt = 0;
    while (1) {
        vgs0_wait_vsync();
        *(VGS0_ADDR_BG_SCROLL_X) += 8;
        *(VGS0_ADDR_BG_SCROLL_Y) += 8;
        *(VGS0_ADDR_FG_SCROLL_X) -= 1;
        *(VGS0_ADDR_FG_SCROLL_Y) -= 1;
        VGS0_ADDR_BG->attr[cnt & 0x1F][cnt & 0x1F] ^= 0b01100000;
        vgs0_se_play(cnt);
        VGS0_ADDR_OAM[cnt].x += 4;
        VGS0_ADDR_OAM[cnt].y++;
        cnt++;
    }
}
