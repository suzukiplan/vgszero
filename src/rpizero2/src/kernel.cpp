/**
 * VGS-Zero for RaspberryPi Baremetal Environment - Kernel implementation
 * License under GPLv3: https://github.com/suzukiplan/vgszero/blob/master/LICENSE-VGS0.txt
 * (C)2023, SUZUKI PLAN
 */
#include "kernel.h"
#include "vgs0.hpp"
#include "config.hpp"

#define TAG "kernel"
static uint8_t pkg_[16777216];
static UINT pkgSize_;
#define MOUNT_DRIVE "SD:"
#define CONFIG_FILE "/config.sys"
#define PKG_FILE "/game.pkg"
#define SAVE_FILE "/save.dat"
extern "C" const unsigned short splash[46080];
extern "C" const unsigned short sderror[3072];
VGS0* vgs0_;
int pendingCounter_;
uint8_t pad1_;
size_t hdmiPitch_;
uint16_t* hdmiBuffer_;
uint16_t pcmData_[735];
uint8_t saveDataCache_[0x4000];
bool saveDataChanged_;
UINT saveDataSize_;
CLogger* logger_;
SystemConfiguration* config_;

CKernel::CKernel(void) : screen(480, 384),
                         timer(&interrupt),
                         logger(LogPanic, nullptr),
                         usb(&interrupt, &timer, TRUE),
                         vchiq(CMemorySystem::Get(), &interrupt),
                         sound(&vchiq, (TVCHIQSoundDestination)options.GetSoundOption()),
                         emmc(&interrupt, &timer, &led),
                         mcm(CMemorySystem::Get()),
                         gpioUp(22, GPIOModeInputPullUp),
                         gpioDown(5, GPIOModeInputPullUp),
                         gpioLeft(26, GPIOModeInputPullUp),
                         gpioRight(6, GPIOModeInputPullUp),
                         gpioA(24, GPIOModeInputPullUp),
                         gpioB(25, GPIOModeInputPullUp),
                         gpioStart(4, GPIOModeInputPullUp),
                         gpioSelect(23, GPIOModeInputPullUp),
                         gamePad(nullptr)
{
    led.Blink(5);
}

CKernel::~CKernel(void)
{
}

boolean CKernel::initialize(void)
{
    boolean bOK = TRUE;
    led.On();

    if (bOK) {
        bOK = screen.Initialize();
    }

    auto buffer = screen.GetFrameBuffer();
    hdmiPitch_ = buffer->GetPitch() / sizeof(TScreenColor);
    unsigned long ptr = buffer->GetBuffer();
    hdmiBuffer_ = (uint16_t*)ptr;
    auto bptr = hdmiBuffer_;
    uint16_t* splashPtr = (uint16_t*)splash;
    buffer->WaitForVerticalSync();
    for (int y = 0; y < 192; y++) {
        for (int x = 0; x < 240; x++) {
            auto col = *splashPtr;
            splashPtr++;
            bptr[x * 2] = col;
            bptr[x * 2 + 1] = col & 0b1110011100011100; 
            bptr[hdmiPitch_ + x * 2] = col & 0b1001110011110011;
            bptr[hdmiPitch_ + x * 2 + 1] = col & 0b1000010000010000;
        }
        bptr += hdmiPitch_ * 2;
    }

    if (bOK) {
        bOK = serial.Initialize(115200);
    }

    if (bOK) {
        CDevice* target = deviceNameService.GetDevice(options.GetLogDevice(), FALSE);
        if (target == 0) {
            target = &screen;
        }
        bOK = logger.Initialize(target);
    }

    if (bOK) {
        logger.Write(TAG, LogNotice, "init interrupt");
        bOK = interrupt.Initialize();
    }

    if (bOK) {
        logger.Write(TAG, LogNotice, "init timer");
        bOK = timer.Initialize();
    }

    if (bOK) {
        logger.Write(TAG, LogNotice, "init vchiq");
        bOK = vchiq.Initialize();
    }

    if (bOK) {
        logger.Write(TAG, LogNotice, "init usb");
        bOK = usb.Initialize();
    }

    if (bOK) {
        logger.Write(TAG, LogNotice, "init emmc");
        bOK = emmc.Initialize();
    }

    if (bOK) {
        logger.Write(TAG, LogNotice, "init mcm");
        logger_ = &logger;
        bOK = mcm.Initialize();
    }

    if (bOK) {
        led.Off();
    }
    return bOK;
}

