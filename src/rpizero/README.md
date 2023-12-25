# FCS80 for RaspberryPi Zero (Bare Metal)

- This is an FCS80 emulator that runs in a RaspberryPi Zero bare-metal environment.
- Compatible with RaspberryPiZero series (Zero, Zero W and Zero WH) and RaspberryPi1.
- Does not compatible with RaspberryPi Zero 2W.

## Required Hardware

- RaspberryPi Zero, Zero 1W or Zero WH
- Display device (e.g., TV) capable of outputting video and audio via HDMI input
- Gamepad with USB connection
- FAT-formatted SD card with at least 4MB (NOT 4GB) of free space

## How to Execute

1. Copy files under the [`image`](image) directory to the root directory of the SD card
2. Replace `game.rom` with the ROM file you want to run
3. Insert SD card into RaspberryPi Zero and launch

## How to Build

> **NOTE: Build is usually not required.**

- Install XCode CLI Tools (macOS) or build-essential (Linux)
- Install [GNU Arm Embedded Toolchain (arm-none-eabi)](https://developer.arm.com/downloads/-/gnu-rm)
- Execute `make`

## Licenses

- [bootcode.bin and start.elf](./licenses-copy/LICENCE.broadcom)
- [Circle](./licenses-copy/circle.txt)
- [FCS80](../../LICENSE.txt)
