# VGS0 for RaspberryPi Zero 2W (Bare Metal)

- This is an VGS0 emulator that runs in a RaspberryPi Zero 2W bare-metal environment.
- Compatible with RaspberryPi 3.
- Does not compatible with RaspberryPi Zero, Zero W, Zero WH.

## Required Hardware

- [RaspberryPi Zero 2W](https://www.raspberrypi.com/products/raspberry-pi-zero-2-w/)
  - [Switch Science (Japan)](https://www.switch-science.com/products/7600)
  - [RaspberryPi Shop by KYS (Japan)](https://raspberry-pi.ksyic.com/main/index/pdp.id/849,850,851/pdp.open/849/)
  - [Amazon (Japan)](https://www.amazon.co.jp/dp/B0B55MFH1D/)
- Display device (e.g., TV) capable of outputting video and audio via HDMI input
- Gamepad with USB connection
- FAT-formatted SD card with at least 20MB (NOT 20GB) of free space

## How to Execute

1. Copy `bootcode.bin`, `start.elf`, `kernel8.img` and `game.pkg` to the root directory of the SD card
2. Insert SD card into RaspberryPi Zero 2W and launch

## How to Build

> **NOTE: Build is usually not required.**

- Install XCode CLI Tools (macOS) or build-essential (Linux)
- Install [Arm GNU Toolchain (aarch64-none-elf)](https://developer.arm.com/downloads/-/arm-gnu-toolchain-downloads), and set the environment variable `PATH`
- Execute `make`

## Licenses

See the [root README.md of licenses section](../../README.md#license)
