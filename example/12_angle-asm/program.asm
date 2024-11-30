#include "../../lib/z80/stdio.asm"
#define bulette_num 128

struct sprite {
    player ds.b 4
    target ds.b 1
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

struct rect {
    x ds.b 1
    y ds.b 1
    width ds.b 1
    height ds.b 1
}

struct vars $C000 {
    frames ds.b 1
    player_x ds.b 1
    player_y ds.b 1
    bidx ds.b 1
    rects rect 2
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

    oam[sprite.target].x = (240 - 16) / 2 + 8
    oam[sprite.target].y = ((192 - 16) / 2) + 8
    oam[sprite.target].ptn = 0x0B
    oam[sprite.target].attr = 0x80
    oam[sprite.target].w = 1
    oam[sprite.target].h = 1

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
    bit pad.start, a
    jp z, target_move
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
; ターゲット移動
;------------------------------------------------------------
.target_move
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
    a = (oam[sprite.target].y)
    a -= 2
    oam[sprite.target].y = a
    pop af
    ret

@down
    push af
    a = (oam[sprite.target].y)
    a += 2
    oam[sprite.target].y = a
    pop af
    ret

@left
    push af
    a = (oam[sprite.target].x)
    a -= 2
    oam[sprite.target].x = a
    pop af
    ret

@right
    push af
    a = (oam[sprite.target].x)
    a += 2
    oam[sprite.target].x = a
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

    a = (oam[sprite.target].x)
    a += 4
    b = a
    c = 0
    (ix + offset(bulette.x)) = bc
    vars.rects[0].x = b
    a = (oam[sprite.target].y)
    a += 4
    b = a
    (ix + offset(bulette.y)) = bc
    vars.rects[0].y = b

    a = (vars.player_x)
    vars.rects[1].x = a
    a = (vars.player_y)
    vars.rects[1].y = a

    a = 16
    vars.rects[0].width = a
    vars.rects[0].height = a
    vars.rects[1].width = a
    vars.rects[1].height = a

    hl = vars.rects
    in a, (io.angle)
    hl = bc
    hl += bc
    hl += bc
    hl += bc
    (ix + offset(bulette.vx)) = hl
    hl = de
    hl += de
    hl += de
    hl += de
    (ix + offset(bulette.vy)) = hl

    d = a
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
