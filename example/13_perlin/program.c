#include "../../lib/sdcc/vgs0lib.h"

#define SPRITE_NUM 256

// グローバル変数
typedef struct {
    uint16_t seed;
    uint16_t x;
    uint16_t y;
    int8_t vx;
    int8_t vy;
} GlobalVariables;
#define GV ((GlobalVariables*)0xC000)

void render(void) __z88dk_fastcall
{
    for (int y = 0; y < 32; y++) {
        for (int x = 0; x < 32; x++) {
            VGS0_ADDR_BG->ptn[y][x] = vgs0_noise_oct(10, GV->x + x, GV->y + y);
        }
    }
}

void update(void)
{
    GV->seed += 255;
    vgs0_noise_seed(GV->seed);
    render();
}

void main(void)
{
    // パレットを初期化
    for (int i = 0; i < 16; i++) {
        vgs0_palette_set(0, i, 0, i << 1, 0);
    }

    // Bank 2 を Character Pattern Table (0xA000) に転送 (DMA)
    vgs0_dma(2);

    // グローバル変数を初期化
    vgs0_memset((uint16_t)GV, 0, sizeof(GV));

    // 縮尺を設定（デフォルトの 64 から 32 にすることで縮尺が倍になる）
    vgs0_noise_limitX(32);
    vgs0_noise_limitY(32);

    // 初期設定
    update();
    uint8_t prev = 0;
    uint8_t now = 0;
    while (1) {
        now = vgs0_joypad_get();
        if (0 == (prev & VGS0_JOYPAD_ST) && 0 != (now & VGS0_JOYPAD_ST)) {
            // 乱数の種をズラしてマップを再生成
            update();
        } else {
            // 入力チェック
            if (now & VGS0_JOYPAD_LE) {
                GV->vx = -1;
            } else if (now & VGS0_JOYPAD_RI) {
                GV->vx = 1;
            } else {
                GV->vx = 0;
            }
            if (now & VGS0_JOYPAD_UP) {
                GV->vy = -1;
            } else if (now & VGS0_JOYPAD_DW) {
                GV->vy = 1;
            } else {
                GV->vy = 0;
            }
            // 入力がある場合はスクロール
            if (GV->vx || GV->vy) {
                GV->x += GV->vx;
                GV->y += GV->vy;
                render();
            }
        }
        prev = now;

        // 垂直動機待ち
        vgs0_wait_vsync();
    }
}
