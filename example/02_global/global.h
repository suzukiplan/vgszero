#pragma once
#include "../../lib/sdcc/vgs0lib.h"

struct GlobalVariables {
    uint8_t c1;
    uint8_t c10;
    uint8_t c100;
    uint8_t c1000;
    uint8_t stop;
};

#define GV ((struct GlobalVariables*)0xC000)
