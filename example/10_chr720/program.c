#include "../../lib/sdcc/vgs0lib.h"
#include "palette.h"

void main(void)
{
    // パレットを初期化
    init_palette();

    // DPM で BG, FG, Sprite それぞれに異なるキャラクタパターンを設定
    *VGS0_ADDR_BG_DPM = 2;
    *VGS0_ADDR_FG_DPM = 3;
    *VGS0_ADDR_SPRITE_DPM = 4;

    // 上半分にBG/FGを並べる
    uint8_t x, y;
    uint8_t n;
    for (y = 0; y < 16; y++) {
        for (x = 0; x < 16; x++) {
            VGS0_ADDR_BG->ptn[y][x] = n;
            VGS0_ADDR_FG->ptn[y][x + 16] = n;
            VGS0_ADDR_FG->attr[y][x + 16] = 0x80;
            n += 1;
        }
    }
    *VGS0_ADDR_BG_SCROLL_Y -= 8;
    *VGS0_ADDR_FG_SCROLL_Y -= 8;

    // 下半分にスプライトの前半と後半を並べる
    uint8_t n1 = 0;
    uint8_t n2 = 0x80;
    for (y = 128 + 8; y < 200; y += 8) {
        uint8_t x2 = 0x80;
        for (x = 0; x < 128; x += 8, x2 += 8) {
            vgs0_oam_set(n1, x, y, 0x80, n1, 0, 0);
            vgs0_oam_set(n2, x2, y, 0x80, n2, 0, 0);
            n1 += 1;
            n2 += 1;
        }
    }
}
