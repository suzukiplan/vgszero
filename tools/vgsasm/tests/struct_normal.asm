struct hoge $C000 {
    var1 ds.b 1
    var2 ds.w 1
    var3 ds.w 1
}

struct hige <- hoge {
    var1 ds.b 1
    var2 ds.w 1
    var3 ds.w 1
}

struct hage <- hoge[3] {
    var1 ds.b 1
    var2 ds.w 1
    var3 ds.w 1
}

LD HL, hoge.var1
LD HL, hoge.var2
LD HL, hoge.var3

LD DE, hige.var1
LD DE, hige.var2
LD DE, hige.var3

LD BC, hage.var1
LD BC, hage.var2
LD BC, hage.var3

LD BC, hage[3].var1
LD BC, hage[3].var2
LD BC, hage[3].var3
