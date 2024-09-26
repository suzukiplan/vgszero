#include "../../lib/sdcc/vgs0lib.h"
#include "palette.h"

void main(void)
{
    init_palette();

    // DPM を Bank1 にする
    *VGS0_ADDR_BG_DPM = 2;
    *VGS0_ADDR_FG_DPM = 2;
    *VGS0_ADDR_SPRITE_DPM = 2;

    vgs0_bg_putstr(2, 2, 0x80, "USE FONT0 BG.");   // reset PTN flag of attribute
    vgs0_bg_putstr(2, 4, 0x90, "USE FONT0+1 BG."); // set PTN flag of attribute
    vgs0_fg_putstr(2, 6, 0x80, "USE FONT0 FG.");   // reset PTN flag of attribute
    vgs0_fg_putstr(2, 8, 0x90, "USE FONT0+1 FG."); // set PTN flag of attribute

    while (1) {
        ;
    }
}