void CKernel::updateUsbStatus(void)
{
    if (usb.UpdatePlugAndPlay()) {
        if (!gamePad) {
            gamePad = (CUSBGamePadDevice*)deviceNameService.GetDevice("upad1", FALSE);
            if (gamePad) {
                gamePad->RegisterStatusHandler([](unsigned index, const TGamePadState* state) {
                    pad1_ = 0;
                    pad1_ |= config_->buttonLeft->check(state) ? VGS0_JOYPAD_LE : 0;
                    pad1_ |= config_->buttonRight->check(state) ? VGS0_JOYPAD_RI : 0;
                    pad1_ |= config_->buttonUp->check(state) ? VGS0_JOYPAD_UP : 0;
                    pad1_ |= config_->buttonDown->check(state) ? VGS0_JOYPAD_DW : 0;
                    pad1_ |= config_->buttonA->check(state) ? VGS0_JOYPAD_T1 : 0;
                    pad1_ |= config_->buttonB->check(state) ? VGS0_JOYPAD_T2 : 0;
                    pad1_ |= config_->buttonStart->check(state) ? VGS0_JOYPAD_ST : 0;
                    pad1_ |= config_->buttonSelect->check(state) ? VGS0_JOYPAD_SE : 0;
                });
            }
        } else {
            if (!(CUSBGamePadDevice*)deviceNameService.GetDevice("upad1", FALSE)) {
                gamePad = nullptr;
                pad1_ = 0;
            }
        }
    }
}

