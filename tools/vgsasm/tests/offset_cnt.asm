struct hoge {
    var1 ds.b 1 
    var2 ds.b 2 
    var3 ds.b 3 
}

struct foo $C000 {
    var1 ds.b 1 
    var2 hoge 2 
    var3 hoge 3 
}

LD A, offset(foo.var3.var3) ; expected 16 

