/**
 * VGS-Zero - SDK for Steam
 * License under GPLv3: https://github.com/suzukiplan/vgszero/blob/master/LICENSE-VGS0.txt
 * (C)2024, SUZUKI PLAN
 */
#define DIRECTINPUT_VERSION 0x0800

#include "picojson.h"
#include "resource.h"
#include "vdp.hpp"
#include "vgs0.hpp"

#include "inputmgr.hpp"
#include "keyconfig.hpp"

#include "steam.hpp"

#include <Windows.h>
#include <commctrl.h>
#include <d3d9.h>
#include <dinput.h>
#include <dsound.h>
#include <fstream>
#include <iostream>
#include <mmeapi.h>
#include <process.h>
#include <sstream>
#include <stdio.h>
#include <string>
#include <time.h>
#include <timeapi.h>
#include <vector>

#define MAX_LOADSTRING 100

#define SBUF_SIZE 8820
#define VRAM_WIDTH_TINY 240
#define VRAM_HEIGHT_TINY 192
#define VRAM_WIDTH_LOW 480
#define VRAM_HEIGHT_LOW 384
#define VRAM_WIDTH_HIGH 960
#define VRAM_HEIGHT_HIGH 768

#define SND_INIT 0
#define SND_READY 1
#define SND_EQ 254
#define SND_END 255

