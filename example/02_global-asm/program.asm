#include "../../lib/z80/vgszero.inc"

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

org $0000

.main
    ; スタックポインタを初期化
    ld sp, 0

    ; VBLANKを待機
    call wait_vblank

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
    print_text(4, 4, "COUNT:    ")

@Loop
    call wait_vblank
    call count_up
    call count_print
    jr @Loop

;------------------------------------------------------------
; VBLANKになるまで待機
;------------------------------------------------------------
.wait_vblank
@Loop
    ld a, (VRAM.status)
    and $80
    jr z, @Loop
    ret

;------------------------------------------------------------
; FG へ $00 終端の文字列を表示
; h = X座標
; l = Y座標
; de = 文字列ポインタ
;------------------------------------------------------------
.print_text_sub
    push af
    push bc
    push de
    push hl

    ; 座標をネームテーブルアドレスへ変換
    ; HL = L * 32 + H + VRAM.fg_name
    ld a, h
    ld h, 0
    ld c, 32
    mul hl, c
    add hl, a
    add hl, VRAM.fg_name

@Loop
    ; 文字コード取得
    ld a, (de)
    and a
    jz @End

    ; 文字コード書き込み
    ld (hl), a

    ; 属性更新
    add hl, $0400
    ld a, $80
    ld (hl), a
    add hl, -$03FF

    inc de
    jr @Loop

@End
    pop hl
    pop de
    pop bc
    pop af
    ret

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
