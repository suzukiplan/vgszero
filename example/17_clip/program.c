#include "../../lib/sdcc/vgs0lib.h"
#include "palette.h"

void main(void)
{
    init_palette();

    *VGS0_ADDR_FG_DPM = 2;
    *VGS0_ADDR_SPRITE_DPM = 3;
    vgs0_oam_set16(0, (240 - 128) / 2 + 8, (192 - 128) / 2 + 8, 0x80, 0, 15, 15);

    while (1) {
        vgs0_wait_vsync();
        uint8_t pad = vgs0_joypad_get();
        if (pad & VGS0_JOYPAD_LE) {
            VGS0_ADDR_OAM16[0].x -= 4;
        } else if (pad & VGS0_JOYPAD_RI) {
            VGS0_ADDR_OAM16[0].x += 4;
        }
        if (pad & VGS0_JOYPAD_UP) {
            VGS0_ADDR_OAM16[0].y -= 4;
        } else if (pad & VGS0_JOYPAD_DW) {
            VGS0_ADDR_OAM16[0].y += 4;
        }
    }
}