static void putlog(const char* msg, ...);
static int ginit(HWND hWnd);
static void gterm();
static void vtrans(int, int*);
static int init_sound();
static void term_sound();
static void sound_thread(void* arg);
static int ds_wait(BYTE wctrl);
static void lock();
static void unlock();
static void reset_keyboard_assign();
static void toggle_fullscreen();
ATOM MyRegisterClass(HINSTANCE hInstance);
BOOL InitInstance(HINSTANCE, int);
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK VirtualKeyboard(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam);
INT_PTR CALLBACK KeyConfigKeyboard(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK ScreenResolution(HWND, UINT, WPARAM, LPARAM);

enum class Resolution {
    High,
    Low,
    Tiny
};

static HINSTANCE hInst;
static bool _isFullScreen;
static bool _isAspectFit;
static Resolution _resolution;
static int _windowX;
static int _windowY;
static int _windowWidth;
static int _windowHeight;
static HWND hWnd;
static HMENU hMenu;
static BOOL isHEL = FALSE;
static LPDIRECT3D9 _lpD3D;
static LPDIRECT3DDEVICE9 _lpDev;
static LPDIRECT3DSURFACE9 _lpBuf;
static int need_restore = 0;
static LPDIRECTSOUND8 _lpDS = nullptr;
static LPDIRECTSOUNDBUFFER8 _lpSB = nullptr;
static LPDIRECTSOUNDNOTIFY8 _lpNtfy = nullptr;
static DSBPOSITIONNOTIFY _dspn;
static uintptr_t _uiSnd = -1;
static BYTE _SndCTRL = SND_INIT;
static CRITICAL_SECTION _lock;
static InputManager _im(putlog);
static std::vector<KeyConfig*> _kbConfig;
static VGS0 vgs0(VDP::ColorMode::RGB555);
static unsigned char _saveData[0x4000];
static size_t _saveSize = 0;
static CSteam* steam;

static void putlog(const char* msg, ...)
{
    FILE* fp;
    va_list args;
    time_t t1;
    struct tm* t2;

    if (NULL == (fp = fopen("log.txt", "a"))) {
        return;
    }
    t1 = time(NULL);
    t2 = localtime(&t1);
    fprintf(fp, "%04d/%02d/%02d %02d:%02d:%02d ", t2->tm_year + 1900, t2->tm_mon + 1, t2->tm_mday, t2->tm_hour, t2->tm_min, t2->tm_sec);
    va_start(args, msg);
    vfprintf(fp, msg, args);
    va_end(args);
    fprintf(fp, "\n");
    fclose(fp);
}

static void save_config()
{
    putlog("Saving config.json");
    picojson::value j;
    picojson::object o;
    picojson::object basic;

    basic.insert(std::make_pair("windowX", picojson::value((double)_windowX)));
    basic.insert(std::make_pair("windowY", picojson::value((double)_windowY)));
    basic.insert(std::make_pair("windowWidth", picojson::value((double)_windowWidth)));
    basic.insert(std::make_pair("windowHeight", picojson::value((double)_windowHeight)));
    basic.insert(std::make_pair("isFullScreen", picojson::value(_isFullScreen)));
    basic.insert(std::make_pair("isAspectFit", picojson::value(_isAspectFit)));
    switch (_resolution) {
        case Resolution::High:
            basic.insert(std::make_pair("resolution", picojson::value("high")));
            break;
        case Resolution::Low:
            basic.insert(std::make_pair("resolution", picojson::value("low")));
            break;
        case Resolution::Tiny:
            basic.insert(std::make_pair("resolution", picojson::value("tiny")));
            break;
    }
    o.insert(std::make_pair("basic_settings", basic));

    picojson::object keyConfig;
    picojson::array kb;
    for (auto cfg : _kbConfig) {
        picojson::object co;
        co.insert(std::make_pair("pad", picojson::value(KeyConfig::toString(cfg->pad))));
        co.insert(std::make_pair("type", picojson::value(KeyConfig::toString(cfg->type))));
        if (cfg->type == KeyConfig::Type::Button) {
            co.insert(std::make_pair("button", picojson::value((double)cfg->buttonMask)));
            kb.push_back(picojson::value(co));
        }
    }
    keyConfig.insert(std::make_pair("keyboard", kb));
    o.insert(std::make_pair("key_config", keyConfig));

    try {
        std::ofstream ofs("config.json");
        ofs << picojson::value(o).serialize(true) << std::endl;
    } catch (...) {
        putlog("Save failed!");
    }
}

static void extract_key_config(std::vector<KeyConfig*>& config, picojson::array& array)
{
    for (auto cfg : config) {
        delete cfg;
    }
    config.clear();
    for (const auto& cfg : array) {
        auto pad = KeyConfig::toPad(cfg.get("pad").get<std::string>());
        auto type = KeyConfig::toType(cfg.get("type").get<std::string>());
        KeyConfig* newConfig = nullptr;
        switch (type) {
            case KeyConfig::Type::Button:
                newConfig = KeyConfig::makeButton(pad, (int)cfg.get("button").get<double>());
                break;
            case KeyConfig::Type::LeftTrigger:
                newConfig = KeyConfig::makeLeftTrigger(pad);
                break;
            case KeyConfig::Type::RightTrigger:
                newConfig = KeyConfig::makeRightTrigger(pad);
                break;
            case KeyConfig::Type::Axis: {
                auto axisType = KeyConfig::toAxisType(cfg.get("axisType").get<std::string>());
                auto compare = (int)cfg.get("compare").get<double>();
                newConfig = KeyConfig::makeAxis(pad, axisType, compare);
                break;
            }
        }
        if (newConfig) {
            config.push_back(newConfig);
        }
    }
}

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                      _In_opt_ HINSTANCE hPrevInstance,
                      _In_ LPWSTR lpCmdLine,
                      _In_ int nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    DeleteFileA("log.txt");

    putlog("Loading config.json");
    reset_keyboard_assign();
    _windowX = CW_USEDEFAULT;
    _windowY = CW_USEDEFAULT;
    _windowWidth = VRAM_WIDTH_LOW;
    _windowHeight = VRAM_HEIGHT_LOW;
    _isFullScreen = false;
    _isAspectFit = true;
    _resolution = Resolution::High;
    std::ifstream ifs("config.json", std::ios::in);
    if (ifs.fail()) {
        putlog("File not found (use default settings)");
        save_config();
    } else {
        const std::string json((std::istreambuf_iterator<char>(ifs)), std::istreambuf_iterator<char>());
        ifs.close();
        picojson::value v;
        const std::string err = picojson::parse(v, json);
        if (err.empty() == false) {
            putlog(("Detected error: " + err).c_str());
        } else {
            auto obj = v.get<picojson::object>();
            auto basic = obj["basic_settings"].get<picojson::object>();
            if (basic.find("windowX")->second.is<double>()) {
                _windowX = (int)basic["windowX"].get<double>();
                if (_windowX < 1) {
                    _windowX = CW_USEDEFAULT;
                }
            }
            if (basic.find("windowY")->second.is<double>()) {
                _windowY = (int)basic["windowY"].get<double>();
                if (_windowY < 1) {
                    _windowY = CW_USEDEFAULT;
                }
            }
            if (basic.find("windowWidth")->second.is<double>()) {
                _windowWidth = (int)basic["windowWidth"].get<double>();
                if (_windowWidth < VRAM_WIDTH_TINY) {
                    _windowWidth = VRAM_WIDTH_TINY;
                }
            }
            if (basic.find("windowHeight")->second.is<double>()) {
                _windowHeight = (int)basic["windowHeight"].get<double>();
                if (_windowHeight < VRAM_HEIGHT_TINY) {
                    _windowHeight = VRAM_HEIGHT_TINY;
                }
            }
            if (basic.find("isFullScreen")->second.is<bool>()) {
                _isFullScreen = basic["isFullScreen"].get<bool>();
            }
            if (basic.find("isAspectFit")->second.is<bool>()) {
                _isAspectFit = basic["isAspectFit"].get<bool>();
            }
            if (basic.find("resolution")->second.is<std::string>()) {
                const char* res = basic["resolution"].get<std::string>().c_str();
                if (0 == strncmp(res, "low", 3)) {
                    _resolution = Resolution::Low;
                } else if (0 == strncmp(res, "tiny", 4)) {
                    _resolution = Resolution::Tiny;
                } else {
                    _resolution = Resolution::High;
                }
            }
            if (basic.find("key_config")->second.is<picojson::object>()) {
                auto keyConfig = obj["key_config"].get<picojson::object>();
                if (basic.find("keyboard")->second.is<picojson::array>()) {
                    picojson::array kb = keyConfig["keyboard"].get<picojson::array>();
                    if (0 < kb.size()) {
                        extract_key_config(_kbConfig, kb);
                    }
                }
            }
        }
    }

    // Steam の BigPicture モードの場合は強制的にフルスクリーンにする
    if (getenv("SteamTenfoot")) {
        putlog("Set force full screen (SteamTenfoot)");
        _isFullScreen = true;
    }

    steam = new CSteam(putlog);
    steam->init();

    putlog("Initializing Window...");
    MyRegisterClass(hInstance);
    if (!InitInstance(hInstance, nCmdShow)) {
        return FALSE;
    }

    putlog("Initializing VGS-Zero emulator...");
    const void* rom = nullptr;
    const void* bgm = nullptr;
    const void* se = nullptr;
    unsigned int romSize = 0;
    unsigned int bgmSize = 0;
    unsigned int seSize = 0;
    {
        HRSRC gamepkg = FindResource(0, MAKEINTRESOURCE(IDR_GAMEPKG), TEXT("BIN"));
        const BYTE* ptr = (const BYTE*)LockResource(LoadResource(0, gamepkg));
        ptr += 8;
        memcpy(&romSize, ptr, 4);
        ptr += 4;
        rom = ptr;
        ptr += romSize;
        memcpy(&bgmSize, ptr, 4);
        ptr += 4;
        bgm = 0 < bgmSize ? ptr : nullptr;
        ptr += bgmSize;
        memcpy(&seSize, ptr, 4);
        ptr += 4;
        se = 0 < seSize ? ptr : nullptr;
    }
    putlog("Loading ROM (%u banks = %u bytes)", romSize / 8192 + 1, romSize);
    vgs0.loadRom(rom, romSize);
    if (0 < bgmSize) {
        putlog("Loading BGM (%u bytes)", bgmSize);
        vgs0.loadBgm(bgm, bgmSize);
    }
    if (0 < seSize) {
        putlog("Loading SE (%u bytes)", seSize);
        vgs0.loadSoundEffect(se, seSize);
    }

    vgs0.saveCallback = [](VGS0* vgs0, const void* data, size_t size) -> bool {
        FILE* fp = fopen("save.dat", "wb");
        if (!fp)
            return false;
        bool result = size == fwrite(data, 1, size, fp);
        fclose(fp);
        return result;
    };
    vgs0.loadCallback = [](VGS0* vgs0, void* data, size_t size) -> bool {
        FILE* fp = fopen("save.dat", "rb");
        if (!fp)
            return false;
        bool result = size = fread(data, 1, size, fp);
        fclose(fp);
        memcpy(_saveData, data, size & 0x3FFF);
        return result;
    };

    _im.bind(hInst, hWnd);

    init_sound();

    need_restore = 0;
    DWORD wt[3] = {17, 17, 16};
    int wi = 0;

    putlog("Start mainloop");
    MSG msg;
    HRESULT res;
    RECT vramRect = {0, 0, 0, 0};
    switch (_resolution) {
        case Resolution::High:
            vramRect.right = VRAM_WIDTH_HIGH;
            vramRect.bottom = VRAM_HEIGHT_HIGH;
            break;
        case Resolution::Low:
            vramRect.right = VRAM_WIDTH_LOW;
            vramRect.bottom = VRAM_HEIGHT_LOW;
            break;
        case Resolution::Tiny:
            vramRect.right = VRAM_WIDTH_TINY;
            vramRect.bottom = VRAM_HEIGHT_TINY;
            break;
    }
    D3DLOCKED_RECT lrect;
    BYTE key[256];
    auto haccel = LoadAccelerators(hInst, MAKEINTRESOURCE(IDR_ACCELERATOR));

    int vramWidth, vramHeight;
    switch (_resolution) {
        case Resolution::High:
            vramWidth = VRAM_WIDTH_HIGH;
            vramHeight = VRAM_HEIGHT_HIGH;
            break;
        case Resolution::Low:
            vramWidth = VRAM_WIDTH_LOW;
            vramHeight = VRAM_HEIGHT_LOW;
            break;
        case Resolution::Tiny:
            vramWidth = VRAM_WIDTH_TINY;
            vramHeight = VRAM_HEIGHT_TINY;
            break;
    }

    int loopCounter = 0;
    bool connected = false;
    bool previousConnected = false;
    while (TRUE) {
        DWORD t1 = timeGetTime();
        loopCounter++;
        loopCounter &= 0x7FFFFFFF;
        if (PeekMessage(&msg, 0, 0, 0, PM_REMOVE)) {
            if (msg.message == WM_QUIT) {
                putlog("Received WM_QUIT");
                DestroyWindow(hWnd);
                break;
            }
            if (!TranslateAccelerator(hWnd, haccel, &msg)) {
                TranslateMessage(&msg);
                DispatchMessage(&msg);
            }
        }
        if (loopCounter % 6 == 0) {
            SteamAPI_RunCallbacks();
        }
        if (need_restore) {
            putlog("Detected need restart message.");
            break;
        }
        if (FAILED(res = _lpDev->BeginScene())) {
            putlog("BeginScene failed (%X)", res);
            need_restore = 1;
            continue;
        }
        if (FAILED(res = _lpBuf->LockRect(&lrect, &vramRect, D3DLOCK_DISCARD))) {
            putlog("LockRect failed (%X)", res);
            need_restore = 1;
            continue;
        }

        unsigned char pad = 0;

        // SteamInput
        pad = steam->getJoypad(&connected);
        if (connected && !previousConnected) {
            putlog("Gamepad connected");
        } else if (!connected && previousConnected) {
            MessageBoxA(hWnd, "Check that the gamepad is properly connected.", "Gamepad Disconnected!", MB_OK);
        }
        previousConnected = connected;

        // DirectInput (keyboard)
        _im.getKeyStatus(key);
        for (auto cfg : _kbConfig) {
            pad |= cfg->check(key);
        }

        if (!_isFullScreen && 0 != pad) {
            ShowCursor(FALSE);
        }

        if (!steam->isOverlay()) {
            lock();
            vgs0.tick(pad);
            unlock();
        }

        if (vgs0.cpu->reg.IFF & 0x80) {
            putlog("Detected HALT");
            break;
        }
        vtrans(lrect.Pitch, (int*)lrect.pBits);
        if (FAILED(res = _lpBuf->UnlockRect())) {
            putlog("UnlockRect failed (%X)", res);
            need_restore = 1;
            continue;
        }
        if (FAILED(res = _lpDev->EndScene())) {
            putlog("EndScene failed (%X)", res);
            need_restore = 1;
            continue;
        }

        // make aspect fit in the window
        if (_isAspectFit) {
            RECT windowRect;
            GetClientRect(hWnd, &windowRect);
            int windowWidth = windowRect.right - windowRect.left;
            int windowHeight = windowRect.bottom - windowRect.top;
            double scaleW = windowWidth;
            scaleW /= vramWidth;
            double scaleH = windowHeight;
            scaleH /= vramHeight;
            double scale = scaleW < scaleH ? scaleW : scaleH;
            int width = (int)(vramWidth * scale);
            int height = (int)(vramHeight * scale);
            windowRect.left = (windowWidth - width) / 2;
            windowRect.right = windowRect.left + width;
            windowRect.top = (windowHeight - height) / 2;
            windowRect.bottom = windowRect.top + height;
            res = _lpDev->Present(&vramRect, &windowRect, hWnd, nullptr);
        } else {
            res = _lpDev->Present(&vramRect, nullptr, hWnd, nullptr);
        }
        if (FAILED(res)) {
            putlog("Present failed (%X)", res);
            need_restore = 1;
            continue;
        }
        if (isHEL) {
            DWORD t2 = timeGetTime() - t1;
            if (t2 < wt[wi]) {
                Sleep(wt[wi] - t2);
            }
            wi++;
            wi %= 3;
        }
    }

    save_config();
    term_sound();
    gterm();
    delete steam;
    putlog("The all of resources are released");

    return (int)msg.wParam;
}

