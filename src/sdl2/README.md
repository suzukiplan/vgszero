# VGS0 for SDL2

## How to Build & Execute

### macOS

```bash
# NOTE: XCode CLI Tools required

# install SDL2
brew install sdl2

# download repository
git clone https://github.com/suzukiplan/vgszero

# move to this dir
cd vgszero/src/sdl2

# build
make

# execute
./vgs0 ../../example/01_hello/game.pkg
```

### Linux

```bash
# install GNU Make, GCC and other
sudo apt install build-essential

# install SDL2
sudo apt-get install libsdl2-dev

# install ALSA
sudo apt-get install libasound2
sudo apt-get install libasound2-dev

# download repository
git clone https://github.com/suzukiplan/vgszero

# move to this dir
cd vgszero/src/sdl2

# build
make

# execute
./vgs0 ../../example/01_hello/game.pkg
```

## Usage

```
usage: vgs0 /path/to/game.pkg ....... Specify game package to be used
            [-g { None .............. GPU: Do not use
                | OpenGL ............ GPU: OpenGL <default>
                | Vulkan ............ GPU: Vulkan
                | Metal ............. GPU: Metal
                }]
            [-f] .................... Full Screen Mode
            [-d] .................... Enable Debug Mode (NOP break)
```

## Debug Mode

デバッグモード (`-d`) で実行すると `NOP` 命令を検出したタイミングでレジスタ情報をダンプしてコマンド入力待ち状態になります。

```
2024.05.27 13:17:27 NOP at 0x0021
A :0x01 F :0xA0, B :0x00, C :0x00, D :0x81, E :0x96, H :0x00, L :0x6F
A':0x00 F':0x00, B':0x00, C':0x00, D':0x00, E':0x00, H':0x00, L':0x00
PC:0x0021, SP:0xFFFF, IX:0x0000, IY:0x0000, I :0x00, R :0x5C
ROM BANK: 0x00, 0x01, 0x02, 0x03
RAM BANK: 0x00
SCANLINE: V=200, H=174
  SCROLL: BGX=0, BGY=0, FGX=0, FGY=0
> 
```

コマンドは大文字と小文字を区別しません。

### (Memory Dump Command)

- `M アドレス サイズ` を実行することでメモリの内容をダンプします
- アドレスは 16 進数で指定してください（デフォルト: 0xC000）
- サイズは 10 進数で指定してください（デフォルト: 256）

```
m8170 64
Dump from 0x8170 (64 bytes)
ADDR  +0 +1 +2 +3 +4 +5 +6 +7   +8 +9 +A +B +C +D +E +F  ASCII
8170: 00 00 00 00 00 00 00 00 - 00 00 00 00 00 00 00 00  ................
8180: 00 00 00 00 00 00 00 00 - 00 00 48 45 4C 4C 4F 2C  ..........HELLO,
8190: 57 4F 52 4C 44 21 00 00 - 00 00 00 00 00 00 00 00  WORLD!..........
81A0: 00 00 00 00 00 00 00 00 - 00 00 00 00 00 00 00 00  ................
```

### (Toggle Disassemble Command)

- `D` を入力するとディスアセンブルの 有効 or 無効 をトグルすることができます
- ディスアセンブルを有効にするとプログラムの実行速度が著しく遅くなるため注意してください
- ディスアセンブルしたい極小範囲に `NOP` を仕掛けて区間的に命令の流れをデバッグすることに指摘しています

```
D
```

### (Exit Process Command)

`E` を入力するとプロセスを終了します。

```
E
```


### (Help Command)

`H` または `?` を入力するとコマンドのヘルプを表示します。

```
H
```

### (Other Command)

プログラムを再開します。
