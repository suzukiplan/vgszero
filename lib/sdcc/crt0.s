.area   _HEADER (ABS)
.globl _main
.ORG 0x0000
    im 1
    di
    ld hl, #0x9607
wait_vdp_standby:
    ld a, (hl)
    and #0x80
    jp z, wait_vdp_standby
    call _main
    di
    halt