ATOM MyRegisterClass(HINSTANCE hInstance)
{
    /* disable IME */
    if (!ImmDisableIME(GetCurrentThreadId())) {
        putlog("ImmDisableIME error. (%d)", GetLastError());
    }

    /* regist window class */
    WNDCLASS wndclass;
    memset(&wndclass, 0, sizeof(wndclass));
    wndclass.hCursor = LoadCursor(NULL, IDC_ARROW);
    wndclass.hIcon = LoadIcon(hInst, MAKEINTRESOURCE(IDI_MAIN));
    wndclass.lpszMenuName = NULL;
    wndclass.lpszClassName = "VGSZero";
    wndclass.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
    wndclass.hInstance = hInst;
    wndclass.style = CS_BYTEALIGNCLIENT | CS_VREDRAW | CS_HREDRAW;
    wndclass.lpfnWndProc = (WNDPROC)WndProc;
    wndclass.cbClsExtra = 0;
    wndclass.cbWndExtra = 0;
    wndclass.lpszMenuName = MAKEINTRESOURCE(IDC_VGSZERO);
    if (!RegisterClass(&wndclass)) {
        putlog("RegisterClass error. (%d)", GetLastError());
        return FALSE;
    }
    return TRUE;
}

BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
    hInst = hInstance; // グローバル変数にインスタンス ハンドルを格納する
    auto style = WS_OVERLAPPEDWINDOW;
    hWnd = CreateWindowA(
        "VGSZero",
        "VGS-Zero",
        style,
        _windowX,
        _windowY,
        _windowWidth,
        _windowHeight,
        nullptr,
        nullptr,
        hInstance,
        nullptr);

    if (!hWnd) {
        putlog("CreateWindow failed (%X)", GetLastError());
        return FALSE;
    }

    hMenu = GetMenu(hWnd);
    ValidateRect(hWnd, 0);
    ShowWindow(hWnd, nCmdShow);
    UpdateWindow(hWnd);
    if (ginit(hWnd)) {
        putlog("Direct3D initialization failed (%X)", GetLastError());
        return FALSE;
    }
    if (_isFullScreen) {
        _isFullScreen = !_isFullScreen;
        toggle_fullscreen();
    }
    return TRUE;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message) {
        case WM_COMMAND: {
            int wmId = LOWORD(wParam);
            switch (wmId) {
                case IDM_RESET:
                    vgs0.reset();
                    break;
                case IDM_EXIT:
                    DestroyWindow(hWnd);
                    break;
                case IDM_FULLSCREEN:
                    toggle_fullscreen();
                    break;
                case IDM_ASPECTMODE:
                    _isAspectFit = !_isAspectFit;
                    _lpDev->ColorFill(_lpBuf, nullptr, 0);
                    _lpDev->Present(nullptr, nullptr, hWnd, nullptr);
                    break;
                case IDM_KEYBOARD:
                    if (_isFullScreen) {
                        ShowCursor(TRUE);
                    }
                    DialogBox(hInst, MAKEINTRESOURCE(IDD_DIALOG_KEYCONFIG), hWnd, KeyConfigKeyboard);
                    if (_isFullScreen) {
                        ShowCursor(FALSE);
                    }
                    break;
                case IDM_SCREEN_RESOLUTION:
                    if (_isFullScreen) {
                        ShowCursor(TRUE);
                    }
                    DialogBox(hInst, MAKEINTRESOURCE(IDD_DIALOG_RESOLUTION), hWnd, ScreenResolution);
                    if (_isFullScreen) {
                        ShowCursor(FALSE);
                    }
                    break;
                default:
                    return DefWindowProc(hWnd, message, wParam, lParam);
            }
            break;
        }
        case WM_MOVE:
            if (!_isFullScreen) {
                RECT rect;
                GetWindowRect(hWnd, &rect);
                _windowX = rect.left;
                _windowY = rect.top;
            }
            return DefWindowProc(hWnd, message, wParam, lParam);
        case WM_SIZE:
            if (SIZE_RESTORED == wParam && !_isFullScreen) {
                RECT rect;
                GetWindowRect(hWnd, &rect);
                _windowWidth = rect.right - rect.left;
                _windowHeight = rect.bottom - rect.top;
            }
            return DefWindowProc(hWnd, message, wParam, lParam);
        case WM_DESTROY:
            PostQuitMessage(0);
            break;
        case WM_SYSKEYDOWN:
            if (wParam == VK_RETURN && (lParam & 0x60000000) == 0x20000000) {
                toggle_fullscreen();
            } else {
                return DefWindowProc(hWnd, message, wParam, lParam);
            }
            break;
        case WM_MOUSEMOVE:
            if (!_isFullScreen) {
                ShowCursor(TRUE);
            }
            return DefWindowProc(hWnd, message, wParam, lParam);
        default:
            return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}

