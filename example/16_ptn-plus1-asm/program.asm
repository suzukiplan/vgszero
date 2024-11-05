#include "../../lib/z80/stdio.asm"

enum bank
{
    program
    font0
    font1
}

.main
    wait_vblank()
    memcpy(VRAM.palette, palette_data, 512)
    vram.bgDPM = bank.font0
    vram.fgDPM = bank.font0
    print_text_bg(2, 2, 0x80, "USE FONT0 BG.")    // reset PTN flag of attribute
    print_text_bg(2, 4, 0x90, "USE FONT0+1 BG.")  // set PTN flag of attribute
    print_text_fg(2, 6, 0x80, "USE FONT0 FG.")    // reset PTN flag of attribute
    print_text_fg(2, 8, 0x90, "USE FONT0+1 FG.")  // set PTN flag of attribute
    jr @+0

;------------------------------------------------------------
; データ
;------------------------------------------------------------
palette_data: #binary "palette.bin", 0, 512
