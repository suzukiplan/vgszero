# Video Game System - Zero [![suzukiplan](https://circleci.com/gh/suzukiplan/vgszero.svg?style=svg)](https://app.circleci.com/pipelines/github/suzukiplan/vgszero)

Language: [Japanese](./README.md), __English__

![logo](logo.png)

The Video Game System - Zero (VGS-Zero) is a game console that runs on a RaspberryPi Zero 2W bare-metal environment.

This repository provides the VGS-Zero body code, distribution images, SDK, and an emulator that runs on a PC (Linux or macOS).

## Table of Content

1. [VGS-Zero Feature](#vgs-zero-feature)
1. [First Step Guide](#first-step-guide)
1. [How to Execute](#how-to-execute)
1. [config.sys](#configsys)
1. [game.pkg](#gamepkg)
1. [Game Development Tools](#game-development-tools)
1. [Programming Guide](#programming-guide)
1. [How to Sell Your Game](#how-to-sell-your-game)
1. [Examples](#examples)
1. [License](#license)

## VGS-Zero Feature

- CPU: Z80 16MHz (16,777,216Hz)
  - Programmable in Z80 assembly language (see [Programming Guide](#programming-guide))
  - Can also be programmed in C with SDCC (see [Programming Guide](#programming-guide))
  - The game executable file [game.pkg](#gamepkg) has a maximum size of 128 megabits.
  - [Up to 2MB (8KB x 256banks)](#cpu-memory-map) of programs and data _(*excluding voice data)_
  - [RAM size 16KB](#cpu-memory-map) (PV16相当!)
  - [Extended RAM size 2MB](#extra-ram-bank)
  - [Built-in RAM save function](#save-data)に対応
- VDP; VGS-Video (Video Display Processor)
  - [VRAM size 16KB](#vram-memory-map)
  - Screen resolution: 240x192 pixels
  - [16 x 16-color palettes](#palette) available（Simultaneous 256 out of 32,768 colors）
  - Up to 256 (8KB) 8x8 pixel [character-pattern](#character-pattern-table) can be defined
  - [Name table](#name-table) size for [BG](#bg) and [FG](#fg): 32x32 (256x256 pixels)
  - Support for [hardware scroll](#hardware-scroll) ([BG](#bg), [FG](#fg) each)
  - Capable of displaying up to 256 [sprites](#sprite) (no horizontal limit)
  - Supports [Direct Pattern Mapping](#direct-pattern-mapping) function to set different [character patterns]((#character-pattern-table)) for [BG](#bg), [FG](#fg), and [sprites](#sprite).
  - [BG](#bg) and [FG](#fg) support [1024 patterns mode](#1024-patterns-mode).
  - Provides a hardware function ([OAM Pattern Size](#oam-pattern-size)) that allows multiple [character pattern](#character-pattern-table) to be displayed side by side on a [sprite](#sprite).
  - Provides a hardware function ([OAM Bank](#oam-bank)) to specify a different bank for each [OAM record](#oam) of [sprite](#sprite).
  - Provides [OAM16](#oam16) which can make the coordinate system of [sprite](#sprite) 16bit.
- DMA (Direct Memory Access)
  - [High-speed transfer of the contents of a specific ROM bank to the character pattern table](#rom-to-character-dma)
  - [Capable of transferring the contents of a specific ROM bank to any memory in any size](#rom-to-memory-dma)
  - [High-speed DMA transfer functionality equivalent to `memset` in C is available](#memset-dma)
  - [High-speed DMA transfer functionality equivalent to `memcpy` in C is available](#memcpy-dma)
- HAGe (High-speed Accumulator for Game)
  - [Hardware hit-or-miss function](#collision-detection)
  - [Hardware multiplication, division, and remainder arithmetic](#hardware-calculation)
  - [Hardware sin table](#hardware-sin-table)
  - [Hardware cos table](#hardware-cos-table)
  - [Hardware atan2 table](#hardware-atan2-table)
  - [Hardware random number generator](#hardware-random)
  - [Hardware perlin noise](#hardware-perlin-noise)
- [BGM](#bgmdat)
  - Playback of background music in [VGS MML](#compile-mml) or [NSF](#nsf) format
  - No need to implement sound drivers on the game program (Z80) side!
  - No need to occupy RAM (16KB) on the game program (Z80) side!
  - Separate asset ([`bgm.dat`](#bgmdat)) from the main ROM ([`game.rom`](#gamerom))
  - Up to 256 songs
- [SE](#sedat) (Sound Effect)
  - PCM data (.wav files) in 44100Hz 16bit 1ch (mono) format can be played back as sound effects
  - No need to implement sound drivers on the game program (Z80) side!
  - No need to occupy RAM (16KB) on the game program (Z80) side!
  - Separate asset ([`se.dat`](#sedat)) from the main ROM ([`game.rom`](#gamerom))
  - Up to 256 sound effects
- Input Devices:
  - [USB joypad](#joypad)
    - Supports joypad in 8-button format (cursor keys, A/B, START/SELECT)
    - Button assignments can be customized via [config.sys](#configsys)
  - [GPIO joypad](#gpio-joypad)
    - Support for joypads that connect directly to RaspberryPi GPIOs

## First Step Guide

The recommended OS for the VGS-Zero game development environment is [Ubuntu Desktop](https://jp.ubuntu.com/download).

More precisely, __Ubuntu Desktop installed on a 2013 MacBook Air__ is the __baseline environment__ in which you can comfortably focus on game development without any inconvenience.

With VGS-Zero, you don't need __a strong PC__ or __a huge SDK download of tens of GB__ to develop games.

Below is a step-by-step guide to running VGS-Zero's [Hello, World! (Z80)](./example/01_hello-asm/) on Ubuntu Desktop with nothing installed:

```bash
# Install middleware needed to build toolchain
sudo apt update
sudo apt install build-essential libsdl2-dev libasound2 libasound2-dev

# Download the VGS-Zero repository
git clone https://github.com/suzukiplan/vgszero

# Move directory
cd vgszero/example/01_hello-asm

# Build and Execute
make
```

If you follow the above steps, the SDL2 version of the VGS-Zero emulator will start `Hello, World`.

![preview](./preview.png)

The toolsets available in VGS-Zero for drawing graphics, creating sound effects, composing music, etc. are listed in detail in the [Game Development Tools](#game-development-tools) chapter.

## How to Execute

This section describes the procedure for running the game on the actual device (RaspberryPi Zero 2W).

### Required Hardware

The following hardware is required:

- RaspberryPi Zero 2W
- HDMI cable (mini HDMI Type C → HDMI Type A)
- USB joypad（D-Pad+A/B+Start/Select）+ Type A to B convert adapter
- USB power supply
- micro SD card (need 20MB empty)
- Displays that meet the following requirements:
  - HDMI input
  - Refresh rate 60Hz
  - Resolution 480x384 pixels or higher
  - Audio output

#### (Joypad)

VGS-Zero only supports input via the 8-button USB joypad with cursor (D-PAD), A button, B button, SELECT button, and START button.

![joypad.png](joypad.png)

The button assignments (key config) of the USB joypad connected to the RaspberryPi Zero 2W can be freely customized by the user via the [config.sys](#configsys) file.

The key assignments for the PC ([SDL2](./src/sdl2/) version) are as follows:

- D-Pad: cursor key
- A button: `X` key
- B button: `Z` key
- START button: `SPACE` key
- SELECT button: `ESC` key

The input status can be obtained from the program by [inputting the 0xA0 port](#joypad-1).

#### (Supported USB Joypad)

The following is a list of USB joypad support in VGS-Zero:

|Product name|Support|Supplementary info.|
|:-|:-:|:-|
|[HXBE37823 (XINYUANSHUNTONG)](https://ja.aliexpress.com/item/1005001905753033.html)|`OK`|Works perfectly|
|[suily USB controller for NES games（wired model）](https://www.amazon.co.jp/dp/B07M7SYX11/)|`OK`|Works perfectly|
|[Elecom JC-U3312](https://www.amazon.co.jp/dp/B003UIRHLE/)|`OK`|Works perfectly|
|[HORI Real Arcade Pro V3SA for PS3](https://www.amazon.co.jp/dp/B002YT9PSI)|`OK`|Works perfectly|
|[HORI Grip Controller Attachment Set for Nintendo Switch](https://www.amazon.co.jp/dp/B09JP9MFFY/)|`NG`|The four-way controller is not usable.|
|[Logicool (Logitech) F310](https://www.amazon.co.jp/dp/B00CDG799E/)|`NG`|No connection|
|[Kiwitata gamepad](https://github.com/rsta2/circle/wiki/Device-compatibility-list)|`NG`|Circle is unsupported|
|[Xbox 360 wired gamepad clone](https://github.com/rsta2/circle/wiki/Device-compatibility-list)|`NG`|Circle is unsupported|

> A standard HID-compliant joypad (game controller) is likely to work.
>
> We do not recommend the use of game controllers that require a dedicated device driver for use on a PC, or game controllers that support XInput (relatively new game controllers), as many of them tend to not be recognized at all, or even if they are recognized, some key input does not work.
>
> SUZUKIPLAN primarily uses the Elecom JC-U3312 and HXBE37823; the Elecom JC-U3312 is an EOL product and may be difficult to find. The HXBE37823 is available inexpensively from Aliexpress and Amazon and is recommended, but may have slightly more oblique input misdetection. (It is not considered to be of high quality, so there may be individual variability issues.)
> 
> Games like [Battle Marine](https://github.com/suzukiplan/bmarine-zero/) can be played comfortably on the HXBE37823 if the direction of movement is focused to the left or right.

#### (GPIO Joypad)

You can also connect buttons directly to the GPIOs of the RaspberryPi Zero 2W with the following pin assignments:

| Button | GPIO |
|:------:|:----:|
| Up     | 22   |
| Down   | 5    |
| Left   | 26   |
| Right  | 6    |
| A      | 24   |
| B      | 25   |
| Start  | 4    |
| Select | 23   |

RaspberryPi Zero 2W Pin Map

![joypad_pin.jpeg](joypad_pin.jpeg)

_NOTE: GND connection is also required._

> For more technical details on GPIO connections, please refer to [this article (Japanese)](https://note.com/suzukiplan/n/ncccafb305eae).
>
> VGS-Zero compatible joysticks and joypads can be developed and sold freely without a license from SUZUKIPLAN, regardless of whether they are for companies (commercial hardware) or individuals (doujin hardware). _No license is required, but our support is not provided, so it is the responsibility of the distributor to provide support to consumers._

Although more difficult to implement than a USB joypad, it has the following advantages:

- No compatibility issues
- Faster response time than USB joypads

If you are selling hardware that can play VGS-Zero games, we recommend that you basically implement it with a GPIO joypad.

Note that when both USB and GPIO joypads are connected, the USB joypad input has priority and the GPIO joypad input does not.

### Launch Sequence

The startup procedure is as follows:

1. Prepare a FAT32 formatted SD card.
2. Add [./image](./image) to the root directory of the SD card.
3. Replace [game.pkg](#gamepkg) with the game you wish to boot.
4. Insert the SD card into the RaspberryPi Zero 2W.
5. Connect the USB joypad to the RaspberryPi Zero 2W.
6. Connect the RaspberryPi Zero 2W to the TV with the HDMI cable
7. Connect power to RaspberryPi Zero 2W and turn it ON

## config.sys

Various customizations can be made by placing the `config.sys` file in the root directory of the SD card inserted into the RaspberryPi Zero 2W.

### Joypad Button Assign

```
#--------------------
# JoyPad settings
#--------------------
A BUTTON_1
B BUTTON_0
SELECT BUTTON_8
START BUTTON_9
UP AXIS_1 < 64
DOWN AXIS_1 > 192
LEFT AXIS_0 < 64
RIGHT AXIS_0 > 192
```

Specification:

```
# Button Settings
key_name △ BUTTON_{0-31}

# AXIS Settings
key_name △ AXIS_{0-1} △ {<|>} △ {0-255}
```

`key_name`:

- `A` A button
- `B` B button
- `START` START button
- `SELECT` SELECT button
- `UP` Up cursor
- `DOWN` Down cursor
- `LEFT` Left cursor
- `RIGHT` Right cursor

You can also assign `BUTTON_` to a cursor or `AXIS_` to a button.

You can use [tools/joypad](./tools/joypad/) can be used to check the button contents of your USB joypad.

## game.pkg

game.pkg is a VGS-Zero game executable file that can be generated with the [makepkg command](./tools/makepkg/).

```
makepkg  -o /path/to/output.pkg
         -r /path/to/game.rom
        [-b /path/to/bgm.dat]
        [-s /path/to/se.dat]
```

- [game.rom](#gamerom): Program and image data
- [bgm.dat](#bgmdat): BGM data
- [se.dat](#sedat): Sound effect data

The maximum size of game.pkg is **16MB (128Mbits)**.

### game.rom

game.rom is a ROM data set in 8KB units that is loaded into the [ROM bank](#cpu-memory-map) and can be generated with the [makerom command](./tools/makerom/) in the toolchain.

```
usage: makerom output input1 input2 ... input256
```

- 8KB (64KBit) = 1 bank
- game.rom can store up to 256 banks (16MBit)
- If the input file exceeds 8KB, it is automatically divided into multiple banks and stored in game.rom.
- If the input file is not divisible by 8KB, padding data is automatically inserted.
- The bank number is determined by the order in which the input files are specified (the first 8KB of the first file specified is bank 0).
- When VGS-Zero starts, banks 0 to 3 are loaded into ROM Bank 0 to 3 of the [CPU Memory Map](#cpu-memory-map).
- Bank 0 must be program code.

### bgm.dat

bgm.dat is a data set containing one or more songs and can be generated by the [makebgm command](./tools/makebgm/).

Music data supports the following two data formats:

- BGM data in VGS; [Video Game Sound format](./VGSBGM.md) compiled by [vgsmml command](./tools/vgsmml)
- BGM data in [NSF format](#nsf)

The VGS-Zero automatically identifies the type of BGM data that has been [Playback instruction (0xE0)](#play-bgm) in the Z80 program.

#### (Compile MML)

```
usage: vgsmml /path/to/file.mml /path/to/file.bgm
```

- [MML of Touhou BGM on VGS](https://github.com/suzukiplan/tohovgs-cli/tree/master/mml) is available for all songs, so you may find it useful for practical use.
- MML files can be playback with the toolchain [vgsplay command](./tools/vgsplay) in the toolchain on your PC.

#### (NSF)

- NSF; NES Sound Format background music data can be created using a DAW; Digital Audio Workstation that supports the NSF format, such as [FamiStudio](https://famistudio.org/)
- Please check [example/15_nsf](./example/15_nsf) for details. 
- For extended sound sources, only VRC6 is supported (VRC7, FME7, FDS, N106, and MMC5 are not supported).
- Reference article: https://note.com/suzukiplan/n/n94ea503ff2c8
- NSF only supports playback of the default track (specified by the 8th byte of the NSF header)
  - If you wish to use NSF data in multi-track format, incorporate multiple identical NSF files with rewritten default tracks into bgm.dat.

#### (Make bgm.dat)

```
makebgm bgm.dat song1.bgm [song2.bgm [song3.bgm...]]
```

Up to 256 BGM or NSF files can be specified.

### se.dat

se.dat is a sound effect data set that can be generated with the [makese command](./tools/makese/).

```
makese se.dat se1.wav [se2.wav [se3.wav...]]
```

The .wav file that can be specified to the makese command must be in the following format:

- Uncompressed RIFF format
- Sampling rate: 44100Hz
- Bit rate: 16bits
- Number of channels: 1 (monaural)

Up to 256 .wav files may be specified.

## Game Development Tools

This section contains information on the tools necessary for VGS-Zero game development.


### Recommended Game Development Tools

The table below lists recommended development tools:

| Name | Type | Information |
|:-----|:-----|:------------|
| [Ubuntu Desktop](https://jp.ubuntu.com/download)| OS | All tools listed in this table can also run on Ubuntu |
| [Visual Studio Code](https://code.visualstudio.com/download) | Coding | Writing programs, MMLs, scripts, etc. |
| [SDCC](https://sdcc.sourceforge.net/) | C compiler | Recommended for use when developing games in C<br>(but only version 4.1.0 can work) |
| [aseprite](https://aseprite.org/) | Graphics Editor | Graphics editor supporting 256-color Bitmap format |
| [Tiled Map Editor](https://www.mapeditor.org) | Map Editor | Examples of Use: [example/08_map-scroll](./example/08_map-scroll/) |
| [Jfxr](https://github.com/ttencate/jfxr) | Sound Effects Editor | Creating game sound effects in the browser |
| [FamiStudio](https://famistudio.org/) | BGM Editor | DAW that can produce background music in [NSF format](#nsf) |

With the above tools, you can develop all the programs and assets (graphics, sound effects, music) needed for your game, and all the tools are free to use. All of the tools are free to use. Some of them are paid, but you can use them for free if you download and build the source code yourself.

> _It does not necessarily mean that only the above tools can be used for development._

### Official Toolchain

The tools provided in this repository are as follows:

| Name | Path | Type | Information |
|:-----|:-----|:-----|:------------|
| vgs0 | [./src/sdl2](./src/sdl2/) | Emulator | VGS-Zero emulator & debugger for PC (Linux, macOS) |
| vgsasm | [./tools/vgsasm](./tools/vgsasm/) | CLI | Z80 Assembler |
| bmp2chr | [./tools/bmp2chr](./tools/bmp2chr/) | CLI | Convert 256-color Bitmap file to [character-pattern format](#character-pattern-table) |
| csv2bin | [./tools/csv2bin](./tools/csv2bin/) | CLI | [Tiled Map Editor](https://www.mapeditor.org) csv to binary format |
| makepkg | [./tools/makepkg](./tools/makepkg/) | CLI | Generate [game.pkg](#gamepkg) |
| makerom | [./tools/makerom](./tools/makerom/) | CLI | Generate [game.rom](#gamerom) |
| makese | [./tools/makese](./tools/makese/) | CLI | Generate [se.dat](#sedat) |
| makebgm | [./tools/makebgm](./tools/makebgm/) | CLI | Generate [bgm.dat](#bgmdat) |
| vgsmml | [./tools/vgsmml](./tools/vgsmml/) | CLI | [MML](#compile-mml) Compiler |
| vgsplay | [./tools/vgsplay](./tools/vgsplay/) | CLI | Playback [MML](#compile-mml) |
| joypad | [./tools/joypad](./tools/joypad/) | RPi | Test tool for USB joypad |


## Programming Guide

Please refer to the following repository for initial projects when developing a game.

[https://github.com/suzukiplan/vgszero-empty-project](https://github.com/suzukiplan/vgszero-empty-project)

### Programming Language

- VGS-Zero games can be written in Z80 assembly language or C.
  - Z80: [./example/01_hello-asm](./example/01_hello-asm)
  - C: [./example/01_hello](./example/01_hello)
- If written in Z80:
  - Recommended assembler: [vgsasm](./tools/vgsasm/)
  - Any Z80-compatible assembler can be used.
- If written in C:
  - [SDCC (Small Device C Compiler)](https://sdcc.sourceforge.net/) can be used as a cross-compiler
  - VGS-Zero supports SDCC **version 4.1.0 only**.
    - This is different from the default installation version in `brew`, `apt`, etc.
    - Download and extract sdcc-4.1.0 from [official download site](https://sourceforge.net/projects/sdcc/files/) for your PC model and cut the `PATH` environment variable.
    - macOS(x64): [https://sourceforge.net/projects/sdcc/files/sdcc-macos-amd64/4.1.0/](https://sourceforge.net/projects/sdcc/files/sdcc-macos-amd64/4.1.0/)
    - Linux(x64): [https://sourceforge.net/projects/sdcc/files/sdcc-linux-amd64/4.1.0/](https://sourceforge.net/projects/sdcc/files/sdcc-linux-amd64/4.1.0/)
    - Do not use [fatal bug](https://github.com/suzukiplan/vgszero/issues/8) in versions 4.2 - 4.4.
  - Standard libraries are not available.
  - [vgs0.lib](./lib/sdcc/) can be used.

### API Manual for Programming Language C

See the [vgs0lib.h](./lib/sdcc/vgs0lib.h) implementation.

Although deprecated because it would be quicker to look at the implementation, you can view the manual in HTML format by running the following command:

```
sudo apt install doxygen build-essential
git clone https://github.com/suzukiplan/vgszero
cd vgszero/lib/sdcc
make doc
open doc/html/index.html
```

### Create Sound Data

- Create [music data](#bgmdat) in VGS Music Macro Language (MML) or NSF format (e.g. [FamiStudio](https://famistudio.org/))
- Create [sound effect data](#sedat) as 44100Hz 16bits 1ch (mono) wav files.

### Joypad Recommended Usage

Games developed for VGS-Zero are intended to be played by game users **without** reading the documentation as much as possible, so the buttons on the joypad are fixed to [a simple 8-button system D-PAD, A/B, Select/Start](#joypad).

- D-PAD usage
  - Use it to move the character or cursor.
- B button usage
  - These buttons are suitable for operations that are expected to be **often tapped (repeatedly tapped)** by the user.
  - Examples of use: shooting shots, plumber firing fireballs, dash by pressing and holding cursor input, etc.
  - It is preferable to use the B button for canceling command operations such as RPG.
- A button usage
  - These buttons are suitable for operations where the user is expected to **carefully tap**.
  - Examples of use: shooting bombers, jumps, sniping shots, etc.
  - It is preferable to use the A button to make decisions for RPG and other command operations.
- START button usage
  - Buttons that are expected to be pressed during system operations.
  - Examples: game start, pause, open command, etc.
- SELECT button usage
  - It is not a button that is expected to be used very often, so it is best to avoid its active use.
  - Example of use: coin drop in arcade-like games, etc.

### How to Debug

- For debugging, an SDL2 version emulator running on a PC (Linux or macOS) ([./src/sdl2](./src/sdl2)) running on a PC (Linux or macOS).
- The SDL2 version of the emulator breaks when it detects a NOP instruction and displays a register and memory dump [debug function](./src/sdl2#debug-mode)

### CPU Memory Map

The memory map visible from the main program (Z80) is as follows:

| CPU address | Map |
| :---------: | :-- |
| 0x0000 ~ 0x1FFF | ROM Bank 0 |
| 0x2000 ~ 0x3FFF | ROM Bank 1 |
| 0x4000 ~ 0x5FFF | ROM Bank 2 |
| 0x6000 ~ 0x7FFF | ROM Bank 3 |
| 0x8000 ~ 0x9FFF | VRAM |
| 0xA000 ~ 0xBFFF | Extra RAM Bank |
| 0xC000 ~ 0xFFFF | Main RAM (16KB) |

- The ROM data of the program is divided into 8KB blocks and can have up to 256 banks.
- After power-on or reset, ROM Bank is set to 0 to 3, and [bank-switch](#bank-switch) is available via I/O on ports B0 to B3.
- The stack area is used from 0xFFFE toward 0xC000
- When using global variables, use them in order from 0xC000, and be careful not to destroy the stack when programming.

### RAM Scheme

The RAM in VGS-Zero can be broadly classified into three types of RAM compartments:

1. VRAM (0x8000 ~ 0x9FFF) = 8KB
2. Extra RAM Bank (0xA000 ~ 0xBFFF) = 8KB x 256 Banks (2MB)
3. Main RAM (0xC000 ~ 0xFFFF) = 16KB

VRAM is a memory area used for graphics display and control functions such as [Name Table](#name-table), [Attribute Table](#attribute), [OAM](#oam), [Palette](#palette) and VDP registers. VDP registers and other functions related to graphics display and control.

And Main RAM is a memory section used to hold data such as variables (0xC000~) and stack (~0xFFFF).

The Extra RAM Bank is a slightly special memory block unique to VGS-Zero that can be used as a [Character Pattern Table](#character-pattern-table) for VRAM (similar to TMS9918A). It can also be used for __other purposes__.

VGS-Video can use data in ROM directly as character patterns by using [DPM; Direct Pattern Mapping](#direct-pattern-mapping) or [OAM Bank](#oam-bank), This eliminates the need to expand the character pattern into RAM (VRAM).

If there is no need to expand character patterns into RAM (VRAM), the Extra RAM Bank can be used as a vast data area for roguelike RPG map data, for example.

### VRAM Memory Map

|   CPU address   |  VRAM address   | Map |
| :-------------: | :-------------: | :-- |
| 0x8000 ~ 0x83FF | 0x0000 ~ 0x03FF | [BG](#bg) [Name Table](#name-table) (32 x 32) |
| 0x8400 ~ 0x87FF | 0x0400 ~ 0x07FF | [BG](#bg) [Attribute](#attribute) Table (32 x 32) |
| 0x8800 ~ 0x8BFF | 0x0800 ~ 0x0BFF | [FG](#fg) [Name Table](#name-table) (32 x 32) |
| 0x8C00 ~ 0x8FFF | 0x0C00 ~ 0x0FFF | [FG](#fg) [Attribute](#attribute) Table (32 x 32) |
| 0x9000 ~ 0x97FF | 0x1000 ~ 0x17FF | [OAM](#oam); Object Attribute Memory (8 x 256) |
| 0x9800 ~ 0x99FF | 0x1800 ~ 0x19FF | [Palette](#palette) Table (2 x 16 x 16) |
| 0x9A00 ~ 0x9DFF | 0x1A00 ~ 0x1DFF | [OAM16](#oam16) |
| 0x9F00          | 0x1F00	        | Register #0: Vertical [Scanline Counter](#scanline-counter) (read only) |
| 0x9F01          | 0x1F01          | Register #1: Horizontal [Scanline Counter](#scanline-counter) (read only) |
| 0x9F02          | 0x1F02          | Register #2: [BG](#bg) [Scroll](#hardware-scroll) X |
| 0x9F03          | 0x1F03          | Register #3: [BG](#bg) [Scroll](#hardware-scroll) Y |
| 0x9F04          | 0x1F04          | Register #4: [FG](#fg) [Scroll](#hardware-scroll) X |
| 0x9F05          | 0x1F05          | Register #5: [FG](#fg) [Scroll](#hardware-scroll) Y |
| 0x9F06          | 0x1F06          | Register #6: IRQ scanline position (NOTE: 0 is disable) |
| 0x9F07          | 0x1F07          | Register #7: [Status](#vdp-status) (read only) |
| 0x9F08          | 0x1F08          | Register #8: [Direct Pattern Maaping](#direct-pattern-mapping) for [BG](#bg) |
| 0x9F09          | 0x1F09          | Register #9: [Direct Pattern Maaping](#direct-pattern-mapping) for [FG](#fg) |
| 0x9F0A          | 0x1F0A          | Register #10: [Direct Pattern Maaping](#direct-pattern-mapping) for [Sprite](#sprite) |
| 0x9F0B          | 0x1F0B          | Register #11: [1024 pattern mode setting for BG/FG](#1024-patterns-mode) |
| 0xA000 ~ $BFFF  | 0x2000 ~ 0x3FFF | [Character Pattern Table](#character-pattern-table) (32 x 256) |

Unlike general VDP, access to VRAM can be easily performed by load/store to CPU address (e.g., LD instruction).

#### (BG)

- BG (Background Graphics) is the basic background image
- It is displayed behind [sprite](#sprite) and [FG](#fg).
- It is intended to be used as the background graphics for games.
- Can be displayed by specifying [character-pattern-table](#character-pattern-table) and [attribute](#attribute) in [name-table](#name-table)
- Transparent color does not exist
- The drawing cannot be hidden by specifying [attribute](#attribute).
- Supports [hardware-scroll](#hardware-scroll), which is independent of [FG](#fg).

#### (FG)

- FG (Foreground Graphics) is the foremost image displayed
- It is displayed in front of the [BG](#bg) and [sprite](#sprite).
- It is intended to be used for displaying game scores, message windows, etc.
- Can be displayed by specifying [character-pattern-table](#character-pattern-table) and [attribute](#attribute) in [name-table](#name-table)
- Color number 0 in [palette](#palette) is transparent color.
- [Attribute](#attribute) can be used to hide the drawing, which is set to `hidden` by default.
- Supports [hardware-scroll](#hardware-scroll), which is independent of [BG](#bg).

#### (Sprite)

- Sprite is an image of a character moving around on the screen
- It is displayed on the front of [BG](#bg) & the back of [FG](#fg).
- Intended for use in game character display
- Up to 256 images can be displayed simultaneously.
- Up to 256 characters can be displayed at the same time.
- The drawing can be hidden by specifying [attribute](#attribute).
- The default size is 1x1 pattern (8x8 pixels), but a maximum of 16x16 pattern (128x128 pixels) can be displayed as a single sprite (see the description of `widthMinus1` and `heightMinus1` in [OAM](#oam) for details)

#### (Name Table)

- VGS-Zero displays graphics by writing 8x8 pixel rectangle [character pattern](#character-pattern-table) numbers to a name table.
- The name table is a two-dimensional array of 32 rows and 32 columns (common to BG/FG).

#### (Attribute)

Attributes are character pattern display attributes common to BG, FG, and sprites.

| Bit-7 | Bit-6 | Bit-5 | Bit-4 | Bit-3 | Bit-2 | Bit-1 | Bit-0 |
| :---: | :---: | :---: | :---: | :---: | :---: | :---: | :---: |
| `VI`  | `LR`  | `UD`  | `PTN` | `P3`  | `P2`  | `P1`  | `P0`  |

- `VI`: `0` = hidden, `1` = shown _(*only for BG, 0 is also shown)_
- `LR`: If `1`, left-right flip is shown.
- `UD`: If `1`, it is displayed upside down.
- `PTN`: If `1`, the bank number will +1 when [Direct Pattern Mapping](#direct-pattern-mapping) is enabled.
- `P0~P3`: [palette](#palette) number (0 ~ 15).

#### (Palette)

- VGS-Zero allows up to 16 palettes
- Each palette can contain 16 colors in RGB555 format
- For FG and sprites, color number 0 is the transparent color

#### (OAM)

OAM is a structure with the following elements:

1. display coordinates of sprite.
2. [Character pattern](#character-pattern-table) number.
3. [Attribute](#attribute)
4. [Size](#oam-pattern-size)
5. [Bank number by OAM](#oam-bank)

```c
struct OAM {
    unsigned char y;
    unsigned char x;
    unsigned char pattern;
    unsigned char attribute;
    unsigned char heightMinus1;
    unsigned char widthMinus1;
    unsigned char bank;
    unsigned char reserved;
} oam[256];
```

VGS-Zero can display up to 256 sprites simultaneously, with no upper limit to the number of sprites displayed horizontally.

#### (OAM Pattern Size)

The `widthMinus1` and `heightMinus1` of [OAM](#oam) can be specified in the range of 0 to 15, and a value of 1 or more will display multiple [character-patterns](#character-pattern-table) side by side.

[character-pattern-table](#character-pattern-table) numbers are incremented by +1 in the horizontal direction and +16 (+0x10) in the vertical direction.

For example, if `widthMinus1` is 2 and `heightMinus` is 3, the [character-pattern](#character-pattern-table) group in the table below is displayed as one sprite:

|`\`|0|1|2|
|:-:|:-:|:-:|:-:|
|0|pattern+0x00|pattern+0x01|pattern+0x02|
|1|pattern+0x10|pattern+0x11|pattern+0x12|
|2|pattern+0x20|pattern+0x21|pattern+0x22|
|3|pattern+0x30|pattern+0x31|pattern+0x32|

#### (OAM Bank)

If `bank` in [OAM](#oam) is 0, the character pattern of the sprite is taken from the [character-pattern](#character-pattern-table) in VRAM or the bank specified by [Direct Pattern Mapping](#direct-pattern-mapping).

If a value greater than or equal to 1 is specified, then the bank number of the specified value is used as the character pattern for the OAM.

Configuration priority:

1. OAM Bank **(highest priority)**
2. Direct Pattern Mapping (#direct-pattern-mapping)
3. [Character Pattern](#character-pattern-table) on VRAM

By using OAM Bank, a different bank character pattern can be used for each OAM.

#### (OAM16)

OAM16 is an area where the sprite coordinates can be 16-bit values.

```c
struct OAM16 {
    unsigned short y;
    unsigned short x;
} oam16[256];
```

Valid for non-zero x or y values in OAM16.

By using this function, sprites larger than 24px can be clipped.

| OAM | OAM16 |
|:-:|:-:|
|![oam](./example/17_clip/preview2.png)|![oam16](./example/17_clip/preview1.png)|

For details on how to use [example/17_clip](./example/17_clip/) for details.

#### (Scanline Counter)

- The scanline counter is a read-only VDP register that can identify the pixel rendering position of the VDP
- Vertical is `0x9F00` and horizontal is `0x9F01`.
- By waiting for the vertical value, processes such as raster [scrolling](#hardware-scroll) can be implemented **without interrupts**.
- Horizontal values are not useful because of the high speed of switching.

#### (Hardware Scroll)

- [BG](#bg) allows you to specify `0x9F02` as X-coordinate and `0x9F03` as Y-coordinate starting point.
- [FG](#fg): `0x9F04` with X-coordinate and `0x9F05` with Y-coordinate starting point.
- You can also get the current scroll position by reading `0x9F02` ~ `0x9F05`.


#### (VDP Status)

| Bit-7 | Bit-6 | Bit-5 | Bit-4 | Bit-3 | Bit-2 | Bit-1 | Bit-0 |
| :---: | :---: | :---: | :---: | :---: | :---: | :---: | :---: |
|  BL   |   -   |   -   |   -   |   -   |   -   |   -   |   -   |

- BL: 1 = start vblank

The BL flag is set at the timing when the drawing of the 192nd line of the visible area (the 200th line of the scan line) is completed.

![bl_timing.png](./bl_timing.png)

NOTE: Status register always reset after read.

#### (Direct Pattern Mapping)

Normally, [BG](#bg), [FG](#fg), and [Sprite](#sprite) refer to a common [character-pattern-table], but by writing a value other than **0** to 0x9F08, 0x9F09, and/or 0x9F0A, the ROM bank corresponding to that value can be used as the respective [character-pattern-table](#character-pattern-table).

- 0x9F08: DPM for [BG](#bg)
- 0x9F09: DPM for [FG](#fg)
- 0x9F0A: DPM for [Sprite](#sprite)

```z80
LD HL, 0x9F08
LD (HL), 0x10   # BG = Bank 16
INC HL
LD (HL), 0x11   # FG = Bank 17
INC HL
LD (HL), 0x12   # Sprite = Bank 18
```

> If you want to do bank-switching animations, you can save CPU resources by using DPM rather than switching [character-pattern-table](#character-pattern-table) with [DMA](#rom-to-character-dma).

#### (1024 Patterns Mode)

The number of patterns available for BG/FG can be expanded to 1024 by setting 0x9F0B.

> Note that the [DPM](#direct-pattern-mapping) setting in BG/FG is required to use this function.

| Bit-7 | Bit-6 | Bit-5 | Bit-4 | Bit-3 | Bit-2 | Bit-1 | Bit-0 |
| :---: | :---: | :---: | :---: | :---: | :---: | :---: | :---: |
|   -   |   -   |   -   |   -   |   -   |   -   | `F1k` | `B1k` |

- `F1k` Number of FG patterns (0: 256, 1: 1024)
- `B1k` Number of BG patterns (0: 256, 1: 1024)

If `B1k` and/or `F1k` are set, then `row÷8 + DPM` in [name-table](#name-table) is the pattern bank number to be applied.

- Lines 0〜7: Pattern Bank = DPM + 0
- Lines 8〜15: Pattern Bank = DPM + 1
- Lines 16〜23: Pattern Bank = DPM + 2
- Lines 24〜31: Pattern Bank = DPM + 3

By entering a 256x256 pixel `.bmp` file into [bmp2chr](./tools/bmp2chr/), you can easily generate 4 banksets of chr data for this mode by entering a 256x256 pixel `.bmp` file.

> For more information, see [./example/14_1024ptn](./example/14_1024ptn/)

#### (Character Pattern Table)

- The character pattern table can contain up to 256 character patterns of 8x8 pixel
- The size of one character is 32 bytes
- The total table size is 32 x 256 = 8192 bytes, exactly the size of a bank
- A high-speed DMA function is provided to transfer the contents of a specific bank to the character pattern table

The bit layout of the character pattern table is as follows:

| px0 | px1 | px2 | px3 | px4 | px5 | px6 | px7 | Line number |
| :-: | :-: | :-: | :-: | :-: | :-: | :-: | :-: | :---------- |
| H00 | L00 | H01 | L01 | H02 | L02 | H03 | L03 | Line 0      |
| H04 | L04 | H05 | L05 | H06 | L06 | H07 | L07 | Line 1      |
| H08 | L08 | H09 | L09 | H10 | L10 | H11 | L11 | Line 2      |
| H12 | L12 | H13 | L13 | H14 | L14 | H15 | L15 | Line 3      |
| H16 | L16 | H17 | L17 | H18 | L18 | H19 | L19 | Line 4      |
| H20 | L20 | H21 | L21 | H22 | L22 | H23 | L23 | Line 5      |
| H24 | L24 | H25 | L25 | H26 | L26 | H27 | L27 | Line 6      |
| H28 | L28 | H29 | L29 | H30 | L30 | H31 | L31 | Line 7      |

- `Hxx` : Upper 4 bits (0 ~ 15 = color number) _*xx indicates byte position._
- `Lxx` : Lower 4 bits (0 ~ 15 = color number) _*xx is byte position._
- For FG and sprites, color number 0 is always a transparent color.
- The palette number to be used is specified in [attribute](#attribute)

The memory area of the Character Pattern Table (0xA000 to 0xBFFF) can be made equivalent to 8KB RAM by setting all of [BG](#bg), [FG](#fg), and [sprite](#sprite) to [Direct Pattern Mapping](#direct-pattern-mapping), the area can be set to be equivalent to 8KB of RAM. Furthermore, since this area supports high-speed bank loading by DMA, it may be suitable as a destination area for large map data (up to 64x128 chips for 1 byte per chip!) in shooting games and RPGs.

### I/O Map

|   Port    |  I  |  O  | Description  |
| :-------: | :-: | :-: | :----------- |
|   0xA0    |  o  |  -  | [Joypad](#joypad-1) |
|   0xB0    |  o  |  o  | [ROM Bank](#bank-switch) 0 (default: 0x00) |
|   0xB1    |  o  |  o  | [ROM Bank](#bank-switch) 1 (default: 0x01) |
|   0xB2    |  o  |  o  | [ROM Bank](#bank-switch) 2 (default: 0x02) |
|   0xB3    |  o  |  o  | [ROM Bank](#bank-switch) 3 (default: 0x03) |
|   0xB4    |  o  |  o  | [Extra RAM Bank](#extra-ram-bank) (default: 0x00) |
|   0xB5    |  -  |  o  | [Duplicate Extra RAM Bank](#duplicate-extra-ram-bank)|
|   0xC0    |  -  |  o  | [ROM to Character DMA](#rom-to-character-dma) |
|   0xC1    |  -  |  o  | [ROM to Memory DMA](#rom-to-memory-dma) |
|   0xC2    |  -  |  o  | [memset DMA](#memset-dma) |
|   0xC3    |  -  |  o  | [memcpy DMA](#memcpy-dma) |
|   0xC4    |  o  |  -  | [Collision Detection](#collision-detection) |
|   0xC5    |  -  |  o  | [Multiplication, division, and remainder](hardware-calculation) |
|   0xC6    |  -  |  o  | [Hardware sin table](#hardware-sin-table) |
|   0xC7    |  -  |  o  | [Hardware cos table](#hardware-cos-table) |
|   0xC8    |  o  |  -  | [Hardware atan2 table](#hardware-atan2-table) |
|   0xC9    |  o  |  o  | [Hardware Random (8-bits)](#hardware-random) |
|   0xCA    |  o  |  o  | [Hardware Random (16-bits)](#hardware-random) |
|   0xCB    |  -  |  o  | [Perlin noise seeding](#hardware-perlin-noise) |
|   0xCC    |  -  |  o  | [Set X-coordinate scale for perlin noise](#hardware-perlin-noise) |
|   0xCD    |  -  |  o  | [Set Y-coordinate scale for perlin noise](#hardware-perlin-noise) |
|   0xCE    |  o  |  -  | [Get perlin noise](#hardware-perlin-noise) |
|   0xCF    |  o  |  -  | [Get perlin noise (with octave)](#hardware-perlin-noise) |
|   0xDA    |  o  |  o  | [Save / Load](#save-data) |
|   0xE0    |  -  |  o  | [Playback BGM](#play-bgm) |
|   0xE1    |  -  |  o  | [Pause](#pause-bgm), [Resume](#resume-bgm) or [Fadeout](#fadeout-bgm) BGM|
|   0xF0    |  -  |  o  | Play Sound Effect |
|   0xF1    |  -  |  o  | Interrupted Sound Effect |
|   0xF2    |  -  |  o  | Check if sound effects are playing. |

#### (JoyPad)

```z80
IN A, (0xA0)
```

| Bit-7 | Bit-6 | Bit-5 | Bit-4 | Bit-3 | Bit-2 | Bit-1 | Bit-0 |
| :---: | :---: | :---: | :---: | :---: | :---: | :---: | :---: |
| `Up`  | `Down` | `Left` | `Right` | `Start` | `Select` | `A` | `B` |

- 0: Entered
- 1: Not entered

#### (Bank Switch)

```z80
# Read current bank of ROM Bank 0
IN A, (0xB0)

# Switch ROM Bank 1 to No.17
LD A, 0x11
OUT (0xB1), A
```

#### (Extra RAM Bank)

By setting port number 0xB4 to OUT, the Extra RAM Bank (0xA000 to 0xBFFF = RAM area (8KB) of [Character Pattern Table](#character-pattern-table)) can be switched to use up to 2MB (8KB x 256) RAM.

```z80
# Read Current Extra RAM Bank
IN A, (0xB4)

# Switch Extra RAM Bank to No.3
LD A, 0x03
OUT (0xB4), A
```

#### (Duplicate Extra RAM Bank)

The contents of the current Extra RAM Bank can be duplicated to another Extra RAM Bank by setting port number 0xB5 to OUT.

```z80
; 現在の Extra RAM Bank = 0x00
XOR A
OUT (0xB4), A

; 0x00 の内容を 0x03 へ複製
LD A, 0x03
OUT (0xB5), A
```

#### (ROM to Character DMA)

By setting port number 0xC0 to OUT, the contents of a specific bank can be DMA-transferred to the [Character Pattern Table](#character-pattern-table) in the [VRAM](#vram-memory-map).

```z80
# Transfer bank number = 0x22 to character pattern table
LD A, 0x22
OUT (0xC0), A
```

#### (ROM to Memory DMA)

By setting port number 0xC1 to OUT, the contents of a specific size of a specific bank can be DMA transferred to an arbitrary address.

This instruction sets BC, DE, and HL to the following:

- BC: Offset in the source bank (0x0000 to 0x1FFF)
- DE: Transfer size (8192 - BC or less)
- HL: Destination address

The following example shows a DMA transfer of 512 bytes from 0x1234 of bank number 0x23 to 0xCE00 (RAM):

```z80
LD A, 0x23
LD BC, 0x1234
LD DE, 512
LD HL, 0xCE00
OUT (0xC1), A
```

#### (memset DMA)

```z80
LD BC, 0xC000   # Forwarding address
LD HL, 0x2000   # Number of bytes transferred
LD A,  0xFF     # Value to be transferred
OUT (0xC2), A   # Execute memset
```

#### (memcpy DMA)

```z80
LD BC, 0xC000   # Forwarding address (RAM)
LD DE, 0x6000   # Source address (ROM Bank 3)
LD HL, 0x2000   # Number of bytes transferred (8KB)
OUT (0xC3), A   # Execute memcpy (*Values written are ignored.)
```

#### (Collision Detection)

The following 8 bytes structure is stored in the HL and 0xC4 is set to IN to determine if a hit has occurred.

```c
struct rect {
    uint8_t x;      // X-coordinate
    uint8_t y;      // Y-coordinate
    uint8_t width;  // width
    uint8_t height; // height
} chr[2];           // For 2 characters of them（8bytes）
```

```z80
LD HL, 0xC000   # First address of the structure
IN A, (0xC4)    # Perform collision determination
AND A           # Zero check
JNZ DETECT_HIT  # Collision Detection
JZ NOT_HIT      # Collision Not Detection
```

#### (Hardware Calculation)

The OUT of 0xC5 enables high-speed execution of multiplication, division, and remainder operations, which Z80 is not good at.

```
# 8bit arithmetic instruction (unsigned)
OUT (0xC5), 0x00 ... HL = H * L
OUT (0xC5), 0x01 ... HL = H / L
OUT (0xC5), 0x02 ... HL = H % L

# 8bit arithmetic instruction (signed)
OUT (0xC5), 0x40 ... HL = H * L
OUT (0xC5), 0x41 ... HL = H / L

# 16bit arithmetic instruction (unsigned)
OUT (0xC5), 0x80 ... HL = HL * C (HL: operation-result mod 65536)
OUT (0xC5), 0x81 ... HL = HL / C
OUT (0xC5), 0x82 ... HL = HL % C

# 16bit arithmetic instruction (Signed)
OUT (0xC5), 0xC0 ... HL = HL * C (HL: operation-result mod 65536)
OUT (0xC5), 0xC1 ... HL = HL / C
```

NOTE: HL is 0xFFFF if division by zero is performed.

#### (Hardware SIN table)

```z80
LD A, 123      # Specify the table element number to be sought in A
OUT (0xC6), A  # A = sin(A × π ÷ 128.0)
```

#### (Hardware COS table)

```z80
LD A, 123      # Specify the table element number to be sought in A
OUT (0xC7), A  # A = cos(A × π ÷ 128.0)
```

#### (Hardware ATAN2 table)

```z80
LD H, <<<y1 - y2>>>   # Set the difference of Y-coordinates to H
LD L, <<<x1 - x2>>>   # Set the difference of X-coordinates to L
IN A, (0xC8)          # Find the angles (x1, y1) and (x2, y2) in A
```

#### (Hardware Random)

You can obtain convergence-guaranteed random numbers.

> Guaranteed convergence means that if 8 bits are called 256 times, the numbers 0 to 255 will always appear once, and if 16 bits are called 65536 times, the numbers 0 to 65535 will always appear once.

```z80
# 8 bits Set random number seed
LD L, 123
LD A, L
OUT (0xC9), A

# Obtain 8 bits random numbers (note that random numbers are also stored in the L)
IN A, (0xC9)

# 16 bits Sets the random number seed (any value specified for OUT is ignored, so anything is OK)
LD HL, 12345
OUT (0xCA), A

# Obtain 16 bits random number (note that random numbers are also stored in the HL)
IN A, (0xCA)
```

#### (Hardware Perlin Noise)

Perlin noise can be obtained.

> For more information, see [./example/13_perlin](./example/13_perlin/).

```z80
# Set random number seed
LD HL, 12345
OUT (0xCB), A

# Set the scale in the x-direction (the smaller the scale, the larger the scale)
LD HL, 123
OUT (0xCC), A

# Set the scale in the Y direction (the smaller the scale, the larger the scale)
LD HL, 456
OUT (0xCD), A

# Get perlin noise
LD HL, 789 # X-coordinate
LD DE, 123 # Y-coordinate
IN A, (0xCE)

# Obtain perlin noise by specifying octave.
LD HL, 789 # X-coordinate
LD DE, 123 # Y-coordinate
LD A, 10   # Octave
IN A, (0xCF)
```

#### (Save Data)

- Save (OUT) and load (IN) can be done with I/O on port 0xDA.
- The save data file name is fixed to `save.dat` in the SD card root directory (current directory for SDL2).
- It is intended to be used as a save function for RPGs and a high score save function for STGs.

Save implementation example:

```z80
LD BC, 0xC000   # Specify the address of the data to be saved (only RAM area can be specified)
LD HL, 0x2000   # Specify size of data to be saved (max 16 KB = 0x4000)
OUT (0xDA), A   # Save (*Written values are ignored, so anything can be saved)
AND 0xFF        # The save result is stored in A
JZ SAVE_SUCCESS # 0 on success
JNZ SAVE_FAILED # On failure, not 0
```

Load implementation example:

```z80
LD BC, 0xC000   # Specify load destination address (only RAM area can be specified)
LD HL, 0x2000   # Specify data size to load (max 16 KB = 0x4000)
IN A, (0xDA)    # Load (*Written values are ignored, so anything is OK)
JZ LOAD_SUCCESS # 0 on success
JNZ LOAD_FAILED # On failure, not 0 (*Load destination is filled with 0x00)
```

RaspberryPi specific notes:

- Saving to the SD card is done asynchronously, so the save will return success even if the SD card is not inserted or in some other incorrect state (at this time, a system message will appear on the screen indicating that the SD card write has failed).
- save.dat is written only when changes are made to the contents of the saved data in order to prevent SD card deterioration.
- The LED lamp will be lit during saving and will blink 3 times if a write failure occurs.
- Powering off the SD card while saving may damage the card.
- The kernel mounts the SD card when save and load are performed and unmounts it automatically when the process is complete

Common notes:

- Even if save.dat is smaller than the size (HL) specified at load time, the load will succeed, and the area where no data exists will be filled with 0x00.
- Loading into the stack area may cause the program to run out of control.
- It is advisable to take into account the possibility that users may be running with saved data from different games

#### (Play BGM)

```z80
LD A, 0x01      # Specify BGM bank number to be played
OUT (0xE0), A   # Start playing background music
```

#### (Pause BGM)

```z80
LD A, 0x00      # Operation ID: Pause
OUT (0xE1), A   # Interruption of background music performance.
```

#### (Resume BGM)

```z80
LD A, 0x01      # Operation ID: Resume
OUT (0xE1), A   # Resume playing background music.
```

#### (Fadeout BGM)

```z80
LD A, 0x02      # Operation ID: Fadeout
OUT (0xE1), A   # Fade out background music performance.
```

#### (Play Sound Effect)

```z80
LD A, 0x01      # Specify the number of the sound effect to be played.
OUT (0xF0), A   # Execute
```

#### (Stop Sound Effect)

```z80
LD A, 0x02      # Specify the number of the sound effect to be stopped.
OUT (0xF1), A   # Execute
```

#### (Check Sound Effect)

```z80
LD A, 0x03      # Specify the number of the sound effect to be checked if it is playing.
OUT (0xF2), A   # Execute (A=0: Stopped, A=1: Playing)
AND 0x01
JNZ EFF03_IS_PILAYING
JZ  EFF03_IS_NOT_PLAYING
```

## How to Sell Your Game

### RaspberryPi Zero 2W

When selling games for VGS-Zero at comic markets (events) or by mail order, the following procedure is recommended:

1. Prepare a micro SD card formatted in FAT32
2. Create a new micro SD card in [./image](./image/) to the root directory of the micro SD card.
3. Replace [game.pkg](#gamepkg)
4. Delete [README](/image/README)
5. Store README.txt (text describing how to play the game)
6. Store micro-SD card in [case](https://www.amazon.co.jp/dp/B08TWR47LV/) etc.
7. Apply game label to the case

Legend of README.txt description:

```
================================================================================
<<<Game Title>>>
<<<Copyright>>>
================================================================================

Thank you very much for purchasing “<<<game title>>>”.
This document describes how to play this game, etc.
Please read it before playing.

(Hardware Required)

- RaspberryPi Zero 2W
- HDMI Cable (mini HDMI Type C → HDMI Type A)
- USB Joypad（D-Pad+A/B+Start/Select）+ Conversion Adapter
- USB power supply
- Television, etc. (with the following conditions)
  - HDMI input support
  - Refresh rate 60Hz
  - Resolution 480x384 pixels or higher
  - Audio output supported

(How to Start-up)

- Insert this product (micro SD card) into RaspberryPi Zero 2W.
- Turn on the power of RaspberryPi Zero 2W

(How to change button assignments)

The joypad button assignments can be customized in config.sys.
The following tools can be used to check what should be set in config.sys.

https://github.com/suzukiplan/vgszero/tree/master/tools/joypad


(How to Play)

<<<Describe how you play your game>>>

(Contact us)

<<<Include Twitter (aka X) account, etc.>>>
```

For mail-order sales, consignment sales at doujin stores may be a good option if the lot size is large, but for small lots (100 or less), Mercari may be a good and easy option in Japan.

No payback (license fee) to SUZUKIPLAN is required for any proceeds from sales.

### Steam

- With [VGS-Zero SDK for Steam](https://github.com/suzukiplan/vgszero-steam) you can easily create Steam (Windows & SteamDeck) versions
- VGS-Zero SDK for Steam requires a subscription to Steamworks and the purchase of App Credits.
- No payback (license fee) to SUZUKIPLAN is required for any revenue generated from the sale of VGS-Zero SDK for Steam.

### Nintendo Switch

_We hope to address this in the future._

## Examples

| Directory | Language | Description |
| :-------- | :------- | :---------- |
| [example/01_hello-asm](./example/01_hello-asm/) | Z80 | Show `HELLO,WORLD!` |
| [example/01_hello](./example/01_hello/) | C | Show `HELLO,WORLD!` |
| [example/02_global](./example/02_global/) | C | Example of Global Variable Usage |
| [example/03_sound](./example/03_sound/) | C | Examples of background music and sound effects |
| [example/04_heavy](./example/04_heavy/) | C | Inspection program to maximize the load on the emulator side |
| [example/05_sprite256](./example/05_sprite256/) | C | Example of displaying and moving 256 sprites |
| [example/06_save](./example/06_save/) | C | Example of [Save Function](#save-data) Usage |
| [example/07_palette](./example/07_palette/) | C | Example using all 16 [palettes](#palette) |
| [example/08_map-scroll](./example/08_map-scroll/) | C | Scrolling map data created with the Tiled Map Editor |
| [example/09_joypad](./example/09_joypad/) | C | Preview joypad input results |
| [example/10_chr720](./example/10_chr720/) | C | Example of displaying a single picture in [Direct Pattern Mapping](#direct-pattern-mapping) |
| [example/11_bigsprite](./example/11_bigsprite/) | C | Example of displaying a huge sprite by specifying `widthMinus1`, `heightMinus1`, and `bank` in [OAM](#oam) |
| [example/12_angle](./example/12_angle) | C | Example of implementing complete self-targeting using [atan2](#hardware-atan2-table) |
| [example/13_perlin](./example/13_perlin) | C | Example usage of [Perlin Noise](#hardware-perlin-noise) |
| [example/14_1024ptn](./example/14_1024ptn) | C | [1024-patterns-mode] (#1024-patterns-mode) usage example |
| [example/15_nsf](./example/15_nsf/) | C | Example usage of [NSF](#nsf) |
| [example/16_ptn-plus1](./example/16_ptn-plus1/) | C | Example usage of `ptn` in [Attribute](#attribute) |
| [example/17_clip](./example/17_clip/) | C| Example usage of [OAM16](#oam16) |

## License

- VGS-Zero is GPLv3 OSS: [LICENSE-VGS0.txt](./LICENSE_VGS0.txt)
- The VGS-Zero distribution image includes a RaspberryPi bootloader: [LICENCE.broadcom](./LICENCE.broadcom)
- VGS-Zero itself includes Circle (GPLv3): [LICENSE-CIRCLE.txt](./LICENSE-CIRCLE.txt)
- VGS-Zero itself includes SUZUKI PLAN - Z80 Emulator (MIT): [LICENSE-Z80.txt](./LICENSE-Z80.txt)
- VGS-Zero itself includes NEZplug (Free Software): [LICENSE-NEZplug.txt](./LICENSE-NEZplug.txt)
- VGS-Zero itself includes Modified NSFPlay (GPLv3): [LICENSE-NSFPlay-alter.txt](./LICENSE-NSFPlay-alter.txt)
- VGS-Zero itself includes KM6502 (Free Software): [LICENSE-km6502.txt](./LICENSE-km6502.txt)
- VGS-Zero Library for Z80 is MIT licensed OSS:　[LICENSE-VGS0LIB.txt](./LICENSE_VGS0LIB.txt)

> The copyright of [game.pkg](#gamepkg) developed by you belongs to you and you are free to use it, including commercial use.
>
> Only VGS-Zero Library for Z80 may be embedded in [game.pkg](#gamepkg), but all other OSS is used on the kernel (VGS-Zero itself) side and does not affect the license of the developed game.
>
> However, if you wish to include it when redistributing [./image/README](./image/README), please check the contents carefully.
