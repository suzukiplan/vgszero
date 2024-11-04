#include "../../lib/z80/stdio.asm"
#define bulette_num 128

struct sprite {
    player ds.b 4
    bulette ds.b bulette_num
}

struct bulette {
    flag ds.b 1
    x ds.w 1
    y ds.w 1
    vx ds.w 1
    vy ds.w 1
    an ds.b 1
}

struct vars $C000 {
    frames ds.b 1
    player_x ds.b 1
    player_y ds.b 1
    bidx ds.b 1
    bulettes bulette bulette_num
}

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
    memset(vars, 0, sizeof(vars))
    dma2chr(bank.font)
    player_init()
    print_text_fg(2, 2, 0x80, "ANGLE:000")

@loop
    hl = vars.frames
    inc (hl)    
    wait_vblank()
    player_move()
    bulette_add()
    bulette_move()
    jr @loop

;------------------------------------------------------------
; プレイヤ初期化
;------------------------------------------------------------
.player_init
    vars.player_x = 120
    vars.player_y = 156

    ix = oam[sprite.player]

    oam[sprite.player].attr = 0x84
    oam[sprite.player + 1].attr = 0x84 + 0x40
    oam[sprite.player + 2].attr = 0x84
    oam[sprite.player + 3].attr = 0x84 + 0x40

    oam[sprite.player + 0].ptn = 9
    oam[sprite.player + 1].ptn = 9
    oam[sprite.player + 2].ptn = 10
    oam[sprite.player + 3].ptn = 10

    player_set_position()
    ret

;------------------------------------------------------------
; プレイヤのスプライト座標を設定
;------------------------------------------------------------
.player_set_position
    a = (vars.player_x)
    oam[sprite.player + 0].x = a
    oam[sprite.player + 2].x = a
    a += 8
    oam[sprite.player + 1].x = a
    oam[sprite.player + 3].x = a

    a = (vars.player_y)
    oam[sprite.player + 0].y = a
    oam[sprite.player + 1].y = a
    a += 8
    oam[sprite.player + 2].y = a
    oam[sprite.player + 3].y = a
    ret

;------------------------------------------------------------
; プレイヤ移動
;------------------------------------------------------------
.player_move
    in a, (io.joypad)
    bit pad.up, a
    call z, @up
    bit pad.down, a
    call z, @down
    bit pad.left, a
    call z, @left
    bit pad.right, a
    call z, @right
    call player_set_position
    ret

@up
    push af
    a = (vars.player_y)
    a -= 2
    vars.player_y = a
    pop af
    ret

@down
    push af
    a = (vars.player_y)
    a += 2
    vars.player_y = a
    pop af
    ret

@left
    push af
    a = (vars.player_x)
    a -= 2
    vars.player_x = a
    pop af
    ret

@right
    push af
    a = (vars.player_x)
    a += 2
    vars.player_x = a
    pop af
    ret

;------------------------------------------------------------
; 4フレームおきに敵弾を追加
;------------------------------------------------------------
.bulette_add
    a = (vars.frames)
    a &= 3
    ret nz

    h = 0
    l = (vars.bidx)
    c = sizeof(bulette)
    hl *= c
    hl += vars.bulettes
    ix = hl

    inc (ix + offset(bulette.flag))

    b = (256 - 8) / 2
    d = b
    d += 4
    c = 0
    (ix + offset(bulette.x)) = bc
    b = ((192 - 8) / 2) + 8
    e = b
    e += 4
    (ix + offset(bulette.y)) = bc

    a = (vars.player_x)
    a += 8
    a -= d
    d = a
    a = (vars.player_y)
    a += 8
    a -= e
    e = a
    atn2 a, de

    a += 0x40
    d = a
    cos a, d
    (ix + offset(bulette.vx)) = a
    bit 7, a
    jr z, @xplus
    a = 0xff
    jr @xset
@xplus
    a = 0x00
@xset
    (ix + offset(bulette.vx) + 1) = a
    hl = (ix + offset(bulette.vx))
    c = 4
    mul hl, c
    (ix + offset(bulette.vx)) = hl

    sin a, d
    (ix + offset(bulette.vy)) = a
    bit 7, a
    jr z, @yplus
    a = 0xff
    jr @yset
@yplus
    a = 0x00
@yset
    (ix + offset(bulette.vy) + 1) = a
    hl = (ix + offset(bulette.vy))
    c = 4
    mul hl, c
    (ix + offset(bulette.vy)) = hl

    h = d
    l = 100
    h /= l
    a = h
    a += '0'
    vram.fg_name + 64 + 8 = a

    h = d
    l = 100
    h %= l
    l = 10
    h /= l
    a = h
    a += '0'
    vram.fg_name + 64 + 9 = a

    h = d
    l = 10
    h %= l
    a = h
    a += '0'
    vram.fg_name + 64 + 10 = a


    a = (vars.bidx)
    a++
    a &= bulette_num - 1
    vars.bidx = a
    ret

;------------------------------------------------------------
; 敵弾を移動
;------------------------------------------------------------
.bulette_move
    b = bulette_num
    ix = vars.bulettes
    iy = oam[sprite.bulette]

@loop
    a = (ix + offset(bulette.flag))
    and a
    jr z, @next
    push bc
    call @move
    pop bc
@next
    ix += sizeof(bulette)
    iy += sizeof(oam)
    djnz @loop
    ret

@move
    hl = (ix + offset(bulette.x))
    bc = (ix + offset(bulette.vx))
    hl += bc
    (ix + offset(bulette.x)) = hl
    a = h
    cp 248
    jr nc, @remove
    (iy + offset(oam.x)) = a

    hl = (ix + offset(bulette.y))
    bc = (ix + offset(bulette.vy))
    hl += bc
    a = h
    cp 200
    jr nc, @remove
    (ix + offset(bulette.y)) = hl
    (iy + offset(oam.y)) = a

    a = (ix + offset(bulette.an))
    (ix + offset(bulette.an)) = ++a
    a &= 0b00011100
    sr a, 2
    (iy + offset(oam.ptn)) = ++a
    (iy + offset(oam.attr)) = 0x80
    ret

@remove
    (ix + offset(bulette.flag)) = 0
    (ix + offset(bulette.an)) = 0
    (iy + offset(oam.attr)) = 0
    ret

;------------------------------------------------------------
; データ
;------------------------------------------------------------
palette_data: #binary "palette.bin", 0, 512
