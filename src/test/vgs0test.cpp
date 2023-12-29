/**
 * VGS-Zero for Test
 * License under GPLv3: https://github.com/suzukiplan/vgszero/blob/master/LICENSE-VGS0.txt
 * (C)2023, SUZUKI PLAN
 */
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

#define WINDOW_TITLE "VGS0 for SDL2"

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

static void log(std::string text)
{
    log("%s", text.c_str());
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

double cpu1Time = 0.0;
double cpu2Time = 0.0;
double cpu3Time = 0.0;

int main(int argc, char* argv[])
{
    const char* romPath = nullptr;
    std::string result = "result";
    bool cliError = false;
    int frames = 60;

    for (int i = 1; !cliError && i < argc; i++) {
        if ('-' != argv[i][0]) {
            romPath = argv[i];
            continue;
        }
        switch (tolower(argv[i][1])) {
            case 'f':
                cliError = argc <= ++i;
                if (cliError) break;
                frames = atoi(argv[i]);
                break;
            case 'r':
                cliError = argc <= ++i;
                if (cliError) break;
                result = argv[i];
                break;
            default:
                cliError = true;
                break;
        }
    }
    if (cliError || !romPath) {
        puts("usage: vgs0test [-f frame_count] [-r result_base_name] /path/to/game.rom");
        return 1;
    }

    log("Booting VGS0 for test.");
    VGS0 vgs0;
    Binary* rom = loadBinary(romPath);
    vgs0.loadRom(rom->data, rom->size);
    vgs0.setExternalRenderingCallback([](void* arg) {
        auto cpu3Start = std::chrono::system_clock::now();
        ((VGS0*)arg)->executeExternalRendering();
        std::chrono::duration<double> diff = std::chrono::system_clock::now() - cpu3Start;
        cpu3Time += diff.count();
    });
    vgs0.saveCallback = [](VGS0* vgs0, const void* data, size_t size) -> bool {
        log("Saving save.dat (%lubytes)", size);
        return true;
    };
    vgs0.loadCallback = [](VGS0* vgs0, void* data, size_t size) -> bool {
        log("Loading save.dat (%lubytes)", size);
        memset(data, 0x55, size);
        return true;
    };

    log("Start main loop...");
    auto start = std::chrono::system_clock::now();
    for (int i = 0; i < frames; i++) {
        auto cpu1Start = std::chrono::system_clock::now();
        vgs0.tick(0);
        std::chrono::duration<double> diff = std::chrono::system_clock::now() - cpu1Start;
        cpu1Time += diff.count();
        auto cpu2Start = std::chrono::system_clock::now();
        vgs0.tickSound(44100 * 2 / 60);
        diff = std::chrono::system_clock::now() - cpu2Start;
        cpu2Time += diff.count();
    }
    std::chrono::duration<double> diff = std::chrono::system_clock::now() - start;
    int us = (int)(diff.count() * 1000000);
    log("Execution time: %dus", us);
    log("Frame average: %dus", us / frames);
    cpu1Time -= cpu3Time;
    log("CPU1 (Z80) time: %dus (%dus/frame)", (int)(cpu1Time * 1000000), (int)(cpu1Time * 1000000 / frames));
    log("CPU2 (VGS) time: %dus (%dus/frame)", (int)(cpu2Time * 1000000), (int)(cpu2Time * 1000000 / frames));
    log("CPU3 (VDP) time: %dus (%dus/frame)", (int)(cpu3Time * 1000000), (int)(cpu3Time * 1000000 / frames));

    log("writing " + result + "_ram.bin");
    {
        FILE* fp = fopen((result + "_ram.bin").c_str(), "wb");
        if (fp) {
            size_t size;
            void* data = vgs0.getRAM(&size);
            fwrite(data, 1, size, fp);
            fclose(fp);
        }
    }

    log("writing " + result + "_vram.bin");
    {
        FILE* fp = fopen((result + "_vram.bin").c_str(), "wb");
        if (fp) {
            size_t size;
            void* data = vgs0.getVRAM(&size);
            fwrite(data, 1, size, fp);
            fclose(fp);
        }
    }

    free(rom->data);
    delete rom;
    return 0;
}
