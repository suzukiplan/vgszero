/**
 * VGS-Zero for RaspberryPi Baremetal Environment - Configuration Parser
 * License under GPLv3: https://github.com/suzukiplan/vgszero/blob/master/LICENSE-VGS0.txt
 * (C)2023, SUZUKI PLAN
 */
#pragma once
#ifndef TEST
#include <circle/usb/usbgamepad.h>
#endif
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

class SystemConfiguration
{
public:
    enum class ButtonType
    {
        Unknown,
        A,
        B,
        SELECT,
        START,
        UP,
        DOWN,
        LEFT,
        RIGHT
    };

    class Button
    {
    public:
        ButtonType type;
        unsigned bitMask;
        bool isAxis;
        int axisIndex;
        bool compareLessThan;
        int compareValue;

    private:
        void setup(ButtonType type_, unsigned bitMask_)
        {
            this->type = type_;
            this->bitMask = bitMask_;
            this->isAxis = false;
            this->axisIndex = 0;
            this->compareLessThan = false;
            this->compareValue = 0;
        }

        void setup(ButtonType type_, int axisIndex_, bool compareLessThan_, int compareValue_)
        {
            this->type = type_;
            this->bitMask = 0;
            this->isAxis = true;
            this->axisIndex = axisIndex_;
            this->compareLessThan = compareLessThan_;
            this->compareValue = compareValue_;
        }

        bool isdigit(int n)
        {
            return '0' <= n && n <= '9';
        }

    public:
        Button(const char* text)
        {
            while (' ' == *text || '\t' == *text) {
                text++;
            }
            ButtonType type_ = ButtonType::Unknown;
            if (0 == strncmp(text, "START", 5)) {
                type_ = ButtonType::START;
                text += 5;
            } else if (0 == strncmp(text, "SELECT", 6)) {
                type_ = ButtonType::SELECT;
                text += 6;
            } else if (0 == strncmp(text, "UP", 2)) {
                type_ = ButtonType::UP;
                text += 2;
            } else if (0 == strncmp(text, "DOWN", 4)) {
                type_ = ButtonType::DOWN;
                text += 4;
            } else if (0 == strncmp(text, "LEFT", 4)) {
                type_ = ButtonType::LEFT;
                text += 4;
            } else if (0 == strncmp(text, "RIGHT", 5)) {
                type_ = ButtonType::RIGHT;
                text += 5;
            } else if (0 == strncmp(text, "A", 1)) {
                type_ = ButtonType::A;
                text += 1;
            } else if (0 == strncmp(text, "B", 1)) {
                type_ = ButtonType::B;
                text += 1;
            } else {
                this->setup(ButtonType::Unknown, 0);
                return;
            }
            while (' ' == *text || '\t' == *text) {
                text++;
            }

            if (0 == strncmp(text, "BUTTON_", 7)) {
                text += 7;
                this->setup(type_, 1 << atoi(text));
            } else if (0 == strncmp(text, "AXIS_", 5)) {
                text += 5;
                int axisIndex_ = atoi(text);
                if (!isdigit(*text)) {
                    this->setup(ButtonType::Unknown, 0);
                    return;
                }
                while (isdigit(*text)) {
                    text++;
                }
                if (' ' != *text && '\t' != *text) {
                    this->setup(ButtonType::Unknown, 0);
                    return;
                }
                while (' ' == *text || '\t' == *text) {
                    text++;
                }
                if ('<' != *text && '>' != *text) {
                    this->setup(ButtonType::Unknown, 0);
                    return;
                }
                bool compareLessThan_ = *text == '<';
                text++;
                if (' ' != *text && '\t' != *text) {
                    this->setup(ButtonType::Unknown, 0);
                    return;
                }
                while (' ' == *text || '\t' == *text) {
                    text++;
                }
                this->setup(type_, axisIndex_, compareLessThan_, atoi(text));
            } else {
                this->setup(ButtonType::Unknown, 0);
                return;
            }
        }

        Button(ButtonType type_, unsigned bitMask_) { this->setup(type_, bitMask_); }
        Button(ButtonType type_, int axisIndex_, bool compareLessThan_, int compareValue_) { this->setup(type_, axisIndex_, compareLessThan_, compareValue_); }

#ifndef TEST
        bool check(const TGamePadState* state)
        {
            if (this->isAxis) {
                if (this->compareLessThan) {
                    return state->axes[this->axisIndex].value < this->compareValue;
                } else {
                    return state->axes[this->axisIndex].value > this->compareValue;
                }
            } else {
                return state->buttons & this->bitMask ? true : false;
            }
        }
#endif
    };

    Button* buttonA;
    Button* buttonB;
    Button* buttonStart;
    Button* buttonSelect;
    Button* buttonUp;
    Button* buttonDown;
    Button* buttonLeft;
    Button* buttonRight;

    SystemConfiguration(const char* text)
    {
        this->buttonA = new Button(ButtonType::A, 0x0002);
        this->buttonB = new Button(ButtonType::B, 0x0001);
        this->buttonStart = new Button(ButtonType::START, 0x0200);
        this->buttonSelect = new Button(ButtonType::SELECT, 0x0100);
        this->buttonUp = new Button(ButtonType::UP, 1, true, 64);
        this->buttonDown = new Button(ButtonType::DOWN, 1, false, 192);
        this->buttonLeft = new Button(ButtonType::LEFT, 0, true, 64);
        this->buttonRight = new Button(ButtonType::RIGHT, 0, false, 192);

        while (*text) {
            Button* button = new Button(text);
            switch (button->type) {
                case ButtonType::Unknown:
                    delete button;
                    break;
                case ButtonType::A:
                    delete this->buttonA;
                    this->buttonA = button;
                    break;
                case ButtonType::B:
                    delete this->buttonB;
                    this->buttonB = button;
                    break;
                case ButtonType::START:
                    delete this->buttonStart;
                    this->buttonStart = button;
                    break;
                case ButtonType::SELECT:
                    delete this->buttonSelect;
                    this->buttonSelect = button;
                    break;
                case ButtonType::UP:
                    delete this->buttonUp;
                    this->buttonUp = button;
                    break;
                case ButtonType::DOWN:
                    delete this->buttonDown;
                    this->buttonDown = button;
                    break;
                case ButtonType::LEFT:
                    delete this->buttonLeft;
                    this->buttonLeft = button;
                    break;
                case ButtonType::RIGHT:
                    delete this->buttonRight;
                    this->buttonRight = button;
                    break;
            }
            while (*text != '\n' && *text != '\0') {
                text++;
            }
            if (*text == '\n') {
                text++;
            }
        }
    }

    ~SystemConfiguration()
    {
        if (this->buttonA) delete this->buttonA;
        if (this->buttonB) delete this->buttonB;
        if (this->buttonStart) delete this->buttonStart;
        if (this->buttonSelect) delete this->buttonSelect;
        if (this->buttonUp) delete this->buttonUp;
        if (this->buttonDown) delete this->buttonDown;
        if (this->buttonLeft) delete this->buttonLeft;
        if (this->buttonRight) delete this->buttonRight;
    }
};
