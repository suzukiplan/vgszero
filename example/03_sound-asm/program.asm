#include "../../lib/z80/stdio.asm"

struct VARS $C000 {
    cur ds.b 1          ; メニューカーソル
    joy_prev ds.b 1     ; ジョイパッド（前フレーム）
}


enum BANK {
    program
    font
}

enum BGM {
    song1
    song2
    song3
    song4
}

enum SFX {
    move
    enter
}

;------------------------------------------------------------
; メインルーチン
;------------------------------------------------------------
.main
    ; VBLANKを待機
    wait_vblank()

    ; パレットを初期化
    memcpy(VRAM.palette, palette_data, 512)

    ; font を Character Pattern Table に転送 (DMA)
    dma2chr(BANK.font)

    ; グローバル変数を初期化
    memset(VARS, 0, sizeof(VARS))
    VARS.cur = 7

    ; メニューを表示
    print_text_fg(6, 7, $80, "> PLAY: #1 PRELUDE")
    print_text_fg(6, 9, $80, "  PLAY: #2 GOLDBERG")
    print_text_fg(6, 11, $80, "  PLAY: #3 WTC1")
    print_text_fg(6, 13, $80, "  PLAY: #4 NSF")
    print_text_fg(6, 15, $80, "  PAUSE")
    print_text_fg(6, 17, $80, "  RESUME")
    print_text_fg(6, 19, $80, "  FADEOUT")

; メインループ
.main_loop
    wait_vblank()

@Check
    in a, (IO.joypad)
    hl = VARS.joy_prev

@CheckUp
    bit PAD.up, (hl)
    jr z, @CheckDown
    bit PAD.up, a
    jr nz, @CheckDown
    call move_cur_up
    jr @CheckA

@CheckDown
    bit PAD.down, (hl)
    jr z, @CheckA
    bit PAD.down, a
    jr nz, @CheckA
    call move_cur_down

@CheckA
    bit PAD.a, (hl)
    jr z, @CheckB
    bit PAD.a, a
    jr nz, @CheckB
    call select_bgm
    jr @End

@CheckB
    bit PAD.b, (hl)
    jr z, @End
    bit PAD.b, a
    jr nz, @End
    call select_bgm

@End
    (hl) = a
    jr main_loop

;------------------------------------------------------------
; BGM を選択
;------------------------------------------------------------
.select_bgm
    push_all_without_i()
    a = (VARS.cur)
    a -= 7
    sr a ; a /= 2
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
    pop_all_without_i()
    eff_play(SFX.enter)
    ret

;------------------------------------------------------------
; カーソルを上へ移動
;------------------------------------------------------------
.move_cur_up
    push_all_without_i()
    call clear_cur
    a = (VARS.cur)
    cp 7
    jr z, @over
    a -= 2
    jr @move
@over
    a = 19
@move
    VARS.cur = a
    call draw_cur
    pop_all_without_i()
    ret

;------------------------------------------------------------
; カーソルを下へ移動
;------------------------------------------------------------
.move_cur_down
    push_all_without_i()
    call clear_cur
    a = (VARS.cur)
    cp 19
    jr z, @over
    a += 2
    jr @move
@over
    a = 7
@move
    VARS.cur = a
    call draw_cur
    pop_all_without_i()
    ret

;------------------------------------------------------------
; カーソル描画
;------------------------------------------------------------
.draw_cur
    call get_cur_pos
    (hl) = '>'
    eff_play(SFX.move)
    ret

;------------------------------------------------------------
; カーソルクリア
;------------------------------------------------------------
.clear_cur
    call get_cur_pos
    (hl) = ' '
    ret

;------------------------------------------------------------
; カーソルの位置取得
;------------------------------------------------------------
.get_cur_pos
    a = (VARS.cur)
    l = a
    h = 0
    c = 32
    mul hl, c
    hl += 6
    hl += VRAM.fg_name
    ret

;------------------------------------------------------------
; データ
;------------------------------------------------------------
palette_data: #binary "palette.bin", 0, 512
