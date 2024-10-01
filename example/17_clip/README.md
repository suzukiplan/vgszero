# Clip Sprite with OAM16

OAM16 を用いてスプライトをクリップして表示する実装例です。

|OAM16|OAM|
|:-:|:-:|
|![preview1](preview1.png)|![preview2](preview2.png)|

- スプライト (128x128) は上下左右に移動できます
- OAM と OAM16 は START ボタンで切り替えることができます

## How to build

### Pre-request

- GNU make and GNU Compiler Collection
  - macOS: install XCODE
  - Linux: `sudo apt install build-essential`
- SDCC version 4.1.0
  - macOS(x64): [https://sourceforge.net/projects/sdcc/files/sdcc-macos-amd64/4.1.0/](https://sourceforge.net/projects/sdcc/files/sdcc-macos-amd64/4.1.0/)
  - Linux(x64): [https://sourceforge.net/projects/sdcc/files/sdcc-linux-amd64/4.1.0/](https://sourceforge.net/projects/sdcc/files/sdcc-linux-amd64/4.1.0/)

### Build

```zsh
git clone https://github.com/suzukiplan/vgszero
cd vgszero/example/17_clip
make
```

## ROM structure

```
8KB x 4 banks = 32KB ROM
```

- Bank 0: program
- Bank 1: program
- Bank 2: font.chr
- Bank 3: sprite.chr
