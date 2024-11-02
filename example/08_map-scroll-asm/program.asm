#include "../../lib/z80/stdio.asm"

struct VARS $C000 {
    next_write ds.b 1
}

enum BANK {
    program
    image
    mapdata
}

#define map_start_position 0x6000 - 32 * 25

;------------------------------------------------------------
; メインルーチン
;------------------------------------------------------------
.main
    wait_vblank()
    memcpy(vram.palette, palette_data, 512)
    dma2chr(bank.image)

    vars.next_write = 0
    memcpy(vram.bg_name, map_start_position, 32 * 25)
    de = map_start_position

@loop
    ; 8px スクロール
    b = 8
@loop8
    wait_vblank()
    dec (vram.bgSY)
    djnz @loop8

    ; マップデータの読み込み位置を計算
    de += -32
    a = d
    a &= 0x1f
    a |= 0x40
    d = a

    ; BG の書き込み位置を計算
    a-- = (vars.next_write)
    a &= 0x1F
    vars.next_write = a
    l = a
    h = 0
    c = 32
    hl *= c
    hl += vram.bg_name
    bc = hl

    ; 次の1行を書き込む
    hl = 32
    out (io.memcpy), a
    jr @loop

;------------------------------------------------------------
; データ
;------------------------------------------------------------
palette_data: #binary "palette.bin", 0, 512
