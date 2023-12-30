# Hello, World

- 画面に `Hello, World!` を表示します
- ジョイパッドでハードウェアスクロール機能を用いて8方向に動かすことができます

![preview](preview.png)

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
cd vgszero/example/01_hello
make
```

## ROM structure

```
8KB x 3 banks = 24KB ROM
```

- Bank 0: program
- Bank 1: program
- Bank 2: font.chr

## Program Code

```c
#include "../../lib/sdcc/vgs0lib.h"
#include "palette.h"

void main(void)
{
    // パレットを初期化
    init_palette();

    // Bank 2 を Character Pattern Table ($A000) に転送 (DMA)
    vgs0_dma(2);

    // 画面中央付近 (10,12) に "HELLO,WORLD!" を描画
    vgs0_bg_putstr(10, 12, 0x80, "HELLO,WORLD!");

    while (1) {
        vgs0_wait_vsync();
        uint8_t pad = vgs0_joypad_get();
        if (pad & VGS0_JOYPAD_LE) {
            *VGS0_ADDR_BG_SCROLL_X += 1;
        } else if (pad & VGS0_JOYPAD_RI) {
            *VGS0_ADDR_BG_SCROLL_X -= 1;
        }
        if (pad & VGS0_JOYPAD_UP) {
            *VGS0_ADDR_BG_SCROLL_Y += 1;
        } else if (pad & VGS0_JOYPAD_DW) {
            *VGS0_ADDR_BG_SCROLL_Y -= 1;
        }
    }
}
```

パレット初期化処理（[palette.h](./palette.h)）はツールチェインの [bmp2chr](../../tools/bmp2chr/) で自動生成しています。
