#pragma once
#include "../../lib/sdcc/vgs0lib.h"

struct GlobalVariables {
    struct Sprite {
        union {
            uint16_t value;
            uint8_t raw[2];
        } x;
        union {
            uint16_t value;
            uint8_t raw[2];
        } y;
    } sprite[256];
};

#define GV ((struct GlobalVariables*)0xC000)
