#include "../lib/vgszero.inc"

struct Foo $C000 {
    var1 ds.b 1     ; param1
    var2 ds.b 1     ; param2
    var3 ds.b 1     ; param3
}

enum Color
{
    Black       // color code 0
    Blue        // color code 1
    Red         // color code 2
    Purple      // color code 3
    Green       // color code 4
    Cyan        // color code 5
    Yellow      // color code 6
    White       // color code 7
}

.main
    LD A, 0x12
    LD (Foo.var1), $12
    SL A
    SLL A
    LD (IX + 2), %1010
    LD (IY + 1), 0b1010
    LD a, (VRAM.bg_attr)
    LD a, Color.Green
