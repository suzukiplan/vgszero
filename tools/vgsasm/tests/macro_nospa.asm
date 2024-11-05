#macro foo(arg1, arg2, arg3)
{
    ld bc, arg1
    ld de, arg2
    ld hl, arg3
}

foo(1,2,)

