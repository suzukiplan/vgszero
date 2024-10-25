#include "../../lib/z80/stdio.asm"
#define SPRITE_NUM 0 ; 0 = 256

struct OBJ $C000 {
    x ds.w 1
    y ds.w 1
    vx ds.w 1
    vy ds.w 1
    an ds.b 1
}

enum BANK {
    program
    font
}

;------------------------------------------------------------
; メインルーチン
;------------------------------------------------------------
.main
    wait_vblank()                               ; VBLANKを待機
    memcpy(VRAM.palette, palette_data, 512)     ; パレットを初期化
    dma2chr(Bank.font)                          ; font を Character Pattern Table に転送 (DMA)
    memset(OBJ, 0, sizeof(OBJ) * SPRITE_NUM)    ; グローバル変数をゼロクリア
    call init_obj                               ; グローバル変数を初期化
    print_text_fg(2, 2, $80, "SPRITE TEST")     ; テキストを表示
    call draw_bg                                ; BG全画面にひし形を描画
@Loop                                           ; メインループ起点
    wait_vblank()                               ; VBLANKを待機
    inc (VRAM.bgSY)                             ; BGを縦スクロール
    call move_obj                               ; 256個のスプライトを動かす
    jr @Loop                                    ; ループ

;------------------------------------------------------------
; 乱数を用いてオブジェクトの座標を更新
;------------------------------------------------------------
.init_obj
    ld b, SPRITE_NUM
    ld iy, OAM[0]
    ld ix, OBJ[0]
@Loop
    ; X 座標
    in a, (IO.rand8)
    (iy + offset(OAM.x)) = a
    (ix + offset(OBJ.x) + 1) = a

    ; Y 座標
    in a, (IO.rand8)
    a %= 192
    (iy + offset(OAM.y)) = a
    (ix + offset(OBJ.y) + 1) = a

    ; X 座標の移動速度
    in a, (IO.rand16)
    a = h
    a &= 0x01
    h = a
    hl += -256
    (ix + offset(OBJ.vx)) = hl

    ; Y 座標の移動速度
    in a, (IO.rand16)
    a = h
    a &= 0x01
    h = a
    hl += -256
    (ix + offset(OBJ.vy)) = hl

    ; アニメーションパターン
    in a, (IO.rand8)
    (ix + offset(OBJ.an)) = a

    ; OAM 初期値を設定
    a &= %00011100
    a++ >>= 2
    (iy + offset(OAM.ptn)) = a

    ; 属性初期値を設定
    a = $80
    (iy + offset(OAM.attr)) = a

    ix += sizeof(OBJ)
    iy += sizeof(OAM)
    djnz @Loop
    ret

;------------------------------------------------------------
; オブジェクトをアニメーションしながら移動
;------------------------------------------------------------
.move_obj
    ld b, SPRITE_NUM
    ld iy, OAM[0]
    ld ix, OBJ[0]

@Loop
    ; X 座標を計算して OAM を更新
    hl = (ix + offset(OBJ.x))   ; X 座標（固定少数点数）を HL へ
    hl += (ix + offset(OBJ.vx)) ; X 座標へ移動速度（固定少数点数）を加算
    (ix + offset(OBJ.x)) = hl   ; X 座標を更新
    (iy + offset(OAM.x)) = h    ; X 座標の整数部を OAM の X 座標へ設定

    ; Y 座標を計算して OAM を更新
    hl = (ix + offset(OBJ.y))   ; Y 座標（固定少数点数）を HL へ
    hl += (ix + offset(OBJ.vy)) ; Y 座標へ移動速度（固定少数点数）を加算
    (ix + offset(OBJ.y)) = hl   ; Y 座標を更新
    (iy + offset(OAM.y)) = h    ; Y 座標の整数部を OAM の Y 座標へ設定

    ; アニメーション
    a++ = (ix + offset(OBJ.an)) ; アニメーション変数を取得してインクリメント
    (ix + offset(OBJ.an)) = a   ; インクリメント結果のアニメーション変数を保持
    a &= %00011100              ; アニメーション変数からパターン番号（1〜8）を計算 Ph.1
    a++ >>= 2                   ; アニメーション変数からパターン番号（1〜8）を計算 Ph.2
    (iy + offset(OAM.ptn)) = a  ; OAM にアニメーション変数から求めたパターン番号を設定

    ; 次要素へインデックスを移動
    ix += sizeof(OBJ)           ; 次のオブジェクトへインデックスを移動
    iy += sizeof(OAM)           ; 次のOAMへインデックスを移動
    djnz @Loop                  ; SPRITE_NUM 回ループ
    ret

;------------------------------------------------------------
; 画面にひし形を描画
;------------------------------------------------------------
.draw_bg
    hl = VRAM.bg_name
    c = 0
@Loop2
    b = 32
@Loop1
    (hl) = $10
    hl += $400
    a = b
    a &= 1
    a <<= 5
    a |= $80
    (hl) = a
    a = c
    a &= 1
    a <<= 6
    a |= (hl)
    (hl) = a
    hl += -$3FF
    djnz @Loop1
    inc c
    ld a, c
    cp 32
    jr nz, @Loop2
    ret

;------------------------------------------------------------
; データ
;------------------------------------------------------------
palette_data: #binary "palette.bin", 0, 512
