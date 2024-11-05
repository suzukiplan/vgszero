#include "../../lib/z80/stdio.asm"

enum BANK {
    program
    bg
    fg
    sprite
}

;------------------------------------------------------------
; メインルーチン
;------------------------------------------------------------
.main
    wait_vblank()
    memcpy(vram.palette, palette_data, 512)
    vram.bgDPM = bank.bg
    vram.fgDPM = bank.fg
    vram.spDPM = bank.sprite

    ; 左(16x16) = BG, 右(16x16) = FG で埋める
    ix = vram.bg_name
    iy = vram.fg_name + 16
    hl = vram.fg_attr + 16
    a = 0
    b = 16
@loop2
    push bc
    b = 16
@loop1
    (ix++) = a
    (iy++) = a++
    (hl++) = 0x80
    djnz @loop1
    ix += 16
    iy += 16
    hl += 16
    pop bc
    djnz @loop2

    ; スクロールして表示位置を調整
    (vram.fgSY) = -8
    (vram.bgSY) = -8

    ; スプライトで下 32x9 を埋める
    b = 9
    c = 0
    d = 0x80
    e = 136
    ix = oam[0]
@loop3
    (ix + offset(oam.ptn)) = c
    (ix + offset(oam.attr)) = 0x80
    (ix + offset(oam.x)) = 0
    (ix + offset(oam.y)) = e
    (ix + offset(oam.w)) = 15
    c += 16
    ix += sizeof(oam)

    (ix + offset(oam.ptn)) = d
    (ix + offset(oam.attr)) = 0x80
    (ix + offset(oam.x)) = 128
    (ix + offset(oam.y)) = e
    (ix + offset(oam.w)) = 15
    d += 16
    e += 8
    ix += sizeof(oam)
    djnz @loop3

    jr @+0

;------------------------------------------------------------
; データ
;------------------------------------------------------------
palette_data: #binary "palette.bin", 0, 512