static std::string get_keyboard_string(unsigned char pad)
{
    static const char* dik[256] = {
        "(n/a)", "DIK_ESCAPE", "DIK_1", "DIK_2", "DIK_3", "DIK_4", "DIK_5", "DIK_6",
        "DIK_7", "DIK_8", "DIK_9", "DIK_0", "DIK_MINUS", "DIK_EQUALS", "DIK_BACK", "DIK_TAB",
        "DIK_Q", "DIK_W", "DIK_E", "DIK_R", "DIK_T", "DIK_Y", "DIK_U", "DIK_I",
        "DIK_O", "DIK_P", "DIK_LBRACKET", "DIK_RBRACKET", "DIK_RETURN", "DIK_LCONTROL", "DIK_A", "DIK_S",
        "DIK_D", "DIK_F", "DIK_G", "DIK_H", "DIK_J", "DIK_K", "DIK_L", "DIK_SEMICOLON",
        "DIK_APOSTROPHE", "DIK_GRAVE", "DIK_LSHIFT", "DIK_BACKSLASH", "DIK_Z", "DIK_X", "DIK_C", "DIK_V",
        "DIK_B", "DIK_N", "DIK_M", "DIK_COMMA", "DIK_PERIOD", "DIK_SLASH", "DIK_RSHIFT", "DIK_MULTIPLY",
        "DIK_LMENU", "DIK_SPACE", "DIK_CAPITAL", "DIK_F1", "DIK_F2", "DIK_F3", "DIK_F4", "DIK_F5",
        "DIK_F6", "DIK_F7", "DIK_F8", "DIK_F9", "DIK_F10", "DIK_NUMLOCK", "DIK_SCROLL", "DIK_NUMPAD7",
        "DIK_NUMPAD8", "DIK_NUMPAD9", "DIK_SUBTRACT", "DIK_NUMPAD4", "DIK_NUMPAD5", "DIK_NUMPAD6", "DIK_ADD", "DIK_NUMPAD1",
        "DIK_NUMPAD2", "DIK_NUMPAD3", "DIK_NUMPAD0", "DIK_DECIMAL", "(n/a)", "(n/a)", "DIK_OEM_102", "DIK_F11",
        "DIK_F12", "(n/a)", "(n/a)", "(n/a)", "(n/a)", "(n/a)", "(n/a)", "(n/a)",
        "(n/a)", "(n/a)", "(n/a)", "(n/a)", "DIK_F13", "DIK_F14", "DIK_F15", "(n/a)",
        "(n/a)", "(n/a)", "(n/a)", "(n/a)", "(n/a)", "(n/a)", "(n/a)", "(n/a)",
        "DIK_KANA", "(n/a)", "(n/a)", "DIK_ABNT_C1", "(n/a)", "(n/a)", "(n/a)", "(n/a)",
        "(n/a)", "DIK_CONVERT", "(n/a)", "DIK_NOCONVERT", "(n/a)", "DIK_YEN", "DIK_ABNT_C2", "(n/a)",
        "(n/a)", "(n/a)", "(n/a)", "(n/a)", "(n/a)", "(n/a)", "(n/a)", "(n/a)",
        "(n/a)", "(n/a)", "(n/a)", "(n/a)", "(n/a)", "DIK_NUMPADEQUALS", "(n/a)", "(n/a)",
        "DIK_PREVTRACK", "DIK_AT", "DIK_COLON", "DIK_UNDERLINE", "DIK_KANJI", "DIK_STOP", "DIK_AX", "DIK_UNLABELED",
        "(n/a)", "DIK_NEXTTRACK", "(n/a)", "(n/a)", "DIK_NUMPADENTER", "DIK_RCONTROL", "(n/a)", "(n/a)",
        "DIK_MUTE", "DIK_CALCULATOR", "DIK_PLAYPAUSE", "(n/a)", "DIK_MEDIASTOP", "(n/a)", "(n/a)", "(n/a)",
        "(n/a)", "(n/a)", "(n/a)", "(n/a)", "(n/a)", "(n/a)", "DIK_VOLUMEDOWN", "(n/a)",
        "DIK_VOLUMEUP", "(n/a)", "DIK_WEBHOME", "DIK_NUMPADCOMMA", "(n/a)", "DIK_DIVIDE", "(n/a)", "DIK_SYSRQ",
        "DIK_RMENU", "(n/a)", "(n/a)", "(n/a)", "(n/a)", "(n/a)", "(n/a)", "(n/a)",
        "(n/a)", "(n/a)", "(n/a)", "(n/a)", "(n/a)", "DIK_PAUSE", "(n/a)", "DIK_HOME",
        "DIK_UP", "DIK_PRIOR", "(n/a)", "DIK_LEFT", "(n/a)", "DIK_RIGHT", "(n/a)", "DIK_END",
        "DIK_DOWN", "DIK_NEXT", "DIK_INSERT", "DIK_DELETE", "(n/a)", "(n/a)", "(n/a)", "(n/a)",
        "(n/a)", "(n/a)", "(n/a)", "DIK_LWIN", "DIK_RWIN", "DIK_APPS", "DIK_POWER", "DIK_SLEEP",
        "(n/a)", "(n/a)", "(n/a)", "DIK_WAKE", "(n/a)", "DIK_WEBSEARCH", "DIK_WEBFAVORITES", "DIK_WEBREFRESH",
        "DIK_WEBSTOP", "DIK_WEBFORWARD", "DIK_WEBBACK", "DIK_MYCOMPUTER", "DIK_MAIL", "DIK_MEDIASELECT", "(n/a)", "(n/a)",
        "(n/a)", "(n/a)", "(n/a)", "(n/a)", "(n/a)", "(n/a)", "(n/a)", "(n/a)",
        "(n/a)", "(n/a)", "(n/a)", "(n/a)", "(n/a)", "(n/a)", "(n/a)", "(n/a)"};
    for (auto cfg : _kbConfig) {
        if (cfg->pad == pad && KeyConfig::Type::Button == cfg->type) {
            return dik[cfg->buttonMask & 0xFF];
        }
    }
    return "n/a";
}

