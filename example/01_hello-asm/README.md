# Hello, World

[01_hello](../01_hello/) をアセンブリ言語のみで実装したものです。

![preview](preview.png)

## How to build

### Pre-request

- GNU make
- clang (for building the depended tools)
- [z88dk](https://z88dk.org/site/)
  - require `z80asm` command in this example

### Build

```zsh
git clone https://github.com/suzukiplan/vgszero
cd vgszero/example/01_hello-asm
make
```

## Program

```program.asm
org $0000
.main
    ; 割り込み関連の初期化
    IM 1
    DI

    ; VBLANKを待機
    call wait_vblank

    ; パレットを初期化
    ld bc, 8
    ld hl, palette0_data
    ld de, $9400
    ldir

    ; Bank 1 を Character Pattern Table ($A000) に転送 (DMA)
    ld a, $01
    out ($c0), a

    ; 画面中央付近 (10,12) に "HELLO,WORLD!" を描画
    ld bc, 12
    ld hl, hello_text
    ld de, 394 + $8000 ; 394 = 12 * 32 + 10
    ldir

    ; メインループ
mainloop:
    ; VBLANKを待機
    call wait_vblank

    ; ジョイパッド(1P)の入力を読み取る
    in a, ($A0)
    ld b, a

    ; 左カーソルが押されているかチェック（押されている場合は左スクロール）
    ld hl, $9602
    and %00100000
    jp nz, mainloop_check_right
    inc (hl)
    jmp mainloop_check_up
mainloop_check_right:
    ; 右カーソルが押されているかチェック（押されている場合は右スクロール）
    ld a, b
    and %00010000
    jp nz, mainloop_check_up
    dec (hl)
mainloop_check_up:
    ; 上カーソルが押されているかチェック（押されている場合は上スクロール）
    ld hl, $9603
    ld a, b
    and %10000000
    jp nz, mainloop_check_down
    inc (hl)
    jmp mainloop_check_end
mainloop_check_down:
    ; 下カーソルが押されているかチェック（押されている場合は下スクロール）
    ld a, b
    and %01000000
    jp nz, mainloop_check_end
    dec (hl)
mainloop_check_end:
    jmp mainloop

; VBLANKになるまで待機
.wait_vblank
    ld hl, $9607
wait_vblank_loop:
    ld a, (hl)
    and $80
    jp z, wait_vblank_loop
    ret

palette0_data: defw %0000000000000000, %0001110011100111, %0110001100011000, %0111111111111111
hello_text: defb "HELLO,WORLD!"
```