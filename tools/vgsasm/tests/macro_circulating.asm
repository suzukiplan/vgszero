#macro hoge(arg1, arg2, arg3) {
    LD BC, arg1
    LD DE, arg2
    LD HL, arg3
    hoge(arg1, arg2, arg3)
}

#macro hige(arg1, arg2, arg3) {
    LD BC, arg1
    LD DE, arg2
    LD HL, arg3
    hoge(arg1, arg2, arg3)
}

org $0000
    hoge(1, 2 + 3, 3 * 4 + 3)
    HALT
