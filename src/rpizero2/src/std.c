/**
 * VGS-Zero for RaspberryPi Baremetal Environment - Standard Library Hooks
 * License under GPLv3: https://github.com/suzukiplan/vgszero/blob/master/LICENSE-VGS0.txt
 * (C)2023, SUZUKI PLAN
 */
#include <circle/startup.h>
#include <stdarg.h>

extern const unsigned short vgs0_rand16[65536];

void exit(int code)
{
    halt();
}

signed long time(signed long* second)
{
    return 0;
}

void* localtime(const signed long* _timer)
{
    return (void*)0;
}

int printf(const char* format, ...)
{
    return 0;
}

int puts(const char* text)
{
    return 0;
}

const char* strrchr(const char* string, int c)
{
    int len = 0;
    for (; 0 != string[len]; len++) {
        ;
    }
    len--;
    while (0 <= len) {
        if (string[len] == c) {
            return &string[len];
        }
    }
    return (const char*)0;
}

int tolower(int c)
{
    if ('A' <= c && c <= 'Z') {
        return c + ('a' - 'A');
    } else {
        return c;
    }
}

int rand(void)
{
    static unsigned short ptr;
    return vgs0_rand16[ptr++] & 0x7FFF;
}