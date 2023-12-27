# VGS0 for RaspberryPi Zero (Bare Metal) **deprecated**

> **NOTE:** VGS0 has discontinued support for RaspberryPi Zero.
> Please use the [RaspberryPi Zero 2W version](../rpizero2).

- This is an VGS0 emulator that runs in a RaspberryPi Zero bare-metal environment.
- Compatible with RaspberryPiZero series (Zero, Zero W and Zero WH) and RaspberryPi1.
- Does not compatible with RaspberryPi Zero 2W.

## Required Hardware

- RaspberryPi Zero, Zero 1W or Zero WH
- Display device (e.g., TV) capable of outputting video and audio via HDMI input
- Gamepad with USB connection
- FAT-formatted SD card with at least 20MB (NOT 20GB) of free space

## How to Execute

1. Copy `bootcode.bin`, `start.elf`, `kernel.img` and `game.pkg` to the root directory of the SD card
2. Insert SD card into RaspberryPi Zero and launch

## How to Build

> **NOTE: Build is usually not required.**

- Install XCode CLI Tools (macOS) or build-essential (Linux)
- Install [GNU Arm Embedded Toolchain (arm-none-eabi)](https://developer.arm.com/downloads/-/gnu-rm)
- Execute `make`

## Licenses

See the [root README.md of licenses section](../../README.md#license)
