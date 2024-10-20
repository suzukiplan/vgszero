org $0000
.Reset
    IM 1
    LD SP, 0
    JP Main

org $0038
.Interrupt
    RETI

org $0100
.Main
    EI
    HALT
    DI
    HALT

org $2000
Data:  DB "Hello, World!", 0x00
