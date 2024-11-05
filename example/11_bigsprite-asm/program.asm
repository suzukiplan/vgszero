#include "../../lib/z80/stdio.asm"

enum BANK {
    program
    image00
    image01
    image02
    image03
    image04
    image05
    image06
    image07
    image08
    image09
    image10
    image11
    image12
    image13
    image14
    image15
}

struct OBJ $C000
{
    flag    ds.b    1
    big     ds.b    1
    an      ds.b    1
    pi      ds.b    1
}

struct VARS $C000
{
    objs    OBJ     256
    oidx    ds.b    1 
}


;------------------------------------------------------------
; メインルーチン
;------------------------------------------------------------
.main
    wait_vblank()
    memcpy(vram.palette, palette_data, 512)
    memset(vars, 0, sizeof(vars))
    vram.spDPM = bank.image00

@loop
    wait_vblank()
    add_new_bomb()
    animate_bomb()
    jr @loop

;------------------------------------------------------------
; 新規爆破
;------------------------------------------------------------
.add_new_bomb
    // ix をオブジェクトの先頭アドレスに設定
    h = 0
    l = (vars.oidx)
    c = sizeof(obj)
    hl *= c
    hl += obj
    ix = hl

    // フラグが非ゼロなら追加を抑止
    a = (ix + offset(obj.flag))
    and a
    ret nz

    // iy に OAM の先頭アドレスを設定
    h = 0
    l = (vars.oidx)
    c = sizeof(oam)
    hl *= c
    hl += oam
    iy = hl

    // obj に初期値を設定
    (ix + offset(obj.flag)) = 1
    (ix + offset(obj.an)) = 0
    (ix + offset(obj.pi)) = 0
    a++ = (vars.oidx)
    vars.oidx = a
    and 7
    jr z, @set_big_flag
    (ix + offset(obj.big)) = 0
    jr @set_end
@set_big_flag
    (ix + offset(obj.big)) = 1
@set_end

    // oam に粗基地を設定
    in a, (io.rand8)
    (iy + offset(oam.x)) = a
    in a, (io.rand8)
    mod a, 192
    (iy + offset(oam.y)) = a
    (iy + offset(oam.attr)) = 0x80
    (iy + offset(oam.ptn)) = 0x00

    a = (ix + offset(obj.big))
    and a
    jr nz, @set_big

@set_nobig
    (iy + offset(oam.bank)) = bank.image00
    (iy + offset(oam.w)) = 2
    (iy + offset(oam.h)) = 2
    ret

@set_big
    (iy + offset(oam.bank)) = bank.image01
    (iy + offset(oam.w)) = 11
    (iy + offset(oam.h)) = 11
    ret

;------------------------------------------------------------
; 爆破を動かす
;------------------------------------------------------------
.animate_bomb
    b = 0
    ix = obj[0]
    iy = oam[0]
@loop
    ; フラグが立ってなければ何もしない
    a = (ix + offset(obj.flag))
    a &= a
    jr z, @next

    ; 4フレームに1回だけ動かす
    a++ = (ix + offset(obj.an))
    (ix + offset(obj.an)) = a
    a &= 3
    jr nz, @next

    ; 巨大かそうでないかで処理を分ける
    a = (ix + offset(obj.big))
    a &= a
    jr z, @small

@big; 大きい爆発アニメーション
    a++ = (iy + offset(oam.bank))
    cp 17
    jr z, @delete
    (iy + offset(oam.bank)) = a
    jr @next

@small; 小さい爆発アニメーション
    a++ = (ix + offset(obj.pi))
    cp 10
    jr z, @delete
    (ix + offset(obj.pi)) = a
    hl = ptnnum
    hl += a
    a = (hl)
    (iy + offset(oam.ptn)) = a
    jr @next

@delete; アニメーション終了
    (ix + offset(obj.flag)) = 0
    (iy + offset(oam.attr)) = 0

@next; 次へ
    ix += sizeof(obj)
    iy += sizeof(oam)
    djnz @loop
    ret

;------------------------------------------------------------
; データ
;------------------------------------------------------------
; パレット
palette_data:
#binary "palette.bin", 0, 512

; image00 のパターン番号
ptnnum:
    db 0x00, 0x03, 0x06, 0x09, 0x0C, 0x30, 0x33, 0x36, 0x39, 0x3C
