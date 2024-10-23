#include "../../lib/sdcc/vgs0lib.h"

#define SPRITE_NUM 256

// グローバル変数
typedef struct {
    // スプライトの表示情報
    struct Obj {
        var16_t x;
        var16_t y;
        var16_t vx;
        var16_t vy;
        uint8_t an;
    } obj[SPRITE_NUM];
} GlobalVariables;
#define GV ((GlobalVariables*)0xC000)

void main(void)
{
    // パレットを初期化
    vgs0_palette_set_rgb555(0, 0, 0b0000000000000000);
    vgs0_palette_set_rgb555(0, 1, 0b0001110011100111);
    vgs0_palette_set_rgb555(0, 2, 0b0110001100011000);
    vgs0_palette_set_rgb555(0, 3, 0b0111111111111111);
    vgs0_palette_set_rgb555(0, 4, 0b0000001110000000);
    vgs0_palette_set_rgb555(0, 5, 0b0000000000011100);

    // Bank 2 を Character Pattern Table (0xA000) に転送 (DMA)
    vgs0_dma(2);

    // FGの左上に "SPRITE TEST" を描画
    vgs0_fg_putstr(2, 2, 0x80, "SPRITE TEST");

    // BG全画面にひし形を描画
    for (int y = 0; y < 32; y++) {
        for (int x = 0; x < 32; x++) {
            VGS0_ADDR_BG->ptn[y][x] = 0x10;
            VGS0_ADDR_BG->attr[y][x] = 0x80 | ((x & 1) << 5) | ((y & 1) << 6);
        }
    }

    // スプライト & グローバル変数を初期化
    for (int i = 0; i < SPRITE_NUM; i++) {
        VGS0_ADDR_OAM[i].x = vgs0_rand8();
        VGS0_ADDR_OAM[i].y = vgs0_rand8() % 192;
        VGS0_ADDR_OAM[i].ptn = 1 + (i & 0x07);
        VGS0_ADDR_OAM[i].attr = 0x80;
        GV->obj[i].x.value = VGS0_ADDR_OAM[i].x << 8;
        GV->obj[i].y.value = VGS0_ADDR_OAM[i].y << 8;
        GV->obj[i].vx.value = vgs0_rand16() % 512 - 256;
        GV->obj[i].vy.value = vgs0_rand16() % 512 - 256;
        GV->obj[i].an = vgs0_rand8();
    }

    // メインループ
    while (1) {
        // V-BLANK を待機
        vgs0_wait_vsync();

        // スプライトを動かす
        for (int i = 0; i < SPRITE_NUM; i++) {
            GV->obj[i].x.value += (int16_t)GV->obj[i].vx.value;
            GV->obj[i].y.value += (int16_t)GV->obj[i].vy.value;
            VGS0_ADDR_OAM[i].x = GV->obj[i].x.raw[1];
            VGS0_ADDR_OAM[i].y = GV->obj[i].y.raw[1];
            GV->obj[i].an++;
            GV->obj[i].an &= 0x03;
            if (0 == GV->obj[i].an) {
                VGS0_ADDR_OAM[i].ptn++;
                if (9 == VGS0_ADDR_OAM[i].ptn) {
                    VGS0_ADDR_OAM[i].ptn = 1;
                }
            }
        }

        // BGをスクロール
        (*VGS0_ADDR_BG_SCROLL_Y)--;
    }
}