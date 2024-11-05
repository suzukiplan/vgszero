struct foo $C000 {
   hoge ds.b 1
   hige ds.b 1
}

struct fuga {
   a ds.b 1
   b ds.b 1
}

ld ix, (foo[fuga.a])
ld iy, (foo[fuga.b])
ld hl, (foo[fuga.b + 3])
ld hl, (foo[fuga.b] + 3)

ld ix, (foo[fuga.a].hoge)
ld iy, (foo[fuga.b].hoge)
ld hl, (foo[fuga.b + 3].hoge)
ld hl, (foo[fuga.b].hoge + 3)

ld ix, (foo[fuga.a].hige)
ld iy, (foo[fuga.b].hige)
ld hl, (foo[fuga.b + 3].hige)
ld hl, (foo[fuga.b].hige + 3)

