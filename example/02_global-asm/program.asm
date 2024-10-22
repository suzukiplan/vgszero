#include "../../lib/z80/stdio.asm"

struct VARS $C000 {
    c1      ds.b 1
    c10     ds.b 1
    c100    ds.b 1
    c1000   ds.b 1
    stop    ds.b 1
}

#macro print_text(posX, posY, string)
{
    ld h, posX
    ld l, posY
    ld de, string
    call print_text_sub
}

.main
    ; VBLANKを待機
    wait_vblank()

    ; パレットを初期化
    memcpy(VRAM.palette, palette_data, 512);

    ; Bank 1 を Character Pattern Table に転送 (DMA)
    ld a, $01
    out (IO.dma), a

    ; グローバル変数を初期化
    xor a
    ld (VARS.stop), a++
    ld (VARS.c1000), a++
    ld (VARS.c100), a++
    ld (VARS.c10), a++
    ld (VARS.c1), a++

    ; "COUNT:" を FG(4,4) に表示
    print_text_fg(4, 4, 0x80, "COUNT:    ")

@Loop
    wait_vblank()
    call count_up
    call count_print
    jr @Loop

;------------------------------------------------------------
; カウントアップ
;------------------------------------------------------------
.count_up
    ld a, (VARS.stop)
    and $FF
    ret nz

@Up1
    ld a, (VARS.c1)
    inc a
    ld (VARS.c1), a
    cp 10
    ret c

@Up10
    xor a
    ld (VARS.c1), a
    ld a, (VARS.c10)
    inc a
    ld (VARS.c10), a
    cp 10
    ret c

@Up100
    xor a
    ld (VARS.c10), a
    ld a, (VARS.c100)
    inc a
    ld (VARS.c100), a
    cp 10
    ret c

@Up1000
    xor a
    ld (VARS.c100), a
    ld a, (VARS.c1000)
    inc a
    ld (VARS.c1000), a
    cp 10
    ret c

@CountStop
    inc (VARS.stop)
    ld a, 9
    ld (VARS.c1), a
    ld (VARS.c10), a
    ld (VARS.c100), a
    ld (VARS.c1000), a
    ret

;------------------------------------------------------------
; カウント表示
;------------------------------------------------------------
.count_print
    ; 1000の位を表示
    ld a, (VARS.c1000)
    add '0'
    ld (VRAM.fg_name + 32 * 4 + 10), a

    ; 100の位を表示
    ld a, (VARS.c100)
    add '0'
    ld (VRAM.fg_name + 32 * 4 + 11), a

    ; 10の位を表示
    ld a, (VARS.c10)
    add '0'
    ld (VRAM.fg_name + 32 * 4 + 12), a

    ; 1の位を表示
    ld a, (VARS.c1)
    add '0'
    ld (VRAM.fg_name + 32 * 4 + 13), a
    ret

;------------------------------------------------------------
; 定数定義
;------------------------------------------------------------
palette_data: #binary "palette.bin", 0, 512
