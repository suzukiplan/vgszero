#define DEBUG

#ifdef DEBUG
ld a, a
#else
ld a, b
#endif

#ifndef DEBUG
ld a, c
#else
ld a, d
#endif

