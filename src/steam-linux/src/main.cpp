/**
 * VGS-Zero SDK for Steam/Linux
 * License under GPLv3: https://github.com/suzukiplan/vgszero/blob/master/LICENSE-VGS0.txt
 * (C)2024, SUZUKI PLAN
 */
#include "SDL.h"
#include "../../core/vgs0.hpp"
#include "steam.hpp"
#include <chrono>
#include <map>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <string>
#include <unistd.h>
#include <vector>
#include <sys/stat.h>

#define WINDOW_TITLE "VGS-Zero SDK for Steam/Linux"

class Binary
{
  public:
    void* data;
    size_t size;

    Binary(void* data_, size_t size_)
    {
        this->data = data_;
        this->size = size_;
    }
};

static pthread_mutex_t soundMutex = PTHREAD_MUTEX_INITIALIZER;
static bool halt = false;
static CSteam* steam = nullptr;

static void log(const char* format, ...)
{
    char buf[256];
    auto now = time(nullptr);
    auto t = localtime(&now);
    va_list args;
    va_start(args, format);
    vsnprintf(buf, sizeof(buf), format, args);
    va_end(args);
    printf("%04d.%02d.%02d %02d:%02d:%02d %s\n",
           t->tm_year + 1900,
           t->tm_mon + 1,
           t->tm_mday,
           t->tm_hour,
           t->tm_min,
           t->tm_sec,
           buf);
}

static void audioCallback(void* userdata, Uint8* stream, int len)
{
    VGS0* vgs0 = (VGS0*)userdata;
    pthread_mutex_lock(&soundMutex);
    if (halt) {
        pthread_mutex_unlock(&soundMutex);
        return;
    }
    void* buf = vgs0->tickSound(len);
    memcpy(stream, buf, len);
    pthread_mutex_unlock(&soundMutex);
}

static inline unsigned char bit5To8(unsigned char bit5)
{
    bit5 <<= 3;
    bit5 |= (bit5 & 0b11100000) >> 5;
    return bit5;
}

static Binary* loadBinary(const char* path)
{
    log("Loading %s", path);
    FILE* fp = fopen(path, "rb");
    if (!fp) return nullptr;
    fseek(fp, 0, SEEK_END);
    size_t size = (size_t)ftell(fp);
    fseek(fp, 0, SEEK_SET);
    void* result = malloc(size);
    if (size != fread(result, 1, size, fp)) {
        fclose(fp);
        free(result);
        return nullptr;
    }
    fclose(fp);
    return new Binary(result, size);
}