static void assignNewKeyConfig(HWND hDlg, unsigned char assign_pad)
{
    int assign_key = 0;
    switch (DialogBox(hInst, MAKEINTRESOURCE(IDD_VIRTUAL_KEY), hDlg, VirtualKeyboard)) {
        case IDC_KEY_0:
            assign_key = DIK_0;
            break;
        case IDC_KEY_1:
            assign_key = DIK_1;
            break;
        case IDC_KEY_2:
            assign_key = DIK_2;
            break;
        case IDC_KEY_3:
            assign_key = DIK_3;
            break;
        case IDC_KEY_4:
            assign_key = DIK_4;
            break;
        case IDC_KEY_5:
            assign_key = DIK_5;
            break;
        case IDC_KEY_6:
            assign_key = DIK_6;
            break;
        case IDC_KEY_7:
            assign_key = DIK_7;
            break;
        case IDC_KEY_8:
            assign_key = DIK_8;
            break;
        case IDC_KEY_9:
            assign_key = DIK_9;
            break;
        case IDC_KEY_A:
            assign_key = DIK_A;
            break;
        case IDC_KEY_B:
            assign_key = DIK_B;
            break;
        case IDC_KEY_C:
            assign_key = DIK_C;
            break;
        case IDC_KEY_D:
            assign_key = DIK_D;
            break;
        case IDC_KEY_E:
            assign_key = DIK_E;
            break;
        case IDC_KEY_F:
            assign_key = DIK_F;
            break;
        case IDC_KEY_G:
            assign_key = DIK_G;
            break;
        case IDC_KEY_H:
            assign_key = DIK_H;
            break;
        case IDC_KEY_I:
            assign_key = DIK_I;
            break;
        case IDC_KEY_J:
            assign_key = DIK_J;
            break;
        case IDC_KEY_K:
            assign_key = DIK_K;
            break;
        case IDC_KEY_L:
            assign_key = DIK_L;
            break;
        case IDC_KEY_M:
            assign_key = DIK_M;
            break;
        case IDC_KEY_N:
            assign_key = DIK_N;
            break;
        case IDC_KEY_O:
            assign_key = DIK_O;
            break;
        case IDC_KEY_P:
            assign_key = DIK_P;
            break;
        case IDC_KEY_Q:
            assign_key = DIK_Q;
            break;
        case IDC_KEY_R:
            assign_key = DIK_R;
            break;
        case IDC_KEY_S:
            assign_key = DIK_S;
            break;
        case IDC_KEY_T:
            assign_key = DIK_T;
            break;
        case IDC_KEY_U:
            assign_key = DIK_U;
            break;
        case IDC_KEY_V:
            assign_key = DIK_V;
            break;
        case IDC_KEY_W:
            assign_key = DIK_W;
            break;
        case IDC_KEY_X:
            assign_key = DIK_X;
            break;
        case IDC_KEY_Y:
            assign_key = DIK_Y;
            break;
        case IDC_KEY_Z:
            assign_key = DIK_Z;
            break;
        case IDC_KEY_MINUS:
            assign_key = DIK_MINUS;
            break;
        case IDC_KEY_EOR:
            assign_key = DIK_CIRCUMFLEX;
            break;
        case IDC_KEY_YEN:
            assign_key = DIK_YEN;
            break;
        case IDC_KEY_BACK:
            assign_key = DIK_BACKSPACE;
            break;
        case IDC_KEY_ESC:
            assign_key = DIK_ESCAPE;
            break;
        case IDC_KEY_TAB:
            assign_key = DIK_TAB;
            break;
        case IDC_KEY_AT:
            assign_key = DIK_AT;
            break;
        case IDC_KEY_KL:
            assign_key = DIK_LBRACKET;
            break;
        case IDC_KEY_CAPS:
            assign_key = DIK_CAPSLOCK;
            break;
        case IDC_KEY_SEMICOLON:
            assign_key = DIK_SEMICOLON;
            break;
        case IDC_KEY_COLON:
            assign_key = DIK_COLON;
            break;
        case IDC_KEY_KR:
            assign_key = DIK_RBRACKET;
            break;
        case IDC_KEY_LSHIFT:
            assign_key = DIK_LSHIFT;
            break;
        case IDC_KEY_COMMA:
            assign_key = DIK_COMMA;
            break;
        case IDC_KEY_PERIOD:
            assign_key = DIK_PERIOD;
            break;
        case IDC_KEY_SLASH:
            assign_key = DIK_SLASH;
            break;
        case IDC_KEY_UNDER:
            assign_key = DIK_BACKSLASH;
            break;
        case IDC_KEY_RSHIFT:
            assign_key = DIK_RSHIFT;
            break;
        case IDC_KEY_ENTER:
            assign_key = DIK_RETURN;
            break;
        case IDC_KEY_SPACE:
            assign_key = DIK_SPACE;
            break;
        case IDC_KEY_LEFT:
            assign_key = DIK_LEFT;
            break;
        case IDC_KEY_UP:
            assign_key = DIK_UP;
            break;
        case IDC_KEY_DOWN:
            assign_key = DIK_DOWN;
            break;
        case IDC_KEY_RIGHT:
            assign_key = DIK_RIGHT;
            break;
        default:
            return;
    }
    KeyConfig* newConfig = KeyConfig::makeButton(assign_pad, assign_key);
    KeyConfig* oldConfig;
    do {
        oldConfig = nullptr;
        for (auto it = _kbConfig.begin(); it != _kbConfig.end(); it++) {
            if ((*it)->pad == assign_pad) {
                oldConfig = *it;
                _kbConfig.erase(it);
                break;
            }
        }
        if (oldConfig) {
            delete oldConfig;
        }
    } while (oldConfig);
    _kbConfig.push_back(newConfig);

    int idc = 0;
    switch (assign_pad) {
        case VGS0_JOYPAD_T1:
            idc = IDC_BUTTON_A;
            break;
        case VGS0_JOYPAD_T2:
            idc = IDC_BUTTON_B;
            break;
        case VGS0_JOYPAD_ST:
            idc = IDC_BUTTON_START;
            break;
        case VGS0_JOYPAD_SE:
            idc = IDC_BUTTON_SELECT;
            break;
        case VGS0_JOYPAD_UP:
            idc = IDC_BUTTON_UP;
            break;
        case VGS0_JOYPAD_DW:
            idc = IDC_BUTTON_DOWN;
            break;
        case VGS0_JOYPAD_LE:
            idc = IDC_BUTTON_LEFT;
            break;
        case VGS0_JOYPAD_RI:
            idc = IDC_BUTTON_RIGHT;
            break;
    }
    if (idc) {
        SetDlgItemTextA(hDlg, idc, get_keyboard_string(assign_pad).c_str());
    }
}

INT_PTR CALLBACK VirtualKeyboard(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message) {
        case WM_INITDIALOG:
            return (INT_PTR)TRUE;
        case WM_COMMAND:
            EndDialog(hDlg, LOWORD(wParam));
            return (INT_PTR)TRUE;
    }
    return (INT_PTR)FALSE;
}

