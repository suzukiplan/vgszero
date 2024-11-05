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
    print_text_fg(8, 7, $80, "> PLAY")
    print_text_fg(8, 9, $80, "  PAUSE")
    print_text_fg(8, 11, $80, "  RESUME")
    print_text_fg(8, 13, $80, "  FADEOUT")

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
    bit pad.a, (hl)
    jr z, @checkB
    bit pad.a, a
    jr nz, @checkB
    call select_menu
    jr @end

@CheckB
    bit pad.b, (hl)
    jr z, @checkStart
    bit pad.b, a
    jr nz, @checkStart
    call select_menu
    jr @end

@CheckStart
    bit pad.start, (hl)
    jr z, @end
    bit pad.start, a
    jr nz, @end
    call select_menu

@End
    (hl) = a
    jr main_loop

;------------------------------------------------------------
; メニュー選択
;------------------------------------------------------------
.select_menu
    push af
    sfx_play(sfx.enter)
    a = (vars.cur)
    cp 7
    jr z, @play
    cp 9
    jr z, @pause
    cp 11
    jr z, @resume
    jr @fadeout

@play
    bgm_play(bgm.song1)
    pop af
    ret

@pause
    bgm_pause()
    pop af
    ret

@resume
    bgm_resume()
    pop af
    ret

@fadeout
    bgm_fadeout()
    pop af
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
    a = 13
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
    cp 13
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
    sfx_play(SFX.move)
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
    hl *= c
    hl += 8
    hl += VRAM.fg_name
    ret

;------------------------------------------------------------
; データ
;------------------------------------------------------------
palette_data: #binary "palette.bin", 0, 512
