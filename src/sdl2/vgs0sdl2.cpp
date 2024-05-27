/**
 * VGS0 for SDL2
 * License under GPLv3: https://github.com/suzukiplan/vgszero/blob/master/LICENSE-VGS0.txt
 * (C)2023, SUZUKI PLAN
 */
#include "SDL.h"
#include "vgs0.hpp"
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

typedef struct BitmapHeader_ {
    int isize;             /* 情報ヘッダサイズ */
    int width;             /* 幅 */
    int height;            /* 高さ */
    unsigned short planes; /* プレーン数 */
    unsigned short bits;   /* 色ビット数 */
    unsigned int ctype;    /* 圧縮形式 */
    unsigned int gsize;    /* 画像データサイズ */
    int xppm;              /* X方向解像度 */
    int yppm;              /* Y方向解像度 */
    unsigned int cnum;     /* 使用色数 */
    unsigned int inum;     /* 重要色数 */
} BitmapHeader;

static pthread_mutex_t soundMutex = PTHREAD_MUTEX_INITIALIZER;
static bool halt = false;
static bool disasm = false;

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

const void* get_bitmap(const unsigned short* display, size_t* size, bool interlace)
{
    static unsigned char buf[14 + 40 + 480 * 384 * 4];
    int iSize = (int)sizeof(buf);
    *size = iSize;
    memset(buf, 0, sizeof(buf));
    int ptr = 0;
    buf[ptr++] = 'B';
    buf[ptr++] = 'M';
    memcpy(&buf[ptr], &iSize, 4);
    ptr += 4;
    ptr += 4;
    iSize = 14 + 40;
    memcpy(&buf[ptr], &iSize, 4);
    ptr += 4;
    BitmapHeader header;
    header.isize = 40;
    header.width = interlace ? 480 : 240;
    header.height = interlace ? 384 : 192;
    header.planes = 1;
    header.bits = 32;
    header.ctype = 0;
    header.gsize = header.width * header.height * (header.bits / 8);
    header.xppm = 1;
    header.yppm = 1;
    header.cnum = 0;
    header.inum = 0;
    memcpy(&buf[ptr], &header, sizeof(header));
    ptr += sizeof(header);
    for (int y = 0; y < 192; y++) {
        for (int x = 0; x < 240; x++) {
            auto col = display[(191 - y) * 240 + x];
            unsigned int rgb888 = 0;
            rgb888 |= bit5To8((col & 0b0111110000000000) >> 10);
            rgb888 <<= 8;
            rgb888 |= bit5To8((col & 0b0000001111100000) >> 5);
            rgb888 <<= 8;
            rgb888 |= bit5To8(col & 0b0000000000011111);
            memcpy(&buf[ptr + 240 * 8], &rgb888, 4);
            if (interlace) {
                unsigned int n = rgb888 & 0xF0F0F0F0;
                memcpy(&buf[ptr + 240 * 8 + 4], &n, 4);
                n = rgb888 & 0x8F8F8F8F;
                memcpy(&buf[ptr], &n, 4);
                n &= 0x80808080;
                memcpy(&buf[ptr + 4], &n, 4);
                ptr += 8;
            } else {
                ptr += 4;
            }
        }
        if (interlace) {
            ptr += 240 * 8;
        }
    }
    *size = ptr;
    return buf;
}

