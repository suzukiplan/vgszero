nop
.label1
nop
label2:
nop
@label3
nop

ld a,(label1)
ld b,(label2)
ld c,(@label3)
ld d,(label1)
ld e,(label2)
ld h,(label3@label2)
ld l,(@label3)
ld ixh,(label1)
ld ixl,(label2)
ld iyh,(@label3)
ld iyl,(label1)
ld hl,(label2)
ld de,(@label3)
ld bc,(label1)
ld sp,(label2)
ld ix,(@label3)
ld iy,(label1)

