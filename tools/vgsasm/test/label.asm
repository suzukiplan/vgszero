.main
    LD A, A
    JR @+1
    LD A, B
.hoge
    JR @ + 4
    LD A, C
    LD A, D
    LD A, E
    JR @+0
@foo
    LD A, H
    LD A, L
    NOP
