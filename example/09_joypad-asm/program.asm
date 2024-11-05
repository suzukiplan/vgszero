#include "../../lib/z80/stdio.asm"

enum BANK {
    program
    font
}

;------------------------------------------------------------
; メインルーチン
;------------------------------------------------------------
.main
    wait_vblank()
    memcpy(vram.palette, palette_data, 512)
    dma2chr(bank.font)

    // ジョイパッドを描画
    a = 10
    bc = vram.bg_name + 4 * 32 + 4
    de = joypad_tile
    hl = 23
@joypad_tile_draw_loop
    out (io.memcpy), a
    bc += 32
    de += 23
    dec a
    jr nz, @joypad_tile_draw_loop

    // 中央に表示するためにずらす
    vram.bgSX = -4
    vram.bgSY = -4

    ld ix, oam[0]
    set_sprite_cursor(24, 32)   // 上カーソル (0~3)
    set_sprite_cursor(24, 56)   // 下カーソル (4~7)
    set_sprite_cursor(12, 44)   // 左カーソル (8~11)
    set_sprite_cursor(36, 44)   // 右カーソル (12~15)
    set_sprite_ctrl(60, 61)     // SELECT (16~18)
    set_sprite_ctrl(88, 61)     // START (19~21)
    set_sprite_button(124, 50)  // B (22~29)
    set_sprite_button(150, 50)  // A (30~37)

@loop
    wait_vblank()
    call check_cur_up
    call check_cur_down
    call check_cur_left
    call check_cur_right
    call check_ctrl_select
    call check_ctrl_start
    call check_button_b
    call check_button_a
    jr @loop

;------------------------------------------------------------
; カーソルの入力チェック
;------------------------------------------------------------
.check_cur_up
    in a, (io.joypad)
    bit pad.up, a
    ix = oam[0]
    jr check_cur

.check_cur_down
    in a, (io.joypad)
    bit pad.down, a
    ix = oam[4]
    jr check_cur

.check_cur_left
    in a, (io.joypad)
    bit pad.left, a
    ix = oam[8]
    jr check_cur

.check_cur_right
    in a, (io.joypad)
    bit pad.right, a
    ix = oam[12]
    jr check_cur

.check_cur
    b = 4
    jr nz, @hide
    c = 0x80
    jr @loop
@hide
    c = 0x00
@loop
    (ix + offset(oam.attr)) = c
    ix += sizeof(oam)
    djnz @loop
    ret

;------------------------------------------------------------
; 制御ボタンの入力チェック
;------------------------------------------------------------
.check_ctrl_select
    in a, (io.joypad)
    bit pad.select, a
    ix = oam[16]
    jr check_ctrl

.check_ctrl_start
    in a, (io.joypad)
    bit pad.start, a
    ix = oam[19]
    jr check_ctrl

.check_ctrl
    b = 3
    jr nz, @hide
    c = 0x80
    jr @loop
@hide
    c = 0x00
@loop
    (ix + offset(oam.attr)) = c
    ix += sizeof(oam)
    djnz @loop
    ret

;------------------------------------------------------------
; ABボタンの入力チェック
;------------------------------------------------------------
.check_button_b
    in a, (io.joypad)
    bit pad.b, a
    ix = oam[22]
    jr check_button

.check_button_a
    in a, (io.joypad)
    bit pad.a, a
    ix = oam[30]
    jr check_button

.check_button
    b = 8
    jr nz, @hide
    c = 0x80
    jr @loop
@hide
    c = 0x00
@loop
    (ix + offset(oam.attr)) = c
    ix += sizeof(oam)
    djnz @loop
    ret

;------------------------------------------------------------
; カーソルスプライトを設定
;------------------------------------------------------------
#macro set_sprite_cursor(x, y)
{
    b = x
    c = y
    call set_sprite_cursor_sub
}

.set_sprite_cursor_sub
    a = 36
    a += b
    (ix + offset(oam.x)) = a
    a = 36
    a += c
    (ix + offset(oam.y)) = a
    (ix + offset(oam.ptn)) = 0x01
    ix += sizeof(oam)

    a = 36 + 8
    a += b
    (ix + offset(oam.x)) = a
    a = 36
    a += c
    (ix + offset(oam.y)) = a
    (ix + offset(oam.ptn)) = 0x02
    ix += sizeof(oam)

    a = 36
    a += b
    (ix + offset(oam.x)) = a
    a = 36 + 8
    a += c
    (ix + offset(oam.y)) = a
    (ix + offset(oam.ptn)) = 0x03
    ix += sizeof(oam)

    a = 36 + 8
    a += b
    (ix + offset(oam.x)) = a
    a = 36 + 8
    a += c
    (ix + offset(oam.y)) = a
    (ix + offset(oam.ptn)) = 0x04
    ix += sizeof(oam)
    ret

;------------------------------------------------------------
; 制御ボタンのスプライトを設定
;------------------------------------------------------------
#macro set_sprite_ctrl(x, y)
{
    b = x
    c = y
    call set_sprite_ctrl_sub
}

.set_sprite_ctrl_sub
    a = 36
    a += b
    (ix + offset(oam.x)) = a
    a = 36
    a += c
    (ix + offset(oam.y)) = a
    (ix + offset(oam.ptn)) = 0x05
    ix += sizeof(oam)

    a = 36 + 8
    a += b
    (ix + offset(oam.x)) = a
    a = 36
    a += c
    (ix + offset(oam.y)) = a
    (ix + offset(oam.ptn)) = 0x06
    ix += sizeof(oam)

    a = 36 + 16
    a += b
    (ix + offset(oam.x)) = a
    a = 36
    a += c
    (ix + offset(oam.y)) = a
    (ix + offset(oam.ptn)) = 0x07
    ix += sizeof(oam)
    ret


