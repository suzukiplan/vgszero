/**
 * VGS-Zero for RaspberryPi Baremetal Environment - Standard Library Hooks
 * License under GPLv3: https://github.com/suzukiplan/vgszero/blob/master/LICENSE-VGS0.txt
 * (C)2023, SUZUKI PLAN
 */
#include <circle/startup.h>
#include <stdarg.h>

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
