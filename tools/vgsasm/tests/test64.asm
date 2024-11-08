struct hoge $C000 {
   hige ds.b 1
}

struct hige {
   foo ds.b 1
}

ld (hoge[3].hige), hige.foo