;------------------------------------------------------------
; A/Bのスプライトを設定
;------------------------------------------------------------
#macro set_sprite_button(x, y)
{
    b = x
    c = y
    call set_sprite_button_sub
}

.set_sprite_button_sub
    a = 36
    a += b
    (ix + offset(oam.x)) = a
    a = 36
    a += c
    (ix + offset(oam.y)) = a
    (ix + offset(oam.ptn)) = 0xC0
    ix += sizeof(oam)

    a = 36 + 8
    a += b
    (ix + offset(oam.x)) = a
    a = 36
    a += c
    (ix + offset(oam.y)) = a
    (ix + offset(oam.ptn)) = 0xC1
    ix += sizeof(oam)

    a = 36 + 16
    a += b
    (ix + offset(oam.x)) = a
    a = 36
    a += c
    (ix + offset(oam.y)) = a
    (ix + offset(oam.ptn)) = 0xC2
    ix += sizeof(oam)

    a = 36
    a += b
    (ix + offset(oam.x)) = a
    a = 36 + 8
    a += c
    (ix + offset(oam.y)) = a
    (ix + offset(oam.ptn)) = 0xD0
    ix += sizeof(oam)

    a = 36 + 8
    a += b
    (ix + offset(oam.x)) = a
    a = 36 + 8
    a += c
    (ix + offset(oam.y)) = a
    (ix + offset(oam.ptn)) = 0xD1
    ix += sizeof(oam)

    a = 36 + 16
    a += b
    (ix + offset(oam.x)) = a
    a = 36 + 8
    a += c
    (ix + offset(oam.y)) = a
    (ix + offset(oam.ptn)) = 0xD2
    ix += sizeof(oam)

    a = 36
    a += b
    (ix + offset(oam.x)) = a
    a = 36 + 16
    a += c
    (ix + offset(oam.y)) = a
    (ix + offset(oam.ptn)) = 0xE0
    ix += sizeof(oam)

    a = 36 + 8
    a += b
    (ix + offset(oam.x)) = a
    a = 36 + 16
    a += c
    (ix + offset(oam.y)) = a
    (ix + offset(oam.ptn)) = 0xE1
    ix += sizeof(oam)
    ret

;------------------------------------------------------------
; データ
;------------------------------------------------------------
palette_data:
#binary "palette.bin", 0, 512

joypad_tile:
    db 0x60, 0x60, 0x60, 0x60, 0x60, 0x60, 0x60, 0x60, 0x60, 0x60, 0x60, 0x60, 0x61, 0x62, 0x63, 0x64, 0x65, 0x66, 0x67, 0x68, 0x69, 0x6A, 0x6B
    db 0x6C, 0x6D, 0x6D, 0x6D, 0x6D, 0x6D, 0x6D, 0x6E, 0x60, 0x60, 0x60, 0x60, 0x60, 0x60, 0x60, 0x60, 0x60, 0x60, 0x60, 0x60, 0x60, 0x60, 0x6F
    db 0x70, 0x71, 0x71, 0x71, 0x71, 0x71, 0x71, 0x72, 0x60, 0x60, 0x60, 0x60, 0x60, 0x60, 0x60, 0x60, 0x60, 0x60, 0x60, 0x60, 0x60, 0x60, 0x6F
    db 0x70, 0x71, 0x73, 0x74, 0x75, 0x71, 0x71, 0x72, 0x60, 0x60, 0x60, 0x60, 0x60, 0x60, 0x60, 0x60, 0x60, 0x60, 0x60, 0x60, 0x60, 0x60, 0x6F
    db 0x70, 0x71, 0x76, 0x77, 0x78, 0x71, 0x71, 0x79, 0x7A, 0x7A, 0x7A, 0x7A, 0x7A, 0x7A, 0x7A, 0x7A, 0x7A, 0x7A, 0x7A, 0x7A, 0x7A, 0x7A, 0x7B
    db 0x70, 0x7C, 0x7D, 0x77, 0x7E, 0x7F, 0x80, 0x71, 0x71, 0x71, 0x71, 0x71, 0x71, 0x71, 0x71, 0x71, 0x81, 0x71, 0x71, 0x82, 0x83, 0x71, 0x84
    db 0x70, 0x84, 0x77, 0x77, 0x77, 0x77, 0x85, 0x86, 0x87, 0x88, 0x89, 0x8A, 0x8B, 0x8C, 0x71, 0x8D, 0x8E, 0x8F, 0x90, 0x91, 0x92, 0x93, 0x84
    db 0x94, 0x95, 0x96, 0x77, 0x97, 0x98, 0x99, 0x9A, 0x9B, 0x9B, 0x9C, 0x9D, 0x9B, 0x9E, 0x9F, 0xA0, 0xA1, 0xA2, 0xA3, 0xA4, 0xA5, 0xA6, 0xA7
    db 0xA8, 0xA9, 0xAA, 0xAB, 0xAC, 0xAD, 0xAD, 0xAE, 0xAF, 0xAF, 0xB0, 0xB1, 0xAF, 0xB2, 0xB3, 0xB4, 0xB5, 0xB6, 0xB7, 0xB8, 0xB9, 0xBA, 0xBB
    db 0xBC, 0xBD, 0xBD, 0xBD, 0xBD, 0xBD, 0xBD, 0xBD, 0xBD, 0xBD, 0xBD, 0xBD, 0xBD, 0xBD, 0xBD, 0xBD, 0xBD, 0xBD, 0xBD, 0xBD, 0xBD, 0xBD, 0xBE
