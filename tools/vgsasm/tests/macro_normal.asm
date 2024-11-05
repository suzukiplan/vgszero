#include "vgszero.inc"

#macro hoge(arg1, arg2, arg3) {
    LD BC, arg1
    LD DE, arg2
    LD HL, arg3
}

struct foo $C000 {
    var ds.w 1
}

org $0000
    hoge(1, 2 + 3, 3 * 4 + 3)
    memcpy(0xC000, "String", 7)
    memset(0xC020, ' ', 1024)
    hige()
    hoge(1, 2, foo)

.hige
    ret
