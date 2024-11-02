.label1
nop
label2:
nop
@label3
nop

ld bc, label1
ld de, label2
ld hl, @label3
ld sp, label3@label2
ld ix, label1
ld iy, label2