INT_PTR CALLBACK KeyConfigKeyboard(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    UNREFERENCED_PARAMETER(lParam);

    switch (message) {
        case WM_INITDIALOG:
            SetDlgItemTextA(hDlg, IDC_BUTTON_A, get_keyboard_string(VGS0_JOYPAD_T1).c_str());
            SetDlgItemTextA(hDlg, IDC_BUTTON_B, get_keyboard_string(VGS0_JOYPAD_T2).c_str());
            SetDlgItemTextA(hDlg, IDC_BUTTON_START, get_keyboard_string(VGS0_JOYPAD_ST).c_str());
            SetDlgItemTextA(hDlg, IDC_BUTTON_SELECT, get_keyboard_string(VGS0_JOYPAD_SE).c_str());
            SetDlgItemTextA(hDlg, IDC_BUTTON_UP, get_keyboard_string(VGS0_JOYPAD_UP).c_str());
            SetDlgItemTextA(hDlg, IDC_BUTTON_DOWN, get_keyboard_string(VGS0_JOYPAD_DW).c_str());
            SetDlgItemTextA(hDlg, IDC_BUTTON_LEFT, get_keyboard_string(VGS0_JOYPAD_LE).c_str());
            SetDlgItemTextA(hDlg, IDC_BUTTON_RIGHT, get_keyboard_string(VGS0_JOYPAD_RI).c_str());
            return (INT_PTR)TRUE;

        case WM_COMMAND:
            putlog("WM_COMMAND wParam=%d, lParam=%d", wParam, lParam);
            switch (wParam) {
                case IDOK:
                    EndDialog(hDlg, LOWORD(wParam));
                    return (INT_PTR)TRUE;
                case IDCANCEL:
                    reset_keyboard_assign();
                    SetDlgItemTextA(hDlg, IDC_BUTTON_A, get_keyboard_string(VGS0_JOYPAD_T1).c_str());
                    SetDlgItemTextA(hDlg, IDC_BUTTON_B, get_keyboard_string(VGS0_JOYPAD_T2).c_str());
                    SetDlgItemTextA(hDlg, IDC_BUTTON_START, get_keyboard_string(VGS0_JOYPAD_ST).c_str());
                    SetDlgItemTextA(hDlg, IDC_BUTTON_SELECT, get_keyboard_string(VGS0_JOYPAD_SE).c_str());
                    SetDlgItemTextA(hDlg, IDC_BUTTON_UP, get_keyboard_string(VGS0_JOYPAD_UP).c_str());
                    SetDlgItemTextA(hDlg, IDC_BUTTON_DOWN, get_keyboard_string(VGS0_JOYPAD_DW).c_str());
                    SetDlgItemTextA(hDlg, IDC_BUTTON_LEFT, get_keyboard_string(VGS0_JOYPAD_LE).c_str());
                    SetDlgItemTextA(hDlg, IDC_BUTTON_RIGHT, get_keyboard_string(VGS0_JOYPAD_RI).c_str());
                    break;
                case IDC_BUTTON_A:
                    assignNewKeyConfig(hDlg, VGS0_JOYPAD_T1);
                    break;
                case IDC_BUTTON_B:
                    assignNewKeyConfig(hDlg, VGS0_JOYPAD_T2);
                    break;
                case IDC_BUTTON_START:
                    assignNewKeyConfig(hDlg, VGS0_JOYPAD_ST);
                    break;
                case IDC_BUTTON_SELECT:
                    assignNewKeyConfig(hDlg, VGS0_JOYPAD_SE);
                    break;
                case IDC_BUTTON_UP:
                    assignNewKeyConfig(hDlg, VGS0_JOYPAD_UP);
                    break;
                case IDC_BUTTON_DOWN:
                    assignNewKeyConfig(hDlg, VGS0_JOYPAD_DW);
                    break;
                case IDC_BUTTON_LEFT:
                    assignNewKeyConfig(hDlg, VGS0_JOYPAD_LE);
                    break;
                case IDC_BUTTON_RIGHT:
                    assignNewKeyConfig(hDlg, VGS0_JOYPAD_RI);
                    break;
            }
            break;
        case WM_CLOSE:
            EndDialog(hDlg, LOWORD(wParam));
            return (INT_PTR)TRUE;
    }
    return (INT_PTR)FALSE;
}

INT_PTR CALLBACK ScreenResolution(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    static Resolution selected;
    switch (message) {
        case WM_INITDIALOG:
            selected = _resolution;
            switch (_resolution) {
                case Resolution::High:
                    SendDlgItemMessage(hDlg, IDC_RADIO_RES_HIGH, BM_SETCHECK, BST_CHECKED, 0);
                    break;
                case Resolution::Low:
                    SendDlgItemMessage(hDlg, IDC_RADIO_RES_LOW, BM_SETCHECK, BST_CHECKED, 0);
                    break;
                case Resolution::Tiny:
                    SendDlgItemMessage(hDlg, IDC_RADIO_RES_TINY, BM_SETCHECK, BST_CHECKED, 0);
                    break;
            }
            EnableWindow(GetDlgItem(hDlg, IDOK), FALSE);
            return (INT_PTR)TRUE;
        case WM_COMMAND:
            if (LOWORD(wParam) == IDCANCEL) {
                EndDialog(hDlg, LOWORD(wParam));
                return (INT_PTR)TRUE;
            } else if (LOWORD(wParam) == IDC_RADIO_RES_HIGH || LOWORD(wParam) == IDC_RADIO_RES_LOW || LOWORD(wParam) == IDC_RADIO_RES_TINY) {
                if (BST_CHECKED == SendMessage(GetDlgItem(hDlg, IDC_RADIO_RES_HIGH), BM_GETCHECK, 0, 0)) {
                    selected = Resolution::High;
                } else if (BST_CHECKED == SendMessage(GetDlgItem(hDlg, IDC_RADIO_RES_LOW), BM_GETCHECK, 0, 0)) {
                    selected = Resolution::Low;
                } else {
                    selected = Resolution::Tiny;
                }
                EnableWindow(GetDlgItem(hDlg, IDOK), _resolution != selected);
            } else if (LOWORD(wParam) == IDOK) {
                _resolution = selected;
                need_restore = 1;
                EndDialog(hDlg, LOWORD(wParam));
                return (INT_PTR)TRUE;
            }
            break;
        case WM_CLOSE:
            EndDialog(hDlg, LOWORD(wParam));
            return (INT_PTR)TRUE;
    }
    return (INT_PTR)FALSE;
}

