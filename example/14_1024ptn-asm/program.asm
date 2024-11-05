#include "../../lib/z80/stdio.asm"

enum BANK {
    program
    image
}

;------------------------------------------------------------
; メインルーチン
;------------------------------------------------------------
.main
    wait_vblank()
    memcpy(vram.palette, palette_data, 512)
    vram.bgDPM = bank.image
    vram.mode1024 = 1

    a = 0
    b = 32
    hl = vram.bg_name
@drawY
    push bc
    b = 32
@drawX
    (hl++) = a++
    djnz @drawX
    pop bc
    djnz @drawY

@loop
    wait_vblank()
    in a, (io.joypad)
@check_left
    bit pad.left, a
    jr nz, @check_right
    dec (vram.bgSX)
    jr @check_up
@check_right
    bit pad.right, a
    jr nz, @check_up
    inc (vram.bgSX)

@check_up
    bit pad.up, a
    jr nz, @check_down
    dec (vram.bgSY)
    jr @loop
@check_down
    bit pad.down, a
    jr nz, @loop
    inc (vram.bgSY)
    jr @loop

;------------------------------------------------------------
; データ
;------------------------------------------------------------
palette_data: #binary "palette.bin", 0, 512
