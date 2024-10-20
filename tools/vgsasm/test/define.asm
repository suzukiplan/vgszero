#include "../lib/vgszero.inc"

#define ClearA XOR A
#define Hoge
#define Hige LD A, (HL)

.Main
    ClearA
    Hoge
    Hige
