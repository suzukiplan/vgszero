#include "../../src/rpizero2/src/config.hpp"

#include <fstream>
#include <iostream>
#include <string>

static void show(SystemConfiguration::Button* button)
{
    switch (button->type) {
        case SystemConfiguration::ButtonType::A: printf("A"); break;
        case SystemConfiguration::ButtonType::B: printf("B"); break;
        case SystemConfiguration::ButtonType::START: printf("START"); break;
        case SystemConfiguration::ButtonType::SELECT: printf("SELECT"); break;
        case SystemConfiguration::ButtonType::UP: printf("UP"); break;
        case SystemConfiguration::ButtonType::DOWN: printf("DOWN"); break;
        case SystemConfiguration::ButtonType::LEFT: printf("LEFT"); break;
        case SystemConfiguration::ButtonType::RIGHT: printf("RIGHT"); break;
        case SystemConfiguration::ButtonType::Unknown: printf("Unknown"); break;
    }
    if (button->isAxis) {
        printf(": axis[%d] %s than %d\n", button->axisIndex, button->compareLessThan ? "less" : "more", button->compareValue);
    } else {
        printf(": bitmask=%08X\n", button->bitMask);
    }
}

int main(int argc, char* argv[])
{
    if (argc < 2) {
        puts("chkcfg /path/to/config.sys");
        return 1;
    }
    std::ifstream ifs(argv[1]);
    if (ifs.fail()) {
        puts("file not found");
        return -1;
    }
    std::string str((std::istreambuf_iterator<char>(ifs)), std::istreambuf_iterator<char>());
    SystemConfiguration cfg(str.c_str());
    show(cfg.buttonA);
    show(cfg.buttonB);
    show(cfg.buttonSelect);
    show(cfg.buttonStart);
    show(cfg.buttonUp);
    show(cfg.buttonDown);
    show(cfg.buttonLeft);
    show(cfg.buttonRight);
    return 0;
}