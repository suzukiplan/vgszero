# Map Scroll (SDCC version)

View and scroll through map data created with the [Tiled Map Editor](https://www.mapeditor.org).

![preview](preview.png)

## How to build

### Pre-request

- GNU make and GNU Compiler Collection
  - macOS: install XCODE
  - Linux: `sudo apt install build-essential`
- SDCC version 4.1.0
  - macOS(x64): [https://sourceforge.net/projects/sdcc/files/sdcc-macos-amd64/4.1.0/](https://sourceforge.net/projects/sdcc/files/sdcc-macos-amd64/4.1.0/)
  - Linux(x64): [https://sourceforge.net/projects/sdcc/files/sdcc-linux-amd64/4.1.0/](https://sourceforge.net/projects/sdcc/files/sdcc-linux-amd64/4.1.0/)

### Map data

- [mapdata.tmx](mapdata.tmx) & [image.tsx](image.tsx) are data that created with the [Tiled Map Editor](https://www.mapeditor.org).

### Build

```zsh
git clone https://github.com/suzukiplan/vgszero
cd vgszero/example/08_map-scroll
make
```

## ROM structure

```
8KB x 4 banks = 32KB ROM
```

- Bank 0: program
- Bank 1: program
- Bank 2: image.chr
- Bank 3: mapdata.bin

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

    // マップの初期状態を描画
    uint8_t scrollCounter = 0;
    uint16_t nextRead = 0x8000 - 32 * 25;
    int8_t nextWrite = 0;
    vgs0_memcpy((uint16_t)VGS0_ADDR_BG, nextRead, 32 * 25);

    // メインループ
    while (1) {
        // VBLANKを待機
        vgs0_wait_vsync();

        // BGを下スクロール
        *VGS0_ADDR_BG_SCROLL_Y -= 1;

        // スクロールカウンタをインクリメント
        scrollCounter++;
        scrollCounter &= 0x07;
        if (0 != scrollCounter) {
            continue;
        }

        // 次のマップを描画
        nextRead -= 32;
        nextRead &= 0x1FFF;
        nextRead |= 0x6000;
        nextWrite--;
        nextWrite &= 0x1F;
        vgs0_memcpy((uint16_t)&VGS0_ADDR_BG->ptn[nextWrite][0], nextRead, 32);
    }
}
```