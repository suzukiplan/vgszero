/**
 * VGS-Zero for SDL2
 * -----------------------------------------------------------------------------
 * The MIT License (MIT)
 *
 * Copyright (c) 2023 Yoji Suzuki.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 * -----------------------------------------------------------------------------
 */
#include "BufferQueue.h"
#include "SDL.h"
#include "fcs80.hpp"
#include <chrono>
#include <map>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <string>
#include <unistd.h>
#include <vector>

#define WINDOW_TITLE "VGS-Zero for SDL2"
#define USE_CBIOS

typedef struct {
    void* data;
    size_t size;
} Binary;

static BufferQueue soundQueue(65536);
static pthread_mutex_t soundMutex = PTHREAD_MUTEX_INITIALIZER;
static bool halt = false;

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
    pthread_mutex_lock(&soundMutex);
    if (halt) {
        pthread_mutex_unlock(&soundMutex);
        return;
    }
    if (soundQueue.getCursor() < len) {
        memset(stream, 0, len);
    } else {
        void* buf;
        size_t bufSize;
        soundQueue.dequeue(&buf, &bufSize, len);
        memcpy(stream, buf, len);
    }
    pthread_mutex_unlock(&soundMutex);
    usleep(1000);
}

static inline unsigned char bit5To8(unsigned char bit5)
{
    bit5 <<= 3;
    bit5 |= (bit5 & 0b11100000) >> 5;
    return bit5;
}

static void* loadBinary(const char* path, size_t* size)
{
    log("Loading %s", path);
    FILE* fp = fopen(path, "rb");
    if (!fp) return nullptr;
    fseek(fp, 0, SEEK_END);
    *size = (size_t)ftell(fp);
    fseek(fp, 0, SEEK_SET);
    void* result = malloc(*size);
    if (*size != fread(result, 1, *size, fp)) {
        fclose(fp);
        free(result);
        return nullptr;
    }
    fclose(fp);
    return result;
}

