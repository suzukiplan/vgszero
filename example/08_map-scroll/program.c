#include "../../lib/sdcc/vgs0lib.h"
#include "palette.h"

void main(void)
{
    // パレットを初期化
    init_palette();

    // Bank 2 を Character Pattern Table ($A000) に転送 (DMA)
    vgs0_dma(2);

    // マップの初期状態を描画
    uint8_t scrollCounter = 0;
    uint16_t nextRead = 0x8000 - 32 * 25;
    int8_t nextWrite = 0;
    vgs0_memcpy((uint16_t)VGS0_ADDR_BG, nextRead, 32 * 25);

    // メインループ
    while (1) {
        // VBLANKを待機
        vgs0_wait_vsync();

        // BGを下スクロール
        *VGS0_ADDR_BG_SCROLL_Y -= 1;

        // スクロールカウンタをインクリメント
        scrollCounter++;
        scrollCounter &= 0x07;
        if (0 != scrollCounter) {
            continue;
        }

        // 次のマップを描画
        nextRead -= 32;
        nextRead &= 0x1FFF;
        nextRead |= 0x6000;
        nextWrite--;
        nextWrite &= 0x1F;
        vgs0_memcpy((uint16_t)&VGS0_ADDR_BG->ptn[nextWrite][0], nextRead, 32);
    }
}