int main(int argc, char* argv[])
{
    const char* pkgPath = nullptr;
    bool cliError = false;
    bool debugMode = false;
    int fullScreen = 0;
    int gpuType = SDL_WINDOW_OPENGL;

    for (int i = 1; !cliError && i < argc; i++) {
        if ('-' != argv[i][0]) {
            pkgPath = argv[i];
            continue;
        }
        switch (tolower(argv[i][1])) {
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
            case 'd':
                debugMode = true;
                break;
            default:
                cliError = true;
                break;
        }
    }
    if (cliError || !pkgPath) {
        puts("usage: vgs0 /path/to/game.pkg ....... Specify game package to be used");
        puts("            [-g { None .............. GPU: Do not use");
        puts("                | OpenGL ............ GPU: OpenGL <default>");
        puts("                | Vulkan ............ GPU: Vulkan");
        puts("                | Metal ............. GPU: Metal");
        puts("                }]");
        puts("            [-f] .................... Full Screen Mode");
        puts("            [-d] .................... Enable Debug Mode (NOP break)");
        return 1;
    }

    log("Booting VGS0 for SDL2.");
    SDL_version sdlVersion;
    SDL_GetVersion(&sdlVersion);
    log("SDL version: %d.%d.%d", sdlVersion.major, sdlVersion.minor, sdlVersion.patch);

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

    vgs0.saveCallback = [](VGS0* vgs0, const void* data, size_t size) -> bool {
        log("Saving save.dat (%lubytes)", size);
        FILE* fp = fopen("save.dat", "wb");
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
        FILE* fp = fopen("save.dat", "rb");
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

    if (debugMode) {
        vgs0.cpu->addBreakOperand(0x00, [](void* arg, unsigned char* op, int len) {
            auto vgs0 = (VGS0*)arg;
            log("NOP at 0x%04X", vgs0->cpu->reg.PC);
            printf("A :0x%02X F :0x%02X, B :0x%02X, C :0x%02X, D :0x%02X, E :0x%02X, H :0x%02X, L :0x%02X\n", vgs0->cpu->reg.pair.A, vgs0->cpu->reg.pair.F, vgs0->cpu->reg.pair.B, vgs0->cpu->reg.pair.C, vgs0->cpu->reg.pair.D, vgs0->cpu->reg.pair.E, vgs0->cpu->reg.pair.H, vgs0->cpu->reg.pair.L);
            printf("A':0x%02X F':0x%02X, B':0x%02X, C':0x%02X, D':0x%02X, E':0x%02X, H':0x%02X, L':0x%02X\n", vgs0->cpu->reg.back.A, vgs0->cpu->reg.back.F, vgs0->cpu->reg.back.B, vgs0->cpu->reg.back.C, vgs0->cpu->reg.back.D, vgs0->cpu->reg.back.E, vgs0->cpu->reg.back.H, vgs0->cpu->reg.back.L);
            printf("PC:0x%04X, SP:0x%04X, IX:0x%04X, IY:0x%04X, I :0x%02X, R :0x%02X\n", vgs0->cpu->reg.PC, vgs0->cpu->reg.SP, vgs0->cpu->reg.IX, vgs0->cpu->reg.IY, vgs0->cpu->reg.I, vgs0->cpu->reg.R);
            printf("ROM BANK: 0x%02X, 0x%02X, 0x%02X, 0x%02X\n", vgs0->ctx.romBank[0], vgs0->ctx.romBank[1], vgs0->ctx.romBank[2], vgs0->ctx.romBank[3]);
            printf("RAM BANK: 0x%02X\n", vgs0->vdp->ctx.bank);
            printf("SCANLINE: V=%d, H=%d\n", vgs0->vdp->ctx.countV, vgs0->vdp->ctx.countH);
            printf("  SCROLL: BGX=%d, BGY=%d, FGX=%d, FGY=%d\n", vgs0->vdp->ctx.ram0[0x1F02], vgs0->vdp->ctx.ram0[0x1F03], vgs0->vdp->ctx.ram0[0x1F04], vgs0->vdp->ctx.ram0[0x1F05]);
            char buf[256];
            bool end = false;
            while (!end && fgets(buf, sizeof(buf), stdin)) {
                for (int i = 0; i < 256 && buf[i]; i++) {
                    buf[i] = toupper(buf[i]);
                }
                buf[255] = 0;
                switch (buf[0]) {
                    case 'M': {
                        unsigned short addr = 0xC000;
                        unsigned short size = 256;
                        const char* ptr = &buf[1];
                        while (' ' == *ptr) {
                            ptr++;
                        }
                        bool endAddr = false;
                        while (!endAddr) {
                            if ('0' <= *ptr && *ptr <= '9') {
                                addr &= 0x0FFF;
                                addr <<= 4;
                                addr |= (*ptr) - '0';
                                ptr++;
                            } else if ('A' <= *ptr && *ptr <= 'F') {
                                addr &= 0x0FFF;
                                addr <<= 4;
                                addr |= (*ptr) - 'A' + 10;
                                ptr++;
                            } else {
                                endAddr = true;
                            }
                        }
                        if (' ' == *ptr) {
                            while (' ' == *ptr) {
                                ptr++;
                            }
                            size = atoi(ptr);
                        }
                        printf("Dump from 0x%04X (%d bytes)\n", addr, size);
                        printf("ADDR  +0 +1 +2 +3 +4 +5 +6 +7   +8 +9 +A +B +C +D +E +F  ASCII");
                        unsigned char data[17];
                        int dptr = 0;
                        while (0 < size) {
                            if (0 == dptr) {
                                printf("\n%04X:", addr);
                                memset(data, 0, sizeof(data));
                            }
                            data[dptr] = vgs0->readMemory(addr++);
                            if (8 == dptr) {
                                printf(" - %02X", data[dptr]);
                            } else {
                                printf(" %02X", data[dptr]);
                            }
                            if (!isprint(data[dptr])) {
                                data[dptr] = '.';
                            }
                            dptr++;
                            dptr &= 0xF;
                            if (0 == dptr) {
                                printf("  %s", data);
                            }
                            size--;
                        }
                        if (dptr) {
                            for (; dptr < 16; dptr++) {
                                if (8 == dptr) {
                                    printf("     ");
                                } else {
                                    printf("   ");
                                }
                            }
                            printf("  %s\n", data);
                        } else {
                            printf("\n");
                        }
                        break;
                    }
                    case 'H':
                    case '?':
                        puts("M ADDR SIZE ... Memory Dump (ADDR: HEX, SIZE: DEC)");
                        puts("D ............. Toggle Disassemble");
                        puts("H or ? ........ Help");
                        puts("Other ......... Continue");
                        break;
                    case 'D':
                        disasm = !disasm;
                        if (disasm) {
                            puts("Enabled Disassemble");
                            vgs0->cpu->setDebugMessage([](void* arg, const char* msg) { puts(msg); });
                        } else {
                            puts("Disable Disassemble");
                            vgs0->cpu->resetDebugMessage();
                        }
                        break;
                    default:
                        end = true;
                }
            }
        });
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
    unsigned char key1 = 0;
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
                    case SDLK_s: {
                        log("Save ram.bin, vram.bin, screen_480x384.bmp, screen_240x192.bmp");
                        FILE* fp = fopen("ram.bin", "wb");
                        if (fp) {
                            fwrite(vgs0.ctx.ram, 1, sizeof(vgs0.ctx.ram), fp);
                            fclose(fp);
                        }
                        fp = fopen("vram.bin", "wb");
                        if (fp) {
                            fwrite(vgs0.vdp->ctx.ram0, 1, 0x2000, fp);
                            fwrite(&vgs0.vdp->ctx.ram1[vgs0.vdp->ctx.bank][0], 1, 0x2000, fp);
                            fclose(fp);
                        }
                        fp = fopen("screen_480x384.bmp", "wb");
                        if (fp) {
                            size_t bmpSize;
                            const void* bmp = get_bitmap(vgs0.getDisplay(), &bmpSize, true);
                            fwrite(bmp, 1, bmpSize, fp);
                            fclose(fp);
                        }
                        fp = fopen("screen_240x192.bmp", "wb");
                        if (fp) {
                            size_t bmpSize;
                            const void* bmp = get_bitmap(vgs0.getDisplay(), &bmpSize, false);
                            fwrite(bmp, 1, bmpSize, fp);
                            fclose(fp);
                        }
                        break;
                    }
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
        int offsetY = fullScreen ? 48 * pitch : 0;
        int offsetX = fullScreen ? 80 : 0;
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
                pcDisplay[offsetX + x * 2] = rgb888;
                pcDisplay[offsetX + x * 2 + 1] = rgb888 & 0xF0F0F0F0;
                pcDisplay[offsetX + pitch + x * 2] = rgb888 & 0x8F8F8F8F;
                pcDisplay[offsetX + pitch + x * 2 + 1] = rgb888 & 0x80808080;
            }
            vgsDisplay += 240;
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
    free(pkg->data);
    delete pkg;
    return 0;
}
