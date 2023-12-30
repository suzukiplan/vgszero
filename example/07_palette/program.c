#include "../../lib/sdcc/vgs0lib.h"
#include "palette.h"

void main(void)
{
    // パレットを初期化
    init_palette();

    // Bank 2 を Character Pattern Table (0xA000) に転送 (DMA)
    vgs0_dma(2);

    // BG Name Table を設定
    uint8_t attr = 0;
    for (int y = 0; y < 32; y++) {
        for (int x = 0; x < 32; x++) {
            VGS0_ADDR_BG->ptn[y][x] = 0x00;
            VGS0_ADDR_BG->attr[y][x] = 0x80 | ((attr + y) & 0x0F);
            attr++;
        }
    }

    while (1) {
        vgs0_wait_vsync();
        *VGS0_ADDR_BG_SCROLL_Y += 1;
    }
}
