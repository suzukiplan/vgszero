#include "../../lib/z80/vgszero.inc"

org $0000

.main
    ; VBLANKを待機
    call wait_vblank

    ; パレットを初期化
    ld bc, VRAM.palette
    ld de, palette0_data
    ld hl, 8
    out (IO.memcpy), a

    ; Bank 1 を Character Pattern Table ($A000) に転送 (DMA)
    ld a, $01
    out (IO.dma), a

    ; 画面中央付近 (10,12) に "HELLO,WORLD!" を描画
    ld bc, 12 * 32 + 10 + VRAM.bg_name
    ld de, hello_text
    ld hl, 12
    out (IO.memcpy), a

    ; メインループ
@Loop
    ; VBLANKを待機
    call wait_vblank

    ; ジョイパッド(1P)の入力を読み取る
    in a, (IO.joypad)

@CheckLeft
    ; 左カーソルが押されているかチェック（押されている場合は左スクロール）
    ld hl, VRAM.bgSX
    bit PAD.left, a
    jr nz, @CheckRight
    inc (hl)
    jr @CheckUp 

@CheckRight
    ; 右カーソルが押されているかチェック（押されている場合は右スクロール）
    bit PAD.right, a
    jr nz, @CheckUp
    dec (hl)

@CheckUp
    ; 上カーソルが押されているかチェック（押されている場合は上スクロール）
    ld hl, VRAM.bgSY
    bit PAD.up, a
    jr nz, @CheckDown
    inc (hl)
    jr @CheckEnd

@CheckDown
    ; 下カーソルが押されているかチェック（押されている場合は下スクロール）
    bit PAD.down, a
    jr nz, @CheckEnd
    dec (hl)

@CheckEnd
    jr @Loop

; VBLANKになるまで待機
.wait_vblank
    ld hl, VRAM.status
@Loop
    ld a, (hl)
    and $80
    jr z, @Loop
    ret

palette0_data: DW %0000000000000000, %0001110011100111, %0110001100011000, %0111111111111111
hello_text: DB "HELLO,WORLD!"