int main(int argc, char* argv[])
{
    const char* pkgPath = nullptr;
    bool cliError = false;
    int gpuType = SDL_WINDOW_OPENGL;

    for (int i = 1; !cliError && i < argc; i++) {
        if ('-' != argv[i][0]) {
            pkgPath = argv[i];
            continue;
        }
        switch (tolower(argv[i][1])) {
            case 'g':
                i++;
                if (argc <= i) {
                    cliError = true;
                    break;
                }
                if (0 == strcasecmp(argv[i], "OpenGL")) {
                    gpuType = SDL_WINDOW_OPENGL;
                } else if (0 == strcasecmp(argv[i], "Vulkan")) {
                    gpuType = SDL_WINDOW_VULKAN;
                } else if (0 == strcasecmp(argv[i], "Metal")) {
                    gpuType = SDL_WINDOW_METAL;
                } else if (0 == strcasecmp(argv[i], "None")) {
                    gpuType = 0;
                }
                break;
            case 'h':
                cliError = true;
                break;
            default:
                cliError = true;
                break;
        }
    }
    if (cliError || !pkgPath) {
        puts("usage: vgszero /path/to/game.pkg ....... Specify game package to be used");
        puts("               [-g { None .............. GPU: Do not use");
        puts("                   | OpenGL ............ GPU: OpenGL <default>");
        puts("                   | Vulkan ............ GPU: Vulkan");
        puts("                   | Metal ............. GPU: Metal");
        puts("                   }]");
        return 1;
    }

    log("Booting VGS0 for SDL2.");
    SDL_version sdlVersion;
    SDL_GetVersion(&sdlVersion);
    log("SDL version: %d.%d.%d", sdlVersion.major, sdlVersion.minor, sdlVersion.patch);

    steam = new CSteam(log);
    steam->init();

    Binary* pkg = loadBinary(pkgPath);
    int romSize;
    void* rom = nullptr;
    int bgmSize;
    void* bgm = nullptr;
    int seSize;
    void* se = nullptr;
    char* ptr = (char*)pkg->data;
    if (0 != memcmp(ptr, "VGS0PKG", 8)) {
        puts("Invalid package!");
        return -1;
    }
    ptr += 8;
    memcpy(&romSize, ptr, 4);
    ptr += 4;
    rom = ptr;
    ptr += romSize;
    printf("- game.rom size: %d\n", romSize);
    if (romSize < 8 + 8192) {
        puts("Invalid game.rom size");
        exit(-1);
    }
    memcpy(&bgmSize, ptr, 4);
    ptr += 4;
    bgm = 0 < bgmSize ? ptr : nullptr;
    ptr += bgmSize;
    printf("- bgm.dat size: %d\n", bgmSize);
    memcpy(&seSize, ptr, 4);
    ptr += 4;
    se = 0 < seSize ? ptr : nullptr;
    printf("- se.dat size: %d\n", seSize);

    log("Initializing VGS0");
    VGS0 vgs0;
    if (0 < bgmSize) vgs0.loadBgm(bgm, bgmSize);
    if (0 < seSize) vgs0.loadSoundEffect(se, seSize);
    vgs0.loadRom(rom, romSize);
#if 0
    vgs0.setExternalRenderingCallback([](void* arg) {
        ((VGS0*)arg)->executeExternalRendering();
    });
#endif

    mkdir("save",  S_IRUSR | S_IWUSR | S_IXUSR | S_IRGRP | S_IWGRP | S_IXGRP | S_IROTH | S_IXOTH | S_IXOTH);
    vgs0.saveCallback = [](VGS0* vgs0, const void* data, size_t size) -> bool {
        log("Saving save.dat (%lubytes)", size);
        FILE* fp = fopen("save/save.dat", "wb");
        if (!fp) {

            log("File open error!");
            return false;
        }
        if (size != fwrite(data, 1, size, fp)) {
            log("File write error!");
            fclose(fp);
            return false;
        }
        fclose(fp);
        return true;
    };

    vgs0.loadCallback = [](VGS0* vgs0, void* data, size_t size) -> bool {
        log("Loading save.dat (%lubytes)", size);
        FILE* fp = fopen("save/save.dat", "rb");
        if (!fp) {
            log("File open error!");
            return false;
        }
        size_t readSize = fread(data, 1, size, fp);
        if (readSize < size) {
            log("warning: file size is smaller than expected");
            memset(&((char*)data)[readSize], 0, size - readSize);
        }
        fclose(fp);
        return true;
    };

    log("Initializing SDL");
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_EVENTS)) {
        log("SDL_Init failed: %s", SDL_GetError());
        exit(-1);
    }

    log("Initializing AudioDriver");
    SDL_AudioSpec desired;
    SDL_AudioSpec obtained;
    desired.freq = 44100;
    desired.format = AUDIO_S16LSB;
    desired.channels = 1;
    desired.samples = 735; // desired.freq * 20 / 1000;
    desired.callback = audioCallback;
    desired.userdata = &vgs0;
    auto audioDeviceId = SDL_OpenAudioDevice(nullptr, 0, &desired, &obtained, 0);
    if (0 == audioDeviceId) {
        log(" ... SDL_OpenAudioDevice failed: %s", SDL_GetError());
        exit(-1);
    }
    log("- obtained.freq = %d", obtained.freq);
    log("- obtained.format = %X", obtained.format);
    log("- obtained.channels = %d", obtained.channels);
    log("- obtained.samples = %d", obtained.samples);
    SDL_PauseAudioDevice(audioDeviceId, 0);

    log("create SDL window");
    auto window = SDL_CreateWindow(
        WINDOW_TITLE,
        0,
        0,
        1280,
        800,
        gpuType | SDL_WINDOW_FULLSCREEN);
    SDL_ShowCursor(SDL_DISABLE);

    log("Get SDL window surface");
    auto windowSurface = SDL_GetWindowSurface(window);
    if (!windowSurface) {
        log("SDL_GetWindowSurface failed: %s", SDL_GetError());
        exit(-1);
    }
    log("Resolution: width=%d, height=%d", windowSurface->w, windowSurface->h);
    if (windowSurface->h < 768 || windowSurface->w < 960) {
        log("Unsupported resolution!");
        exit(-1);
    }
    log("PixelFormat: %d bits (%d bytes)", (int)windowSurface->format->BitsPerPixel, (int)windowSurface->format->BytesPerPixel);
    log("Rmask: %08X", (int)windowSurface->format->Rmask);
    log("Gmask: %08X", (int)windowSurface->format->Gmask);
    log("Bmask: %08X", (int)windowSurface->format->Bmask);
    log("Amask: %08X", (int)windowSurface->format->Amask);
    if (4 != windowSurface->format->BytesPerPixel) {
        log("unsupported pixel format (support only 4 bytes / pixel)");
        exit(-1);
    }
    SDL_UpdateWindowSurface(window);

    log("Start main loop...");
    SDL_Event event;
    unsigned int loopCount = 0;
    const int waitFps60[3] = {17000, 17000, 16000};
    unsigned char msxKeyCodeMap[16];
    memset(msxKeyCodeMap, 0, sizeof(msxKeyCodeMap));
    bool stabled = false;
    unsigned char key1 = 0;
    bool joypadConnected = false;
    bool joypadConnectedPrev = false;
    bool detectJoypadDisconnected = false;
    while (!halt) {
        loopCount++;
        auto start = std::chrono::system_clock::now();
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                halt = true;
            } else if (event.type == SDL_KEYDOWN) {
                switch (event.key.keysym.sym) {
                    case SDLK_q: halt = true; break;
                    case SDLK_UP: key1 |= VGS0_JOYPAD_UP; break;
                    case SDLK_DOWN: key1 |= VGS0_JOYPAD_DW; break;
                    case SDLK_LEFT: key1 |= VGS0_JOYPAD_LE; break;
                    case SDLK_RIGHT: key1 |= VGS0_JOYPAD_RI; break;
                    case SDLK_SPACE: key1 |= VGS0_JOYPAD_ST; break;
                    case SDLK_ESCAPE: key1 |= VGS0_JOYPAD_SE; break;
                    case SDLK_x: key1 |= VGS0_JOYPAD_T1; break;
                    case SDLK_z: key1 |= VGS0_JOYPAD_T2; break;
                    case SDLK_r: {
                        log("Reset");
                        vgs0.reset();
                        break;
                    }
                }
            } else if (event.type == SDL_KEYUP) {
                switch (event.key.keysym.sym) {
                    case SDLK_UP: key1 ^= VGS0_JOYPAD_UP; break;
                    case SDLK_DOWN: key1 ^= VGS0_JOYPAD_DW; break;
                    case SDLK_LEFT: key1 ^= VGS0_JOYPAD_LE; break;
                    case SDLK_RIGHT: key1 ^= VGS0_JOYPAD_RI; break;
                    case SDLK_SPACE: key1 ^= VGS0_JOYPAD_ST; break;
                    case SDLK_ESCAPE: key1 ^= VGS0_JOYPAD_SE; break;
                    case SDLK_x: key1 ^= VGS0_JOYPAD_T1; break;
                    case SDLK_z: key1 ^= VGS0_JOYPAD_T2; break;
                }
            }
        }
        if (halt) {
            break;
        }

        auto pad = steam->getJoypad(&joypadConnected);
        if (joypadConnected) {
            detectJoypadDisconnected = false;
            key1 |= pad;
        } else if (joypadConnectedPrev) {
            log("Joypad Disconnected! (waiting for resume..)");
            detectJoypadDisconnected = true;
            usleep(20000);
            continue;
        } else if (detectJoypadDisconnected) {
            usleep(20000);
            continue;
        }
        joypadConnectedPrev = joypadConnected;

        // execute emulator 1 frame
        pthread_mutex_lock(&soundMutex);
        vgs0.tick(key1);
        pthread_mutex_unlock(&soundMutex);
        if (vgs0.cpu->reg.IFF & 0x80) {
            if (0 == (vgs0.cpu->reg.IFF & 0x01)) {
                log("Detected the HALT while DI");
                break;
            }
        }

        // render graphics
        auto vgsDisplay = vgs0.getDisplay();
        auto pcDisplay = (unsigned int*)windowSurface->pixels;
        auto pitch = windowSurface->pitch / windowSurface->format->BytesPerPixel;
        int offsetY = (windowSurface->h - 768) / 2;
        int offsetX = (windowSurface->w - 960) / 2;
        pcDisplay += offsetY;
        for (int y = 0; y < 192; y++) {
            for (int x = 0; x < 240; x++) {
                unsigned int rgb555 = vgsDisplay[x];
                unsigned int rgb888 = 0;
                rgb888 |= bit5To8((rgb555 & 0b0111110000000000) >> 10);
                rgb888 <<= 8;
                rgb888 |= bit5To8((rgb555 & 0b0000001111100000) >> 5);
                rgb888 <<= 8;
                rgb888 |= bit5To8(rgb555 & 0b0000000000011111);
                rgb888 |= windowSurface->format->Amask;
                auto offset = offsetX + x * 4;
                pcDisplay[offset++] = rgb888;
                pcDisplay[offset++] = rgb888;
                pcDisplay[offset++] = rgb888 & 0xF0F0F0F0;
                pcDisplay[offset] = rgb888 & 0xF0F0F0F0;
                offset = offsetX + pitch * 2 + x * 4;
                pcDisplay[offset++] = rgb888 & 0x8F8F8F8F;
                pcDisplay[offset++] = rgb888 & 0x8F8F8F8F;
                pcDisplay[offset++] = rgb888 & 0x80808080;
                pcDisplay[offset] = rgb888 & 0x80808080;
            }
            memcpy(&pcDisplay[offsetX + pitch], &pcDisplay[offsetX], 240 * 4 * 4);
            memcpy(&pcDisplay[offsetX + pitch * 3], &pcDisplay[offsetX + pitch * 2], 240 * 4 * 4);
            vgsDisplay += 240;
            pcDisplay += pitch * 4;
        }
        SDL_UpdateWindowSurface(window);

        // sync 60fps
        std::chrono::duration<double> diff = std::chrono::system_clock::now() - start;
        int us = (int)(diff.count() * 1000000);
        int wait = waitFps60[loopCount % 3];
        if (us < wait) {
            usleep(wait - us);
            if (!stabled) {
                stabled = true;
                log("Frame rate stabilized at 60 fps (%dus per frame)", us);
            }
        } else if (stabled) {
            stabled = false;
            log("warning: Frame rate is lagging (%dus per frame)", us);
        }
    }

    log("Terminating");
    delete steam;
    SDL_CloseAudioDevice(audioDeviceId);
    SDL_DestroyWindow(window);
    SDL_Quit();
    free(pkg->data);
    delete pkg;
    return 0;
}
