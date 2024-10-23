#include "../../lib/z80/vgszero.inc"

org $0000

.main
    ; スタックポインタを初期化
    ld sp, 0

    ; VBLANKを待機
    wait_vblank()

    ; パレットを初期化
    memcpy(VRAM.palette, palette0_data, 8)

    ; Bank 1 を Character Pattern Table ($A000) に転送 (DMA)
    dma2chr($01)

    ; 画面中央付近 (10,12) に "HELLO,WORLD!" を描画
    memcpy(12 * 32 + 10 + VRAM.bg_name, "HELLO,WORLD!", 12)

    ; メインループ
@Loop
    ; VBLANKを待機
    wait_vblank()

    ; ジョイパッド(1P)の入力を読み取る
    in a, (IO.joypad)

@CheckLeft
    ; 左カーソルが押されているかチェック（押されている場合は左スクロール）
    bit PAD.left, a
    jr nz, @CheckRight
    inc (VRAM.bgSX)
    jr @CheckUp 

@CheckRight
    ; 右カーソルが押されているかチェック（押されている場合は右スクロール）
    bit PAD.right, a
    jr nz, @CheckUp
    dec (VRAM.bgSX)

@CheckUp
    ; 上カーソルが押されているかチェック（押されている場合は上スクロール）
    bit PAD.up, a
    jr nz, @CheckDown
    inc (VRAM.bgSY)
    jr @CheckEnd

@CheckDown
    ; 下カーソルが押されているかチェック（押されている場合は下スクロール）
    bit PAD.down, a
    jr nz, @CheckEnd
    dec (VRAM.bgSY)

@CheckEnd
    jr @Loop

palette0_data: DW %0000000000000000, %0001110011100111, %0110001100011000, %0111111111111111
