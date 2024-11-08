struct foo {
    a ds.b 1
    b ds.b 1
}

struct vars 0xC000 {
    foos foo 3
}

LD HL, vars.foos.a ; expected 0xC000
LD HL, vars.foos.b ; expected 0xC001
LD HL, vars.foos[0].a ; expected 0xC000
LD HL, vars.foos[0].b ; expected 0xC001
LD HL, vars.foos[1].a ; expected 0xC002
LD HL, vars.foos[1].b ; expected 0xC003
LD HL, vars.foos[2].a ; expected 0xC004
LD HL, vars.foos[2].b ; expected 0xC005

; also do it!
LD HL, vars[1].foos.a ; expected 0xC006
LD HL, vars[1].foos.b ; expected 0xC007
LD HL, vars[1].foos[0].a ; expected 0xC006
LD HL, vars[1].foos[0].b ; expected 0xC007
LD HL, vars[1].foos[1].a ; expected 0xC008
LD HL, vars[1].foos[1].b ; expected 0xC009
LD HL, vars[1].foos[2].a ; expected 0xC00A
LD HL, vars[1].foos[2].b ; expected 0xC00B

