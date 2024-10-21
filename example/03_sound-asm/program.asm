#include "../../lib/z80/vgszero.inc"

struct VARS $C000 {
    cur ds.b 1          ; メニューカーソル
    joy_prev ds.b 1     ; ジョイパッド（前フレーム）
    push_up ds.b 1      ; 上ボタン押下
    push_down ds.b 1    ; 下ボタン押下
    push_a ds.b 1       ; Aボタン押下
    push_b ds.b 1       ; Bボタン押下
}

struct Bank 0 {
    program ds.b 1
    font ds.b 1
}

struct BankS 0 {
    move ds.b 1
    enter ds.b 1
}

struct BankB 0 {
    song1 ds.b 1
    song2 ds.b 1
    song3 ds.b 1
    song4 ds.b 1
}

;------------------------------------------------------------
; メインルーチン
;------------------------------------------------------------
org $0000
.main
    ; スタックポインタを初期化
    ld sp, 0

    ; VBLANKを待機
    call wait_vblank

    ; パレットを初期化
    memcpy(VRAM.palette, palette_data, 512)

    ; font を Character Pattern Table に転送 (DMA)
    ld a, Bank.font
    out (IO.dma), a

    ; グローバル変数をゼロクリア
    memset(VARS, 0, sizeof(VARS))
    ld (VARS.cur), 7

    ; メニューを表示
    print_text(6, 7, "> PLAY: #1 PRELUDE")
    print_text(6, 9, "  PLAY: #2 GOLDBERG")
    print_text(6, 11, "  PLAY: #3 WTC1")
    print_text(6, 13, "  PLAY: #4 NSF")
    print_text(6, 15, "  PAUSE")
    print_text(6, 17, "  RESUME")
    print_text(6, 19, "  FADEOUT")

; メインループ
.main_loop
    call wait_vblank

@Check
    in a, (IO.joypad)
    ld hl, VARS.joy_prev

@CheckUp
    bit PAD.up, (hl)
    jr z, @CheckDown
    bit PAD.up, a
    jr nz, @CheckDown
    call move_cur_up

@CheckDown
    bit PAD.down, (hl)
    jr z, @CheckA
    bit PAD.down, a
    jr nz, @CheckA
    call move_cur_down

@CheckA
    bit PAD.a, (hl)
    jr z, @End
    bit PAD.a, a
    jr nz, @End
    call select_bgm

@End
    ld (hl), a
    jr main_loop

;------------------------------------------------------------
; BGM を選択
;------------------------------------------------------------
.select_bgm
    push_all()
    ld a, (VARS.cur)
    sub 7
    sr a
    cp 4
    jr c, @Play
    jr z, @Pause
    cp 5
    jr z, @Resume
    jr @Fadeout
@Pause
    bgm_pause()
    jr @End
@Resume
    bgm_resume()
    jr @End
@Fadeout
    bgm_fadeout()
    jr @End
@Play
    out (IO.bgm.play), a
@End
    pop_all()
    eff_play(BankS.enter)
    ret

;------------------------------------------------------------
; カーソルを上へ移動
;------------------------------------------------------------
.move_cur_up
    push_all()
    call clear_cur
    ld a, (VARS.cur)
    cp 7
    jr z, @over
    sub 2
    jr @move
@over
    ld a, 19
@move
    ld (VARS.cur), a
    call draw_cur
    pop_all()
    ret

;------------------------------------------------------------
; カーソルを下へ移動
;------------------------------------------------------------
.move_cur_down
    push_all()
    call clear_cur
    ld a, (VARS.cur)
    cp 19
    jr z, @over
    add 2
    jr @move
@over
    ld a, 7
@move
    ld (VARS.cur), a
    call draw_cur
    pop_all()
    ret

;------------------------------------------------------------
; カーソル描画
;------------------------------------------------------------
.draw_cur
    call get_cur_pos
    ld a, '>'
    ld (hl), a
    eff_play(BankS.move)
    ret

;------------------------------------------------------------
; カーソルクリア
;------------------------------------------------------------
.clear_cur
    call get_cur_pos
    ld a, ' '
    ld (hl), a
    ret

;------------------------------------------------------------
; カーソルの位置取得
;------------------------------------------------------------
.get_cur_pos
    ld a, (VARS.cur)
    ld l, a
    ld h, 0
    ld c, 32
    mul hl, c
    add hl, 6
    add hl, VRAM.fg_name
    ret

;------------------------------------------------------------
; スタック
;------------------------------------------------------------
#macro push_all()
{
    push af
    push bc
    push de
    push hl
}

#macro pop_all()
{
    pop hl
    pop de
    pop bc
    pop af
}

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
;------------------------------------------------------------
#macro print_text(posX, posY, string)
{
    push_all()
    ld h, posX
    ld l, posY
    ld de, string
    call print_text_sub
    pop_all()
}

.print_text_sub
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
    ret z

    ; 文字コード書き込み
    ld (hl), a

    ; 属性更新
    add hl, $0400
    ld a, $80
    ld (hl), a
    add hl, -$03FF

    inc de
    jr @Loop

;------------------------------------------------------------
; データ
;------------------------------------------------------------
palette_data: #binary "palette.bin", 0, 512
