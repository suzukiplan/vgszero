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

    // メニューを表示
    vgs0_bg_putstr(8, 7, 0x80, "PLAY: #1 PRELUDE");
    vgs0_bg_putstr(8, 9, 0x80, "PLAY: #2 GOLDBERG");
    vgs0_bg_putstr(8, 11, 0x80, "PLAY: #3 WTC1");
    vgs0_bg_putstr(8, 13, 0x80, "PAUSE");
    vgs0_bg_putstr(8, 15, 0x80, "RESUME");
    vgs0_bg_putstr(8, 17, 0x80, "FADEOUT");
    VGS0_ADDR_BG->ptn[7][6] = '>';

    // ボタン入力制御変数を準備
    uint8_t pushing = 0;
    uint8_t push = 0;

    // カーソル移動制御変数を準備
    uint8_t moving = 0;
    int8_t move = 0;
    uint8_t cursor = 7;

    while (1) {
        vgs0_wait_vsync();
        uint8_t pad = vgs0_joypad_get();

        // カーソル入力チェック
        if (pad & VGS0_JOYPAD_UP) {
            if (!moving) {
                move = -2;
                moving = 1;
            }
        } else if (pad & VGS0_JOYPAD_DW) {
            if (!moving) {
                move = 2;
                moving = 1;
            }
        } else {
            moving = 0;
        }

        // カーソル移動
        if (move) {
            VGS0_ADDR_BG->ptn[cursor][6] = ' ';
            cursor += move;
            if (cursor < 7) {
                cursor = 17;
            } else if (17 < cursor) {
                cursor = 7;
            }
            VGS0_ADDR_BG->ptn[cursor][6] = '>';
            move = 0;
        }

        // ボタン入力チェック
        if ((pad & VGS0_JOYPAD_T1) || (pad & VGS0_JOYPAD_T2)) {
            if (!pushing) {
                push = 1;
                pushing = 1;
            }
        } else {
            if (pushing) {
                pushing = 0;
            } else {
                push = 0;
            }
        }

        // Aボタンを押して離した瞬間にカーソル位置のコマンドを実行
        if (!pushing && push) {
            switch (cursor) {
                case 7: vgs0_bgm_play(0); break;
                case 9: vgs0_bgm_play(1); break;
                case 11: vgs0_bgm_play(2); break;
                case 13: vgs0_bgm_pause(); break;
                case 15: vgs0_bgm_resume(); break;
                case 17: vgs0_bgm_fadeout(); break;
            }
        }
    }
}
