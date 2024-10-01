#include "../../lib/sdcc/vgs0lib.h"
#include "palette.h"

void main(void)
{
    init_palette();

    *VGS0_ADDR_FG_DPM = 2;
    *VGS0_ADDR_SPRITE_DPM = 3;
    vgs0_oam_set16(0, (240 - 128) / 2 + 8, (192 - 128) / 2 + 8, 0x80, 0, 15, 15);

    uint8_t isOAM16 = 1;
    uint8_t padPrev = 0;

    while (1) {
        vgs0_wait_vsync();
        uint8_t pad = vgs0_joypad_get();

        if (isOAM16) {
            vgs0_putstr(VGS0_ADDR_FG, 2, 2, 0x80, "OAM16 MODE");
        } else {
            vgs0_putstr(VGS0_ADDR_FG, 2, 2, 0x80, "OAM MODE  ");
        }

        // Move Sprite
        if (isOAM16) {
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
        } else {
            if (pad & VGS0_JOYPAD_LE) {
                VGS0_ADDR_OAM[0].x -= 4;
            } else if (pad & VGS0_JOYPAD_RI) {
                VGS0_ADDR_OAM[0].x += 4;
            }
            if (pad & VGS0_JOYPAD_UP) {
                VGS0_ADDR_OAM[0].y -= 4;
            } else if (pad & VGS0_JOYPAD_DW) {
                VGS0_ADDR_OAM[0].y += 4;
            }
        }

        // Switch OAM and OAM16
        if (pad & VGS0_JOYPAD_ST && !(padPrev & VGS0_JOYPAD_ST)) {
            if (isOAM16) {
                VGS0_ADDR_OAM[0].x = VGS0_ADDR_OAM16[0].x & 0xFF;
                VGS0_ADDR_OAM[0].y = VGS0_ADDR_OAM16[0].y & 0xFF;
                VGS0_ADDR_OAM16[0].x = 0;
                VGS0_ADDR_OAM16[0].y = 0;
                isOAM16 = 0;
            } else {
                VGS0_ADDR_OAM16[0].x = VGS0_ADDR_OAM[0].x;
                VGS0_ADDR_OAM16[0].y = VGS0_ADDR_OAM[0].y;
                VGS0_ADDR_OAM[0].x = 0;
                VGS0_ADDR_OAM[0].y = 0;
                isOAM16 = 1;
            }
        }

        padPrev = pad;
    }
}
