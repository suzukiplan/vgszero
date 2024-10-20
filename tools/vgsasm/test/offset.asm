struct OAM $9000 {
    y           ds.b    1
    x           ds.b    1
    ptn         ds.b    1
    attr        ds.b    1
    h           ds.b    1
    w           ds.b    1
    bank        ds.b    1
    reserved    ds.b    1
}

org $0000

.Main
    ld ix, OAM[16]
    ld b, 4
@Loop
    ld (ix + offset(OAM.x)), 0x10
    ld (ix + offset(OAM.y)), 0x20
    ld (ix + offset(OAM.ptn)), 0x30
    ld (ix + offset(OAM.bank)), 0x40
    add ix, sizeof(OAM)
    djnz @Loop