int main(int argc, char* argv[])
{
    const char* romPath = nullptr;
    bool cliError = false;
    int fullScreen = 0;
    int gpuType = SDL_WINDOW_OPENGL;
    std::vector<Binary*> vgsTable;

    for (int i = 1; !cliError && i < argc; i++) {
        if ('-' != argv[i][0]) {
            romPath = argv[i];
            continue;
        }
        switch (tolower(argv[i][1])) {
            case 'v': {
                i++;
                if (argc <= i) {
                    cliError = true;
                    break;
                }
                auto bin = new Binary();
                bin->data = loadBinary(argv[i], &bin->size);
                if (!bin->data) {
                    puts("File not found");
                    cliError = true;
                    break;
                }
                vgsTable.push_back(bin);
                break;
            }
            case 'f':
                fullScreen = SDL_WINDOW_FULLSCREEN;
                break;
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
    if (cliError || !romPath) {
        puts("usage: vgszero /path/to/file.rom ....... Specify ROM file to be used");
        puts("               [-v /path/to/bgm.vgs] ... VGS BGM data (max 256 data)");
        puts("               [-g { None .............. GPU: Do not use");
        puts("                   | OpenGL ............ GPU: OpenGL <default>");
        puts("                   | Vulkan ............ GPU: Vulkan");
        puts("                   | Metal ............. GPU: Metal");
        puts("                   }]");
        puts("               [-f] .................... Full Screen Mode");
        return 1;
    }

    log("Booting FCS80 for SDL2.");
    SDL_version sdlVersion;
    SDL_GetVersion(&sdlVersion);
    log("SDL version: %d.%d.%d", sdlVersion.major, sdlVersion.minor, sdlVersion.patch);

    log("Initializing VGS-Zero");
    FCS80 fcs80;

    int vgsIndex = 0;
    for (Binary* vgs : vgsTable) {
        log("Loaded BGM data #%d (%lu bytes)", vgsIndex, vgs->size);
        fcs80.loadVgsData(vgsIndex++, vgs->data, vgs->size);
    }

    if (!fcs80.loadRomFile(romPath)) {
        log("Load ROM failed: %s", romPath);
        return -1;
    }

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
    desired.channels = 2;
    desired.samples = 735; // desired.freq * 20 / 1000;
    desired.callback = audioCallback;
    desired.userdata = &fcs80;
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
        SDL_WINDOWPOS_UNDEFINED,
        SDL_WINDOWPOS_UNDEFINED,
        fullScreen ? 640 : 480,
        fullScreen ? 480 : 384,
        gpuType | fullScreen);
    if (fullScreen) {
        SDL_ShowCursor(SDL_DISABLE);
    }
    log("Get SDL window surface");
    auto windowSurface = SDL_GetWindowSurface(window);
    if (!windowSurface) {
        log("SDL_GetWindowSurface failed: %s", SDL_GetError());
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
    bool hotKey = false;
    unsigned char key1 = 0;
    while (!halt) {
        loopCount++;
        auto start = std::chrono::system_clock::now();
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_KEYDOWN) {
                switch (event.key.keysym.sym) {
                    case SDLK_q: halt = true; break;
                    case SDLK_UP: key1 |= FCS80_JOYPAD_UP; break;
                    case SDLK_DOWN: key1 |= FCS80_JOYPAD_DW; break;
                    case SDLK_LEFT: key1 |= FCS80_JOYPAD_LE; break;
                    case SDLK_RIGHT: key1 |= FCS80_JOYPAD_RI; break;
                    case SDLK_SPACE: key1 |= FCS80_JOYPAD_ST; break;
                    case SDLK_ESCAPE: key1 |= FCS80_JOYPAD_SE; break;
                    case SDLK_x: key1 |= FCS80_JOYPAD_T1; break;
                    case SDLK_z: key1 |= FCS80_JOYPAD_T2; break;
                }
            } else if (event.type == SDL_KEYUP) {
                switch (event.key.keysym.sym) {
                    case SDLK_UP: key1 ^= FCS80_JOYPAD_UP; break;
                    case SDLK_DOWN: key1 ^= FCS80_JOYPAD_DW; break;
                    case SDLK_LEFT: key1 ^= FCS80_JOYPAD_LE; break;
                    case SDLK_RIGHT: key1 ^= FCS80_JOYPAD_RI; break;
                    case SDLK_SPACE: key1 ^= FCS80_JOYPAD_ST; break;
                    case SDLK_ESCAPE: key1 ^= FCS80_JOYPAD_SE; break;
                    case SDLK_x: key1 ^= FCS80_JOYPAD_T1; break;
                    case SDLK_z: key1 ^= FCS80_JOYPAD_T2; break;
                }
            }
        }
        if (halt) {
            break;
        }

        // execute emulator 1 frame
        fcs80.tick(key1, 0);

        // enqueue sound
        pthread_mutex_lock(&soundMutex);
        size_t pcmSize;
        auto pcm = fcs80.dequeSoundBuffer(&pcmSize);
        soundQueue.enqueue(pcm, pcmSize);
        pthread_mutex_unlock(&soundMutex);

        // render graphics
        auto fcs80Display = fcs80.getDisplay();
        auto pcDisplay = (unsigned int*)windowSurface->pixels;
        auto pitch = windowSurface->pitch / windowSurface->format->BytesPerPixel;
        int offsetY = fullScreen ? 48 * pitch : 0;
        int offsetX = fullScreen ? 80 : 0;
        pcDisplay += offsetY;
        for (int y = 0; y < 192; y++) {
            for (int x = 0; x < 240; x++) {
                unsigned int rgb555 = fcs80Display[x];
                unsigned int rgb888 = 0;
                rgb888 |= bit5To8((rgb555 & 0b0111110000000000) >> 10);
                rgb888 <<= 8;
                rgb888 |= bit5To8((rgb555 & 0b0000001111100000) >> 5);
                rgb888 <<= 8;
                rgb888 |= bit5To8(rgb555 & 0b0000000000011111);
                rgb888 |= windowSurface->format->Amask;
                pcDisplay[offsetX + x * 2] = rgb888;
                pcDisplay[offsetX + x * 2 + 1] = rgb888;
            }
            fcs80Display += 240;
            memcpy(&pcDisplay[pitch], &pcDisplay[0], windowSurface->pitch);
            pcDisplay += pitch * 2;
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
    SDL_CloseAudioDevice(audioDeviceId);
    SDL_DestroyWindow(window);
    SDL_Quit();
    for (Binary* bin : vgsTable) {
        free(bin->data);
        delete bin;
    }
    return 0;
}
