#include "../../lib/z80/stdio.asm"

struct VARS $C000 {
    mcnt ds.b 1
}

enum BANK {
    program
    image
}

;------------------------------------------------------------
; メインルーチン
;------------------------------------------------------------
.main
    wait_vblank()                               ; VBLANKを待機
    memcpy(vram.palette, palette_data, 512)     ; パレットを初期化
    dma2chr(bank.image)                         ; image を Character Pattern Table に転送 (DMA)

    memset(vram.bg_name, 0, sizeof(vram.bg_name))
    b = 32
    hl = vram.bg_attr
    a = 0
    d = 0
@loop1
    push bc
    b = 32
@loop2
    push af
    a += d
    a &= 0x0F
    (hl++) = a
    pop af
    a++
    djnz @loop2
    pop bc
    d++
    djnz @loop1

@loop3
    wait_vblank()
    inc (vram.bgSY)
    jr @loop3

;------------------------------------------------------------
; データ
;------------------------------------------------------------
palette_data: #binary "palette.bin", 0, 512
