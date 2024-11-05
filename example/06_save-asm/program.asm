#include "../../lib/z80/stdio.asm"

struct VARS $C000 {
    mcnt ds.b 1
}

struct SRAM <- VARS {
    x ds.b 1
    y ds.b 1
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
    print_text_fg(1, 1, $80, "B=LOAD A=SAVE")   ; テキストを表示
    call init_sprites                           ; スプライト初期化
    memset(VARS, 0, sizeof(VARS));              ; 変数初期化
    data_load(SRAM, sizeof(SRAM))               ; セーブデータ読み込み
    jr z, @Loop                                 ; 読み込み成功したらメインループへ
    SRAM.x = (256 - 16) / 2                     ; X座標
    SRAM.y = (200 - 16) / 2                     ; Y座標
@Loop                                           ; メインループ起点
    wait_vblank()                               ; VBLANKを待機
    call move_obj                               ; オブジェクトの移動
    call render_sprites                         ; スプライト描画
    call print_message                          ; メッセージ表示
    jr @Loop                                    ; ループ

;------------------------------------------------------------
; スプライト初期化
;------------------------------------------------------------
.init_sprites
    OAM[0].attr = $8B                           ; 左上の属性
    OAM[0].ptn = 9                              ; 左上のパターン 
    OAM[1].attr = $CB                           ; 右上の属性
    OAM[1].ptn = 9                              ; 右上のパターン 
    OAM[2].attr = $8B                           ; 左上の属性
    OAM[2].ptn = 10                             ; 左上のパターン 
    OAM[3].attr = $CB                           ; 右下の属性
    OAM[3].ptn = 10                             ; 右下のパターン 
    ret

;------------------------------------------------------------
; オブジェクト移動
;------------------------------------------------------------
.move_obj
    in a, (IO.joypad)
    b = a
    a = (SRAM.x)
@left
    bit PAD.left, b
    jr nz, @right
    a -= 1
    jr @up
@right
    bit PAD.right, b
    jr nz, @up
    a += 1
@up
    SRAM.x = a
    a = (SRAM.y)
    bit PAD.up, b
    jr nz, @down
    a -= 1
    jr @end_move
@down
    bit PAD.down, b
    jr nz, @end_move
    a += 1
@end_move
    SRAM.y = a

    a = (VARS.mcnt)
    a &= a
    jnz @end_save

@save
    bit PAD.a, b
    jr nz, @load
    data_save(SRAM, sizeof(SRAM))
    jr nz, @save_failed
    set_message("SAVE SUCCESS.")
    jr @end_save
@save_failed
    set_message("SAVE FAILED.")
    jr @end_save

@load
    bit PAD.b, b
    jr nz, @end_save
    data_load(SRAM, sizeof(SRAM))
    jr nz, @load_failed
    set_message("LOAD SUCCESS.")
    jr @end_save
@load_failed
    set_message("LOAD FAILED.")
    jr @end_save

@end_save
    ret

;------------------------------------------------------------
; メッセージ設定
;------------------------------------------------------------
#macro set_message(text)
{
    print_text_fg(1, 24, $80, text)
    VARS.mcnt = 60
}

;------------------------------------------------------------
; メッセージ表示
;------------------------------------------------------------
.print_message
    a = (VARS.mcnt)
    a &= a
    ret z
    VARS.mcnt = --a
    jr z, @clear
    cp 59
    ret nz
    ret
@clear
    print_text_fg(1, 24, $00, "               ")    
    ret

;------------------------------------------------------------
; スプライト描画
;------------------------------------------------------------
.render_sprites
    a = (SRAM.x)
    OAM[0].x = a
    OAM[2].x = a
    a += 8
    OAM[1].x = a
    OAM[3].x = a
    a = (SRAM.y)
    OAM[0].y = a
    OAM[1].y = a
    a += 8
    OAM[2].y = a
    OAM[3].y = a
    ret

;------------------------------------------------------------
; データ
;------------------------------------------------------------
palette_data: #binary "palette.bin", 0, 512
