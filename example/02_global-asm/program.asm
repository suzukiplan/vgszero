#include "../../lib/z80/vgszero.inc"

struct VARS $C000 {
    c1      ds.b 1
    c10     ds.b 1
    c100    ds.b 1
    c1000   ds.b 1
    stop    ds.b 1
}

org $0000

.main
    ; スタックポインタを初期化
    ld sp, 0

    ; VBLANKを待機
    call wait_vblank

    ; パレットを初期化
    ld bc, VRAM.palette
    ld de, $2000 ; PRG1
    ld hl, 16 * 16 * 2 ; 16 palettes x 16 colors x sizeof(word)
    out (IO.memcpy), a

    ; Bank 2 を Character Pattern Table に転送 (DMA)
    ld a, $02
    out (IO.dma), a

    ; グローバル変数を初期化
    xor a
    ld (VARS.stop), a++
    ld (VARS.c1000), a++
    ld (VARS.c100), a++
    ld (VARS.c10), a++
    ld (VARS.c1), a++

    ; "COUNT:" を FG(4,4) に表示
    ld de, str_count
    ld hl, $0404
    call print_text

    ; メインループ
@Loop
    ; VBLANKを待機
    call wait_vblank
    call count_up

    ; 1000の位を表示
    ld a, (VARS.c1000)
    add $30     ; TODO: add '0' と書きたい
    ld (VRAM.fg_name + 32 * 4 + 10), a

    ; 100の位を表示
    ld a, (VARS.c100)
    add $30     ; TODO: add '0' と書きたい
    ld (VRAM.fg_name + 32 * 4 + 11), a

    ; 10の位を表示
    ld a, (VARS.c10)
    add $30     ; TODO: add '0' と書きたい
    ld (VRAM.fg_name + 32 * 4 + 12), a

    ; 1の位を表示
    ld a, (VARS.c1)
    add $30     ; TODO: add '0' と書きたい
    ld (VRAM.fg_name + 32 * 4 + 13), a

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
.print_text
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
; 定数定義
;------------------------------------------------------------
str_count: DB "COUNT:****", 0