TShutdownMode CKernel::run(void)
{
    logger.Write(TAG, LogNotice, "Mounting SD card");
    FRESULT result = f_mount(&fatFs, MOUNT_DRIVE, 1);
    if (FR_OK != result) {
        logger.Write(TAG, LogPanic, "Mount failed! (%d)", (int)result);
        return ShutdownHalt;
    }

    logger.Write(TAG, LogNotice, "Loading config.sys...");
    FIL configSys;
    result = f_open(&configSys, MOUNT_DRIVE CONFIG_FILE, FA_READ | FA_OPEN_EXISTING);
    if (FR_OK == result) {
        char buf[0x10000];
        UINT bufSize;
        if (FR_OK == f_read(&configSys, buf, sizeof(buf) - 1, &bufSize)) {
            buf[bufSize] = 0;
            config_ = new SystemConfiguration(buf);
        } else {
            config_ = new SystemConfiguration("");
        }
        f_close(&configSys);
    } else {
        config_ = new SystemConfiguration("");
    }

    logger.Write(TAG, LogNotice, "Loading game.pkg...");
    FIL gamePkg;
    result = f_open(&gamePkg, MOUNT_DRIVE PKG_FILE, FA_READ | FA_OPEN_EXISTING);
    if (FR_OK != result) {
        logger.Write(TAG, LogPanic, "File not found! (%d)", (int)result);
        return ShutdownHalt;
    }
    result = f_read(&gamePkg, pkg_, sizeof(pkg_), &pkgSize_);
    if (FR_OK != result) {
        logger.Write(TAG, LogPanic, "File read error! (%d)", (int)result);
        return ShutdownHalt;
    }
    logger.Write(TAG, LogNotice, "Load success: %d bytes", (int)pkgSize_);
    f_close(&gamePkg);

    FIL saveDat;
    result = f_open(&saveDat, MOUNT_DRIVE SAVE_FILE, FA_READ | FA_OPEN_EXISTING);
    if (FR_OK == result) {
        memset(saveDataCache_, 0, sizeof(saveDataCache_));
        saveDataChanged_ = false;
        if (FR_OK != f_read(&saveDat, saveDataCache_, sizeof(saveDataCache_), &saveDataSize_)) {
            saveDataSize_ = 0;
        }
        f_close(&saveDat);
    }

    f_unmount(MOUNT_DRIVE);

    void* rom = nullptr;
    void* bgm = nullptr;
    void* se = nullptr;
    int romSize = 0;
    int bgmSize = 0;
    int seSize = 0;
    { // extract package
        uint8_t* ptr = pkg_;
        if (0 != memcmp(ptr, "VGS0PKG", 8)) {
            logger.Write(TAG, LogPanic, "Invalid game.pkg data");
            return ShutdownHalt;
        }
        ptr += 8;
        memcpy(&romSize, ptr, 4);
        if (romSize < 8 + 8192) {
            logger.Write(TAG, LogPanic, "Invalid game.pkg data");
            return ShutdownHalt;
        }
        logger.Write(TAG, LogNotice, "game.rom: %d bytes", romSize);
        ptr += 4;
        rom = ptr;
        ptr += romSize;
        memcpy(&bgmSize, ptr, 4);
        if (bgmSize < 0) {
            logger.Write(TAG, LogPanic, "Invalid game.pkg data");
            return ShutdownHalt;
        }
        logger.Write(TAG, LogNotice, "bgm.dat: %d bytes", bgmSize);
        ptr += 4;
        bgm = ptr;
        ptr += bgmSize;
        memcpy(&seSize, ptr, 4);
        if (seSize < 0) {
            logger.Write(TAG, LogPanic, "Invalid game.pkg data");
            return ShutdownHalt;
        }
        logger.Write(TAG, LogNotice, "se.dat: %d bytes", seSize);
        if (sizeof(pkg_) < (size_t)romSize + bgmSize + seSize) {
            logger.Write(TAG, LogPanic, "Invalid game.pkg data (size too large)");
            return ShutdownHalt;
        }
        ptr += 4;
        se = ptr;
    }

    sound.SetControl(VCHIQ_SOUND_VOLUME_MAX);
    VGS0 vgs0(VDP::ColorMode::RGB565);
    vgs0.loadRom(rom, romSize);
    if (0 < bgmSize) vgs0.loadBgm(bgm, bgmSize);
    if (0 < seSize) vgs0.loadSoundEffect(se, seSize);
    vgs0.setExternalRenderingCallback([](void* arg) {
        CMultiCoreSupport::SendIPI(3, IPI_USER + 2); // request execute rendering core (vdp)
    });
    vgs0.saveCallback = [](VGS0* vgs0, const void* data, size_t size) -> bool {
        // この処理はサブCPUで実行されるのでキャッシュと変更フラグのみ更新して実際のセーブはメイン処理に委ねる
        if (sizeof(saveDataCache_) < size) return false;
        if (saveDataSize_ != size || 0 != memcmp(saveDataCache_, data, size)) {
            memcpy(saveDataCache_, data, size);
            saveDataChanged_ = true;
            saveDataSize_ = size;
        }
        return true;
    };
    vgs0.loadCallback = [](VGS0* vgs0, void* data, size_t size) -> bool {
        // 実際にSDカードからは読み込まずキャッシュから読む
        // NOTE: ゲーム稼働中にSDカードのsave.datを置き換えてゲーム内でロードしても無効
        if (sizeof(saveDataCache_) < size) return false;
        if (0 < saveDataSize_) {
            memcpy(data, saveDataCache_, size);
            return true;
        } else {
            return false;
        }
    };
    vgs0.resetCallback = [](VGS0* vgs0) {
        pendingCounter_ = 16;
    };
    pendingCounter_ = 0;
    vgs0_ = &vgs0;

    // fill empty buffer to the sound queue
    memset(pcmData_, 0, sizeof(pcmData_));
    for (int i = 0; i < 8; i++) {
        sound.Playback(pcmData_, 735, 1, 16);
        scheduler.Yield(); // ensure the VCHIQ tasks can run
    }

    int swap = 0;
    auto buffer = screen.GetFrameBuffer();
    while (1) {
        // update status of the peripheral devices
        updateUsbStatus();

        // Check GPIO joystick if usb gamepad is not connected
        if (!gamePad) {
            auto up = gpioUp.Read();
            auto dw = gpioDown.Read();
            auto le = gpioLeft.Read();
            auto ri = gpioRight.Read();
            auto t1 = gpioA.Read();
            auto t2 = gpioB.Read();
            auto st = gpioStart.Read();
            auto se = gpioSelect.Read();
            if (LOW == le && LOW == ri) {
                ; // 左右を同時に押している場合は入力を無視
            } else {
                pad1_ = 0;
                pad1_ |= LOW == t1 ? VGS0_JOYPAD_T1 : 0;
                pad1_ |= LOW == t2 ? VGS0_JOYPAD_T2 : 0;
                pad1_ |= LOW == st ? VGS0_JOYPAD_ST : 0;
                pad1_ |= LOW == se ? VGS0_JOYPAD_SE : 0;
                pad1_ |= LOW == up ? VGS0_JOYPAD_UP : 0;
                pad1_ |= LOW == dw ? VGS0_JOYPAD_DW : 0;
                pad1_ |= LOW == le ? VGS0_JOYPAD_LE : 0;
                pad1_ |= LOW == ri ? VGS0_JOYPAD_RI : 0;
            }
        }

        // reset pending
        if (pendingCounter_) {
            uint16_t col = 0b0001100011100111;
            auto hdmi = hdmiBuffer_;
            for (int y = 0; y < 192; y++) {
                for (int x = 0; x < 240; x++) {
                    hdmi[x * 2] = col;
                    hdmi[x * 2 + 1] = col & 0b1110011100011100; 
                    hdmi[hdmiPitch_ + x * 2] = col & 0b1001110011110011;
                    hdmi[hdmiPitch_ + x * 2 + 1] = col & 0b1000010000010000;
                }
                hdmi += hdmiPitch_ * 2;
            }
            buffer->WaitForVerticalSync();
            swap = 192 - swap;
            buffer->SetVirtualOffset(0, swap);
            pendingCounter_--;
            continue;
        }

        // request to the another cpus
        CMultiCoreSupport::SendIPI(1, IPI_USER + 0); // request execute main core (z80)
        CMultiCoreSupport::SendIPI(2, IPI_USER + 1); // request execute sound core (vgs)

        // wait v-sync (cpu0 expects synchronize with the another cores)
        buffer->WaitForVerticalSync();

        // flip screen buffer
        swap = 192 - swap;
        buffer->SetVirtualOffset(0, swap);

        // playback sound
        sound.Playback(pcmData_, 735, 1, 16);
        scheduler.Yield(); // ensure the VCHIQ tasks can run

        // save if needed
        if (saveDataChanged_ && 0 < saveDataSize_ && saveDataSize_ < sizeof(saveDataCache_)) {
            led.On();
            int blinkCount = 0;
            saveDataChanged_ = false;
            result = f_mount(&fatFs, MOUNT_DRIVE, 1);
            if (FR_OK != result) {
                logger.Write(TAG, LogError, "Mount failed! (%d)", (int)result);
                blinkCount = 3;
            } else {
                result = f_open(&saveDat, MOUNT_DRIVE SAVE_FILE, FA_WRITE | FA_CREATE_ALWAYS);
                if (FR_OK != result) {
                    logger.Write(TAG, LogError, "File open failed! (%d)", (int)result);
                    blinkCount = 3;
                } else {
                    UINT wrote;
                    result = f_write(&saveDat, saveDataCache_, saveDataSize_, &wrote);
                    if (FR_OK != result || wrote != saveDataSize_) {
                        logger.Write(TAG, LogError, "File write failed! (%d)", (int)result);
                        blinkCount = 3;
                    }
                    f_close(&saveDat);
                }
                f_unmount(MOUNT_DRIVE);
            }
            if (0 < blinkCount) {
                auto bptr = hdmiBuffer_;
                bptr += hdmiPitch_ * 84;
                uint16_t* eptr = (uint16_t*)sderror;
                buffer->WaitForVerticalSync();
                for (int y = 0; y < 24; y++) {
                    memcpy(bptr + 56, eptr, 128 * 2);
                    bptr += hdmiPitch_;
                    eptr += 128;
                }
                swap = 192 - swap;
                buffer->SetVirtualOffset(0, swap);
                led.Blink(blinkCount);
            }
            led.Off();
        }
    }

    return ShutdownHalt;
}
