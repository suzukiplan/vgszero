#include "../../lib/sdcc/vgs0lib.h"

#define SPRITE_NUM 256

// グローバル変数
typedef struct {
    uint8_t x;
    uint8_t y;

    struct Bullet {
        uint8_t flag;
        var16_t x;
        var16_t y;
        var16_t vx;
        var16_t vy;
    } bullets[128];
    uint8_t bulletIndex;
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
    vgs0_palette_set(0, 13, 0xD0 >> 3, 0xD0 >> 3, 0x68 >> 3);

    // Bank 2 を Character Pattern Table (0xA000) に転送 (DMA)
    vgs0_dma(2);

    // 座標初期化
    vgs0_memset(0xC000, 0x00, sizeof(GlobalVariables));
    GV->x = 120;
    GV->y = 156;

    // スプライト表示
    vgs0_oam_set(0, GV->x, GV->y, 0x80, 9, 0, 0);
    vgs0_oam_set(1, GV->x + 8, GV->y, 0x80 | 0x40, 9, 0, 0);
    vgs0_oam_set(2, GV->x, GV->y + 8, 0x80, 10, 0, 0);
    vgs0_oam_set(3, GV->x + 8, GV->y + 8, 0x80 | 0x40, 10, 0, 0);

    vgs0_fg_putstr(2, 2, 0x80, "ANGLE:000");

    // メインループ
    uint8_t a = 0;
    while (1) {
        // V-BLANK を待機
        vgs0_wait_vsync();
        uint8_t pad = vgs0_joypad_get();

        // スプライトの移動
        if (pad & VGS0_JOYPAD_LE) {
            GV->x -= 2;
        } else if (pad & VGS0_JOYPAD_RI) {
            GV->x += 2;
        }
        if (pad & VGS0_JOYPAD_UP) {
            GV->y -= 2;
        } else if (pad & VGS0_JOYPAD_DW) {
            GV->y += 2;
        }

        // スプライトの座標更新
        VGS0_ADDR_OAM[0].x = GV->x;
        VGS0_ADDR_OAM[0].y = GV->y;
        VGS0_ADDR_OAM[1].x = GV->x + 8;
        VGS0_ADDR_OAM[1].y = GV->y;
        VGS0_ADDR_OAM[2].x = GV->x;
        VGS0_ADDR_OAM[2].y = GV->y + 8;
        VGS0_ADDR_OAM[3].x = GV->x + 8;
        VGS0_ADDR_OAM[3].y = GV->y + 8;

        // 弾の移動
        for (uint8_t i = 0; i < 128; i++) {
            if (GV->bullets[i].flag) {
                GV->bullets[i].x.value += GV->bullets[i].vx.value;
                GV->bullets[i].y.value += GV->bullets[i].vy.value;
                if (200 < GV->bullets[i].y.raw[1] || 248 < GV->bullets[i].x.raw[1]) {
                    GV->bullets[i].flag = 0;
                    VGS0_ADDR_OAM[128 + i].attr = 0x00;
                } else {
                    vgs0_oam_set(128 + i, GV->bullets[i].x.raw[1], GV->bullets[i].y.raw[1], 0x80, 0x08, 0, 0);
                }
            }
        }

        // 4フレームに1回、画面中央から自機狙いを発射
        a++;
        a &= 0x03;
        if (0 == a && 0 == GV->bullets[GV->bulletIndex].flag) {
            GV->bullets[GV->bulletIndex].flag = 1;
            GV->bullets[GV->bulletIndex].x.raw[1] = 124;
            GV->bullets[GV->bulletIndex].y.raw[1] = 100;
            uint8_t r = vgs0_angle(127, 104, GV->x + 8, GV->y + 8);
            GV->bullets[GV->bulletIndex].vx.value = (uint16_t)(vgs0_sin(r) * 3);
            GV->bullets[GV->bulletIndex].vy.value = (uint16_t)(vgs0_cos(r) * 3);
            GV->bulletIndex++;
            GV->bulletIndex &= 0x7F;
            VGS0_ADDR_FG->ptn[2][8] = '0' + vgs0_div(r, 100);
            VGS0_ADDR_FG->ptn[2][9] = '0' + vgs0_div(vgs0_mod(r, 100), 10);
            VGS0_ADDR_FG->ptn[2][10] = '0' + vgs0_mod(r, 10);
        }
    }
}