static int ginit(HWND hWnd)
{
    D3DDISPLAYMODE dm;
    D3DPRESENT_PARAMETERS dprm;
    HRESULT res;
    putlog("Initializing Direct3D...");

    if (NULL == (_lpD3D = Direct3DCreate9(D3D_SDK_VERSION))) {
        MessageBoxA(hWnd, "Direct3DCreate9 failed!", "ERROR", MB_OK | MB_ICONERROR);
        return -1;
    }

    _lpD3D->GetAdapterDisplayMode(D3DADAPTER_DEFAULT, &dm);
    putlog("Adapter display mode: Format=0x%X, Width=%d, Height=%d, RefreshRate=%dHz", dm.Format, dm.Width, dm.Height, dm.RefreshRate);

    memset(&dprm, 0, sizeof(dprm));
    dprm.Windowed = TRUE;
    dprm.FullScreen_RefreshRateInHz = 0;
    switch (_resolution) {
        case Resolution::High:
            dprm.BackBufferWidth = VRAM_WIDTH_HIGH;
            dprm.BackBufferHeight = VRAM_HEIGHT_HIGH;
            break;
        case Resolution::Low:
            dprm.BackBufferWidth = VRAM_WIDTH_LOW;
            dprm.BackBufferHeight = VRAM_HEIGHT_LOW;
            break;
        case Resolution::Tiny:
            dprm.BackBufferWidth = VRAM_WIDTH_TINY;
            dprm.BackBufferHeight = VRAM_HEIGHT_TINY;
            break;
    }
    dprm.BackBufferCount = 1;
    dprm.SwapEffect = D3DSWAPEFFECT_COPY;
    dprm.BackBufferFormat = dm.Format;
    dprm.EnableAutoDepthStencil = TRUE;
    dprm.AutoDepthStencilFormat = D3DFMT_D16;
    dprm.PresentationInterval = D3DPRESENT_INTERVAL_DEFAULT;
    dprm.Flags = D3DPRESENTFLAG_LOCKABLE_BACKBUFFER;

    do {
        res = _lpD3D->CreateDevice(
            D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, hWnd, D3DCREATE_SOFTWARE_VERTEXPROCESSING, &dprm, &_lpDev);
        if (!FAILED(res)) {
            putlog("VERTEX PROCESSING: using software mode");
            isHEL = FALSE;
            break;
        }
        res = _lpD3D->CreateDevice(
            D3DADAPTER_DEFAULT, D3DDEVTYPE_REF, hWnd, D3DCREATE_HARDWARE_VERTEXPROCESSING, &dprm, &_lpDev);
        if (!FAILED(res)) {
            putlog("VERTEX PROCESSING: using hardware mode");
            isHEL = TRUE;
            break;
        }
    } while (0);

    if (FAILED(res)) {
        MessageBoxA(hWnd, "CreateDevice failed!", "ERROR", MB_OK | MB_ICONERROR);
        return -1;
    }

    res = _lpDev->GetBackBuffer(0, 0, D3DBACKBUFFER_TYPE_MONO, &_lpBuf);
    if (FAILED(res)) {
        MessageBoxA(hWnd, "GetBackBuffer failed!", "ERROR", MB_OK | MB_ICONERROR);
        return -1;
    }

    return 0;
}

static void gterm()
{
    if (_lpDev) {
        _lpDev->Release();
        _lpDev = nullptr;
    }
    if (_lpD3D) {
        _lpD3D->Release();
        _lpD3D = nullptr;
    }
}

static inline unsigned char bit5To8(unsigned char bit5)
{
    bit5 <<= 3;
    bit5 |= (bit5 & 0b11100000) >> 5;
    return bit5;
}

static inline int rgb555_to_888(unsigned short rgb555)
{
    int result = 0;
    result += bit5To8((rgb555 & 0x7C00) >> 10);
    result <<= 8;
    result += bit5To8((rgb555 & 0x03E0) >> 5);
    result <<= 8;
    result += bit5To8(rgb555 & 0x001F);
    result |= 0xFF000000;
    return result;
}

static void vtrans(int pitch, int* ptr)
{
    register short vx, vy;
    register int p;
    pitch >>= 2;

    p = 0;
    unsigned short* display = vgs0.getDisplay();
    switch (_resolution) {
        case Resolution::High:
            for (vy = 0; vy < VRAM_HEIGHT_HIGH; vy += 4) {
                for (vx = 0; vx < VRAM_WIDTH_HIGH; vx += 4) {
                    auto offset = vy * pitch + vx;
                    ptr[offset] = rgb555_to_888(*display);
                    ptr[offset + 1] = ptr[offset];
                    ptr[offset + 2] = ptr[offset] & 0xFFF0F0F0;
                    ptr[offset + 3] = ptr[offset + 2];
                    ptr[offset + pitch * 2] = ptr[offset] & 0x8F8F8F8F;
                    ptr[offset + pitch * 2 + 1] = ptr[offset] & 0x8F8F8F8F;
                    ptr[offset + pitch * 2 + 2] = ptr[offset] & 0x80808080;
                    ptr[offset + pitch * 2 + 3] = ptr[offset] & 0x80808080;
                    display++;
                }
                auto offset = vy * pitch;
                memcpy(&ptr[offset + pitch], &ptr[offset], pitch * 4);
                memcpy(&ptr[offset + pitch * 3], &ptr[offset + pitch * 2], pitch * 4);
            }
            break;
        case Resolution::Low:
            for (vy = 0; vy < VRAM_HEIGHT_LOW; vy += 2) {
                for (vx = 0; vx < VRAM_WIDTH_LOW; vx += 2) {
                    auto offset = vy * pitch + vx;
                    ptr[offset] = rgb555_to_888(*display);
                    ptr[offset + 1] = ptr[offset] & 0xFFF0F0F0;
                    ptr[offset + pitch] = ptr[offset] & 0x8F8F8F8F;
                    ptr[offset + pitch + 1] = ptr[offset] & 0x80808080;
                    display++;
                }
            }
            break;
        case Resolution::Tiny:
            for (vy = 0; vy < VRAM_HEIGHT_TINY; vy++) {
                int offset = vy * pitch;
                for (vx = 0; vx < VRAM_WIDTH_TINY; vx++) {
                    ptr[offset + vx] = rgb555_to_888(*display);
                    display++;
                }
            }
            break;
    }
}

