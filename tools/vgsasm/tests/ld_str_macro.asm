struct vars 0xC000 {
   foo ds.b 1
}

#macro hoge() {
ld a, (vars.foo)
}

hoge()

