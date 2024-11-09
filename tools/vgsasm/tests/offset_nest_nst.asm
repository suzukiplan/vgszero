struct hoge {
    var1 ds.b 1
    var2 ds.b 1
    var3 ds.b 1
}

struct foo $C000 {
    var1 ds.b 1
    var2 hoge 1
    var3 hoge 1
}

LD A, offset(foo.var1.var2)

