#include "../lib/vgszero.inc"

struct Foo $C000 {
    var1 ds.b 1
    var2 ds.b 1
    var3 ds.b 1
}

struct Hoge <- Foo {
    var1 ds.b 1
}

struct Hige <- Foo[3] {
    var1 ds.b 1
}

struct VARS $C000 {
    msg ds.w 1
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

.Main
    ld bc, VRAM.oam             ; BC = $9000
    ld hl, sizeof(OAM) * 256    ; HL = OAM size (8) x 256 = 2048
    ld bc, VRAM.oam + 1 * 0x21  ; BC = $9021
    ld (OAM), a
    ld (OAM[0].x), a 
    ld (OAM[1].x), a 
    ld (OAM[2].x), a 
    ld (OAM[3].x), a 
    ld (OAM[1+2*3].x), a 
    ld (OAM[1+2*3].x + 3), a 

    ld hl, hoge.var1
    ld hl, hige.var1
