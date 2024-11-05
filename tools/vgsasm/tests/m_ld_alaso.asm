nop
.label1
nop
label2:
nop
@label3
nop
@label4

ld (label1), a
ld (label2), b
ld (@label3), c
ld (@label4), d
ld (label1),e
ld (label2),h
ld (@label3),l
ld (@label4),ixh
ld (label1),ixl
ld (label2),iyh
ld (@label3),iyl
ld (label4@label2),bc
ld (label1),de
ld (label2),hl
ld (label3@label2),sp
ld (label4@label2),ix
ld (label1),iy

