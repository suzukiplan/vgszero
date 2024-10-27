#include "../lib/vgszero.inc"

enum BANK {
    program
    font
}

.main
    OAM[0].x = a
    OAM[2].x = a
    OAM[1].x = a
    OAM[3].x = a
    OAM[0].y = a
    OAM[1].y = a
    OAM[2].y = a
    OAM[3].y = a
