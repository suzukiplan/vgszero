#macro hoge(foo, arg2, arg3) {
    LD BC, arg1
    LD DE, arg2
    LD HL, arg3
}

struct foo 0 {
    hoge ds.b 1
    hige ds.b 1
}

org $0000
    hoge(1, 2 + 3, 3 * 4 + 3)
    HALT
