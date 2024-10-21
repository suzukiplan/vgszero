#include "../lib/vgszero.inc"

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
