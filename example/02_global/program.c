#include "../../lib/sdcc/vgs0lib.h"
#include "global.h"

void countUp(void)
{
    if (!GV->stop) {
        GV->c1++;
        if (9 < GV->c1) {
            GV->c1 = 0;
            GV->c10++;
            if (9 < GV->c10) {
                GV->c10 = 0;
                GV->c100++;
                if (9 < GV->c100) {
                    GV->c100 = 0;
                    GV->c1000++;
                    if (9 < GV->c1000) {
                        GV->stop = 1;
                        GV->c1 = 9;
                        GV->c10 = 9;
                        GV->c100 = 9;
                        GV->c1000 = 9;
                    }
                }
            }
        }
    }
}

void main(void)
{
    // パレットを初期化
    vgs0_palette_set(0, 0, 0, 0, 0);    // black
    vgs0_palette_set(0, 1, 7, 7, 7);    // dark gray
    vgs0_palette_set(0, 2, 24, 24, 24); // light gray
    vgs0_palette_set(0, 3, 31, 31, 31); // white

    // Bank 2 を Character Pattern Table ($A000) に転送 (DMA)
    // vgs0_dma(2);
    vgs0_dma_ram(2, 0x0000, 8192, 0xA000);

    // グローバル変数を初期化
    GV->stop = 0;
    GV->c1 = 4;
    GV->c10 = 3;
    GV->c100 = 2;
    GV->c1000 = 1;
    vgs0_bg_putstr(4, 4, 0x80, "COUNT:");

    // メインループ
    while (1) {
        vgs0_wait_vsync();
        countUp();
        VGS0_ADDR_BG->ptn[4][10] = '0' + GV->c1000;
        VGS0_ADDR_BG->ptn[4][11] = '0' + GV->c100;
        VGS0_ADDR_BG->ptn[4][12] = '0' + GV->c10;
        VGS0_ADDR_BG->ptn[4][13] = '0' + GV->c1;
    }
}
