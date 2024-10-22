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
    a = 1
    out (IO.dma), a

    ; グローバル変数を初期化
    a ^= a
    VARS.stop = a++
    VARS.c1000 = a++
    VARS.c100 = a++
    VARS.c10 = a++
    VARS.c1 = a

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
    a = (VARS.stop)
    a &= a
    ret nz

@Up1
    a = (VARS.c1)
    VARS.c1 = ++a
    cp 10
    ret c

@Up10
    VARS.c1 = 0
    a = (VARS.c10)
    VARS.c10 = ++a
    cp 10
    ret c

@Up100
    VARS.c10 = 0
    a = (VARS.c100)
    VARS.c100 = ++a
    cp 10
    ret c

@Up1000
    VARS.c100 = 0
    a = (VARS.c1000)
    VARS.c1000 = ++a
    cp 10
    ret c

@CountStop
    VARS.stop = 1
    VARS.c1 = 9
    VARS.c10 = 9
    VARS.c100 = 9
    VARS.c1000 = 9
    ret

;------------------------------------------------------------
; カウント表示
;------------------------------------------------------------
.count_print
    ; 1000の位を表示
    a = (VARS.c1000)
    a += '0'
    VRAM.fg_name + 32 * 4 + 10 = a

    ; 100の位を表示
    a = (VARS.c100)
    a += '0'
    VRAM.fg_name + 32 * 4 + 11 = a

    ; 10の位を表示
    a = (VARS.c10)
    a += '0'
    VRAM.fg_name + 32 * 4 + 12 = a

    ; 1の位を表示
    a = (VARS.c1)
    a += '0'
    VRAM.fg_name + 32 * 4 + 13 = a
    ret

;------------------------------------------------------------
; 定数定義
;------------------------------------------------------------
palette_data: #binary "palette.bin", 0, 512
