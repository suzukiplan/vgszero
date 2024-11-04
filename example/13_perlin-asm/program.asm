#include "../../lib/z80/stdio.asm"
#define bulette_num 128

struct vars $C000 {
    seed ds.w 1
    x ds.w 1
    y ds.w 1
    prev ds.b 1
}

enum BANK {
    program
    image
}

;------------------------------------------------------------
; メインルーチン
;------------------------------------------------------------
.main
    wait_vblank()
    memcpy(vram.palette, palette_data, 512)
    memset(vars, 0, sizeof(vars))
    dma2chr(bank.image)
    init_perlin()

@loop
    wait_vblank()
    render_map()

@check_start
    in a, (io.joypad)
    bit pad.start, a
    jr nz, @check_up
    b = (vars.prev)
    bit pad.start, b
    jr z, @check_up
    update_perlin()

@check_up
    vars.prev = a
    hl = vars.y
    bit pad.up, a
    jr nz, @check_down
    dec (hl)
    jr @check_left
@check_down
    bit pad.down, a
    jr nz, @check_left
    inc (hl)
@check_left
    hl = vars.x
    bit pad.left, a
    jr nz, @check_right
    dec (hl)
    jr @loop
@check_right
    bit pad.right, a
    jr nz, @loop
    inc (hl)
    jr @loop

;------------------------------------------------------------
; パーリンノイズの初期化
;------------------------------------------------------------
.init_perlin
    hl = 32
    out (io.perlin_x_scale), a
    hl = 32
    out (io.perlin_y_scale), a
    update_perlin()
    ret

;------------------------------------------------------------
; パーリンノイズのシードを更新
;------------------------------------------------------------
.update_perlin
    hl = (vars.seed)
    hl += 255
    vars.seed = hl
    out (io.perlin_seed), a
    ret

;------------------------------------------------------------
; マップを描画
;------------------------------------------------------------
.render_map
    ix = vram.bg_name
    hl = (vars.x)
    de = (vars.y)

    b = 32
@loopY
    push bc
    b = 32
@loopX
    a = 10
    in a, (io.perlin_get_oct)
    (ix++) = a
    hl++
    djnz @loopX

    pop bc
    hl += -32
    de++
    djnz @loopY
    ret

;------------------------------------------------------------
; データ
;------------------------------------------------------------
palette_data: #binary "palette.bin", 0, 512
