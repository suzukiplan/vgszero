/**
 * VGS-Zero - SDK for Steam
 * License under GPLv3: https://github.com/suzukiplan/vgszero/blob/master/LICENSE-VGS0.txt
 * (C)2024, SUZUKI PLAN
 */
#pragma once
#include <Xinput.h>
#include <dinput.h>
#include <new>
#include <string.h>
#include <string>
#include <windows.h>

class KeyConfig
{
  public:
    enum class Type {
        Unknown,
        Button,
        LeftTrigger,
        RightTrigger,
        Axis,
    };

    enum class AxisType {
        None,
        LeftX,
        LeftY,
        RightX,
        RightY,
    };

    unsigned char pad;
    Type type;
    int buttonMask;
    AxisType axisType;
    bool isLessThan;
    int compareValue;

    static std::string toString(unsigned char pad)
    {
        if (pad & VGS0_JOYPAD_T1) return "A";
        if (pad & VGS0_JOYPAD_T2) return "B";
        if (pad & VGS0_JOYPAD_ST) return "START";
        if (pad & VGS0_JOYPAD_SE) return "SELECT";
        if (pad & VGS0_JOYPAD_UP) return "UP";
        if (pad & VGS0_JOYPAD_DW) return "DOWN";
        if (pad & VGS0_JOYPAD_LE) return "LEFT";
        if (pad & VGS0_JOYPAD_RI) return "RIGHT";
        return "Unknown";
    }

    static unsigned char toPad(std::string str)
    {
        if (0 == _stricmp(str.c_str(), "A")) {
            return VGS0_JOYPAD_T1;
        } else if (0 == _stricmp(str.c_str(), "B")) {
            return VGS0_JOYPAD_T2;
        } else if (0 == _stricmp(str.c_str(), "START")) {
            return VGS0_JOYPAD_ST;
        } else if (0 == _stricmp(str.c_str(), "SELECT")) {
            return VGS0_JOYPAD_SE;
        } else if (0 == _stricmp(str.c_str(), "UP")) {
            return VGS0_JOYPAD_UP;
        } else if (0 == _stricmp(str.c_str(), "DOWN")) {
            return VGS0_JOYPAD_DW;
        } else if (0 == _stricmp(str.c_str(), "LEFT")) {
            return VGS0_JOYPAD_LE;
        } else if (0 == _stricmp(str.c_str(), "RIGHT")) {
            return VGS0_JOYPAD_RI;
        } else {
            return 0x00;
        }
    }

    static std::string toString(Type type)
    {
        switch (type) {
            case Type::Button: return "Button";
            case Type::LeftTrigger: return "LeftTrigger";
            case Type::RightTrigger: return "RightTrigger";
            case Type::Axis: return "Axis";
            default: return "Unknown";
        }
    }

    static std::string toString(AxisType type)
    {
        switch (type) {
            case AxisType::LeftX: return "LeftX";
            case AxisType::LeftY: return "LeftY";
            case AxisType::RightX: return "RightX";
            case AxisType::RightY: return "RightY";
            default: return "None";
        }
    }

    static Type toType(std::string str)
    {
        if (0 == _stricmp(str.c_str(), "Button")) {
            return Type::Button;
        } else if (0 == _stricmp(str.c_str(), "LeftTrigger")) {
            return Type::LeftTrigger;
        } else if (0 == _stricmp(str.c_str(), "RightTrigger")) {
            return Type::RightTrigger;
        } else if (0 == _stricmp(str.c_str(), "Axis")) {
            return Type::Axis;
        } else {
            return Type::Unknown;
        }
    }

    static AxisType toAxisType(std::string str)
    {
        if (0 == _stricmp(str.c_str(), "LeftX")) {
            return AxisType::LeftX;
        } else if (0 == _stricmp(str.c_str(), "LeftY")) {
            return AxisType::LeftY;
        } else if (0 == _stricmp(str.c_str(), "RightX")) {
            return AxisType::RightX;
        } else if (0 == _stricmp(str.c_str(), "RightY")) {
            return AxisType::RightY;
        } else {
            return AxisType::None;
        }
    }

    static KeyConfig* makeButton(unsigned char pad, int button)
    {
        return new KeyConfig(pad, button);
    }

    static KeyConfig* makeLeftTrigger(unsigned char pad)
    {
        return new KeyConfig(pad, Type::LeftTrigger);
    }

    static KeyConfig* makeRightTrigger(unsigned char pad)
    {
        return new KeyConfig(pad, Type::RightTrigger);
    }

    static KeyConfig* makeAxis(unsigned char pad, AxisType axisType, int compareValue)
    {
        return new KeyConfig(pad, axisType, compareValue < 0, compareValue);
    }

    KeyConfig(unsigned char pad, int buttonMask)
    {
        this->pad = pad;
        this->type = Type::Button;
        this->buttonMask = buttonMask;
        this->axisType = AxisType::None;
        this->isLessThan = false;
        this->compareValue = 0;
    }

    KeyConfig(unsigned char pad, Type type)
    {
        this->pad = pad;
        this->type = type;
        this->buttonMask = 0;
        this->axisType = AxisType::None;
        this->isLessThan = false;
        this->compareValue = 0;
    }

    KeyConfig(unsigned char pad, AxisType axisType, bool isLessThan, int compareValue)
    {
        this->pad = pad;
        this->type = Type::Axis;
        this->buttonMask = 0;
        this->axisType = axisType;
        this->isLessThan = isLessThan;
        this->compareValue = compareValue;
    }

    inline unsigned char check(XINPUT_STATE* st)
    {
        switch (this->type) {
            case Type::Button:
                return st->Gamepad.wButtons & this->buttonMask ? this->pad : 0;
            case Type::LeftTrigger:
                return st->Gamepad.bLeftTrigger ? this->pad : 0;
            case Type::RightTrigger:
                return st->Gamepad.bRightTrigger ? this->pad : 0;
            case Type::Axis: {
                short thumb = 0;
                switch (this->axisType) {
                    case AxisType::LeftX: thumb = st->Gamepad.sThumbLX; break;
                    case AxisType::LeftY: thumb = st->Gamepad.sThumbLY; break;
                    case AxisType::RightX: thumb = st->Gamepad.sThumbRX; break;
                    case AxisType::RightY: thumb = st->Gamepad.sThumbRY; break;
                }
                if (this->isLessThan) {
                    if (thumb < this->compareValue) {
                        return this->pad;
                    }
                } else {
                    if (thumb > this->compareValue) {
                        return this->pad;
                    }
                }
                break;
            }
        }
        return 0;
    }

    inline unsigned char check(DIJOYSTATE* st)
    {
        switch (this->type) {
            case Type::Button:
                return st->rgbButtons[this->buttonMask & 0x1F] ? this->pad : 0;
            case Type::Axis: {
                LONG thumb = 0;
                switch (this->axisType) {
                    case AxisType::LeftX: thumb = st->lX; break;
                    case AxisType::LeftY: thumb = st->lY; break;
                    case AxisType::RightX: thumb = st->lRx; break;
                    case AxisType::RightY: thumb = st->lRy; break;
                }
                if (this->isLessThan) {
                    if (thumb < this->compareValue) {
                        return this->pad;
                    }
                } else {
                    if (thumb > this->compareValue) {
                        return this->pad;
                    }
                }
                break;
            }
        }
        return 0;
    }

    inline unsigned char check(BYTE* st)
    {
        return this->type == Type::Button && st[this->buttonMask & 0xFF] ? this->pad : 0;
    }
};
