/**
 * VGS-Zero - DirectInput manager (keyboard only)
 * License under GPLv3: https://github.com/suzukiplan/vgszero/blob/master/LICENSE-VGS0.txt
 * (C)2023, SUZUKI PLAN
 */
#pragma once
#include <dinput.h>
#include <process.h>
#include <vector>

class InputManager
{
  private:
    void (*putlog)(const char*, ...);
    LPDIRECTINPUT8 lpDI;
    LPDIRECTINPUTDEVICE8 lpKeyboard;
    std::vector<DIDEVICEINSTANCE> diInstances;

  public:
    InputManager(void (*putlog)(const char*, ...))
    {
        this->putlog = putlog;
        lpDI = nullptr;
        lpKeyboard = nullptr;
    }

    ~InputManager()
    {
        release();
    }

    inline void getKeyStatus(BYTE* key)
    {
        memset(key, 0, 256);
        if (lpKeyboard) {
            HRESULT res = lpKeyboard->GetDeviceState(256, key);
            if (FAILED(res)) {
                lpKeyboard->Acquire();
                res = lpKeyboard->GetDeviceState((DWORD)256, key);
            }
        }
    }

    void bind(HINSTANCE hInst, HWND hWnd)
    {
        putlog("Binding DirectInput: instance=%p, window=%p", hInst, hWnd);
        this->release();
        HRESULT res = DirectInput8Create(hInst, DIRECTINPUT_VERSION, IID_IDirectInput8, (LPVOID*)&lpDI, nullptr);
        if (FAILED(res)) {
            putlog("DirectInput8Create failed! (%X)", res);
            lpDI = nullptr;
            lpKeyboard = nullptr;
            return;
        }

        // キーボード
        res = lpDI->CreateDevice(GUID_SysKeyboard, &lpKeyboard, nullptr);
        if (FAILED(res)) {
            putlog("CreateDevice(Keyboard) failed! (%X)", res);
        } else {
            res = lpKeyboard->SetDataFormat(&c_dfDIKeyboard);
            if (FAILED(res)) {
                putlog("SetDataFormat(Keyboard) failed! (%X)", res);
                lpKeyboard->Release();
                lpKeyboard = nullptr;
            } else {
                res = lpKeyboard->SetCooperativeLevel(hWnd, DISCL_FOREGROUND | DISCL_NONEXCLUSIVE | DISCL_NOWINKEY);
                if (FAILED(res)) {
                    putlog("SetCooperativeLevel(keyboard) failed! (%X)", res);
                    lpKeyboard->Release();
                    lpKeyboard = nullptr;
                } else {
                    lpKeyboard->Acquire();
                }
            }
        }
    }

  private:
    void release()
    {
        if (lpKeyboard) {
            lpKeyboard->Unacquire();
            lpKeyboard->Release();
            lpKeyboard = nullptr;
        }
        if (lpDI) {
            lpDI->Release();
            lpDI = nullptr;
        }
    }
};
