#include "../../lib/z80/stdio.asm"

struct vars 0xC000
{
    isOAM16 ds.b 1
    prev ds.b 1
}

enum bank
{
    program
    font
    sprite
}

.main
    wait_vblank()
    memcpy(VRAM.palette, palette_data, 512)
    vram.fgDPM = bank.font
    vram.spDPM = bank.sprite

    print_text_fg(2, 2, 0x80, "OAM16 MODE");

    vars.isOAM16 = 1
    oam16[0].x = (240 - 128) / 2 + 8
    oam16[0].y = (192 - 128) / 2 + 8
    oam[0].attr = 0x80
    oam[0].w = 15
    oam[0].h = 15

.loop16
    wait_vblank()
    in a, (io.joypad)
@left
    bit pad.left, a
    jr nz, @right
    hl = (oam16[0].x)
    hl += -4
    (oam16[0].x) = hl
    jr @up
@right
    bit pad.right, a
    jr nz, @up
    hl = (oam16[0].x)
    hl += 4
    (oam16[0].x) = hl
@up
    bit pad.up, a
    jr nz, @down
    hl = (oam16[0].y)
    hl += -4
    (oam16[0].y) = hl
    jr @start
@down
    bit pad.down, a
    jr nz, @start
    hl = (oam16[0].y)
    hl += 4
    (oam16[0].y) = hl
@start
    bit pad.start, a
    jr nz, @end
    b = (vars.prev)
    bit pad.start, b
    jr z, @end
    jr @change
@end
    vars.prev = a
    jr loop16
@change
    vars.prev = a
    a = (oam16[0].x)
    oam[0].x = a
    a = (oam16[0].y)
    oam[0].y = a
    hl = 0
    oam16[0].x = hl
    oam16[0].y = hl
    print_text_fg(2, 2, 0x80, "OAM8 MODE ");

.loop8
    wait_vblank()
    in a, (io.joypad)
@left
    bit pad.left, a
    jr nz, @right
    (oam[0].x) += -4
    jr @up
@right
    bit pad.right, a
    jr nz, @up
    (oam[0].x) += 4
@up
    bit pad.up, a
    jr nz, @down
    (oam[0].y) += -4
    jr @start
@down
    bit pad.down, a
    jr nz, @start
    (oam[0].y) += 4

@start
    bit pad.start, a
    jr nz, @end
    b = (vars.prev)
    bit pad.start, b
    jr z, @end
    jr @change
@end
    vars.prev = a
    jr loop8
@change
    vars.prev = a
    a = (oam[0].x)
    oam16[0].x = a
    a = (oam[0].y)
    oam16[0].y = a
    print_text_fg(2, 2, 0x80, "OAM16 MODE");
    jp loop16

;------------------------------------------------------------
; データ
;------------------------------------------------------------
palette_data: #binary "palette.bin", 0, 512
