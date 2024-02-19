# 1024 Patterns Mode

1024 パターンを表示する機能を用いる例です。

![preview.png](preview.png)

カーソルで上下左右にスクロールできます。

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
cd vgszero/example/14_1024ptn
make
```

## ROM structure

```
8KB x 7 banks = 56KB ROM
```

- Bank 0: program
- Bank 1: program
- Bank 2: palette
- Bank 3~6: image.chr
