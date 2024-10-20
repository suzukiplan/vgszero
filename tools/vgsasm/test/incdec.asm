org $0000

.Increment
    NOP
    LD (HL++), A
    NOP
    LD (++HL), A

.Decrement
    NOP
    LD (HL--), A
    NOP
    LD (--HL), A

.Multiple
    NOP
    LD (HL++), --A