static int init_sound()
{
    DSBUFFERDESC desc;
    LPDIRECTSOUNDBUFFER tmp = NULL;
    HRESULT res;
    WAVEFORMATEX wFmt;
    putlog("Initializing DirectSound...");
    InitializeCriticalSection(&_lock);

    res = DirectSoundCreate8(NULL, &_lpDS, NULL);
    if (FAILED(res)) {
        putlog("DirectSoundCreate8 failed (%X)", res);
        return -1;
    }

    res = _lpDS->SetCooperativeLevel(hWnd, DSSCL_NORMAL);
    if (FAILED(res)) {
        putlog("SetCooperativeLevel failed (%X)", res);
        return -1;
    }

    memset(&wFmt, 0, sizeof(wFmt));
    wFmt.wFormatTag = WAVE_FORMAT_PCM;
    wFmt.nChannels = 1;
    wFmt.nSamplesPerSec = 44100;
    wFmt.wBitsPerSample = 16;
    wFmt.nBlockAlign = wFmt.nChannels * wFmt.wBitsPerSample / 8;
    wFmt.nAvgBytesPerSec = wFmt.nSamplesPerSec * wFmt.nBlockAlign;
    wFmt.cbSize = 0;
    memset(&desc, 0, sizeof(desc));
    desc.dwSize = (DWORD)sizeof(desc);
    desc.dwFlags = DSBCAPS_CTRLPOSITIONNOTIFY;
    desc.dwFlags |= DSBCAPS_GLOBALFOCUS;
    desc.dwBufferBytes = SBUF_SIZE;
    desc.lpwfxFormat = &wFmt;
    desc.guid3DAlgorithm = GUID_NULL;
    res = _lpDS->CreateSoundBuffer(&desc, &tmp, NULL);
    if (FAILED(res)) {
        putlog("CreateSoundBuffer failed (%X)", res);
        return -1;
    }

    res = tmp->QueryInterface(IID_IDirectSoundBuffer8, (void**)&_lpSB);
    tmp->Release();
    if (FAILED(res)) {
        putlog("QueryInterface - IID_IDirectSoundBuffer8 failed (%X)", res);
        return -1;
    }

    res = _lpSB->QueryInterface(IID_IDirectSoundNotify, (void**)&_lpNtfy);
    if (FAILED(res)) {
        putlog("QueryInterface - IID_IDirectSoundNotify failed (%X)", res);
        return -1;
    }

    _dspn.dwOffset = desc.dwBufferBytes - 1;
    _dspn.hEventNotify = CreateEvent(nullptr, false, false, nullptr);
    if ((HANDLE)-1 == _dspn.hEventNotify || NULL == _dspn.hEventNotify) {
        putlog("CreateEvent failed (%X)", GetLastError());
        return -1;
    }
    res = _lpNtfy->SetNotificationPositions(1, &_dspn);
    if (FAILED(res)) {
        putlog("SetNotificationPositions failed (%X)", res);
        return -1;
    }

    /* start sound controller thread */
    _uiSnd = _beginthread(sound_thread, 65536, NULL);
    if (-1L == _uiSnd) {
        putlog("_beginthread failed (%X)", GetLastError());
        return -1;
    }

    /* wait for ready */
    if (ds_wait(SND_READY)) {
        putlog("wait ready for sound thread failed");
        return -1;
    }
    putlog("Ready for raw PCM streaming.");
    return 0;
}

static void sound_thread(void* arg)
{
    HRESULT res;
    LPVOID lpBuf;
    DWORD dwSize;
    char buf[SBUF_SIZE];

    putlog("Sound thread started.");
    _SndCTRL = SND_READY;

    memset(buf, 0, sizeof(buf));
    while (1) {
        while (SND_READY == _SndCTRL) {
            lock();
            memcpy(buf, vgs0.tickSound(SBUF_SIZE), SBUF_SIZE);
            unlock();
            dwSize = (DWORD)sizeof(buf);
            while (1) {
                res = _lpSB->Lock(0, (DWORD)sizeof(buf), &lpBuf, &dwSize, NULL, NULL, DSBLOCK_FROMWRITECURSOR);
                if (!FAILED(res)) {
                    break;
                }
                Sleep(1);
            }
            memcpy(lpBuf, buf, dwSize);
            res = _lpSB->Unlock(lpBuf, dwSize, NULL, NULL);
            if (FAILED(res)) {
                goto ENDPROC;
            }
            ResetEvent(_dspn.hEventNotify);
            res = _lpSB->SetCurrentPosition(0);
            if (FAILED(res)) {
                goto ENDPROC;
            }
            while (1) {
                res = _lpSB->Play(0, 0, 0);
                if (!FAILED(res))
                    break;
                Sleep(1);
            }
            WaitForSingleObject(_dspn.hEventNotify, 200);
        }
        if (SND_EQ == _SndCTRL)
            break;
        _SndCTRL = SND_READY;
    }
ENDPROC:
    putlog("Sound thread will be ended");
    _SndCTRL = SND_END;
    return;
}

static void term_sound()
{
    if (-1 == _uiSnd) {
        return;
    }
    if (ds_wait(SND_READY)) {
        return;
    }
    _SndCTRL = SND_EQ;
    if (ds_wait(SND_END)) {
        return;
    }
    WaitForSingleObject((HANDLE)_uiSnd, INFINITE);

    if (_lpNtfy) {
        _lpNtfy->Release();
        _lpNtfy = nullptr;
    }
    if ((HANDLE)-1 == _dspn.hEventNotify || nullptr == _dspn.hEventNotify) {
        CloseHandle(_dspn.hEventNotify);
        _dspn.hEventNotify = nullptr;
    }
    if (_lpSB) {
        _lpSB->Release();
        _lpSB = nullptr;
    }
    if (_lpDS) {
        _lpDS->Release();
        _lpDS = nullptr;
    }
    DeleteCriticalSection(&_lock);
}

static int ds_wait(BYTE wctrl)
{
    DWORD ec;
    while (wctrl != _SndCTRL) {
        Sleep(10);
        if (GetExitCodeThread((HANDLE)_uiSnd, &ec)) {
            if (STILL_ACTIVE != ec) {
                return -1;
            }
        } else {
            return -1;
        }
    }
    return 0;
}

static void lock()
{
    EnterCriticalSection(&_lock);
}

static void unlock()
{
    LeaveCriticalSection(&_lock);
}

// DirectInput (Keyboard) の初期値設定
static void reset_keyboard_assign()
{
    for (auto cfg : _kbConfig) delete cfg;
    _kbConfig.clear();
    _kbConfig.push_back(KeyConfig::makeButton(VGS0_JOYPAD_UP, DIK_UP));
    _kbConfig.push_back(KeyConfig::makeButton(VGS0_JOYPAD_DW, DIK_DOWN));
    _kbConfig.push_back(KeyConfig::makeButton(VGS0_JOYPAD_LE, DIK_LEFT));
    _kbConfig.push_back(KeyConfig::makeButton(VGS0_JOYPAD_RI, DIK_RIGHT));
    _kbConfig.push_back(KeyConfig::makeButton(VGS0_JOYPAD_T1, DIK_X));
    _kbConfig.push_back(KeyConfig::makeButton(VGS0_JOYPAD_T2, DIK_Z));
    _kbConfig.push_back(KeyConfig::makeButton(VGS0_JOYPAD_ST, DIK_SPACE));
    _kbConfig.push_back(KeyConfig::makeButton(VGS0_JOYPAD_SE, DIK_ESCAPE));
}

static void toggle_fullscreen()
{
    _isFullScreen = !_isFullScreen;
    if (!_isFullScreen) {
        SetWindowLongPtr(hWnd, GWL_STYLE, WS_OVERLAPPEDWINDOW);
        SetWindowLongPtr(hWnd, GWL_EXSTYLE, 0);
        SetWindowPos(hWnd, HWND_TOP, _windowX, _windowY, _windowWidth, _windowHeight, SWP_NOMOVE | SWP_NOZORDER | SWP_FRAMECHANGED);
        ShowWindow(hWnd, SW_SHOWNORMAL);
        SetMenu(hWnd, hMenu);
        ShowCursor(TRUE);
    } else {
        SetWindowLongPtr(hWnd, GWL_EXSTYLE, WS_EX_TOPMOST);
        SetWindowLongPtr(hWnd, GWL_STYLE, WS_POPUP);
        SetWindowPos(hWnd, HWND_TOP, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_FRAMECHANGED);
        ShowWindow(hWnd, SW_SHOWMAXIMIZED);
        SetMenu(hWnd, nullptr);
        ShowCursor(FALSE);
    }
}
