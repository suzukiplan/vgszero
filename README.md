# [WIP] Video Game System - Zero

![logo](logo.png)

Video Game System - Zero (VGS-Zero) は RaspberryPi Zero 2W のベアメタル環境で動作するゲーム機です。

本リポジトリは、VGS-Zero の本体コード、配布イメージ、SDK、パソコン（Linux または macOS）で動作するエミュレータを提供します。

## WIP status

- problem
  - [x] 限界性能のテストプログラムが RaspberryPi Zero (無印) で動作遅延する問題の対処
    - Zero 2W 専用にしてマルチコア前提で動かせば大丈夫そう
    - Zero 無印のサポートを落とすか緩くサポートするかで悩み中... _But I made a decision._
    - _2W の供給も安定してきたし良いよね（旧Zeroのテストも面倒だし）_
- implementation
  - [x] CPU
  - [x] VDP
  - [x] BGM API
  - [x] Sound Effect API
  - [x] RaspberryPi Zero
  - [x] RaspberryPi Zero 2W
  - [x] 22050Hz 1ch -> 44100Hz 2ch へ変更が必要かも（HDMIの仕様）
  - [x] 44100Hz 2ch -> 44100Hz 1ch にする（モノラルでもイケたので）
  - [x] game.rom, bgm.dat, se.dat を1ファイルに纏めたい (game.pkg)
  - [x] bank switch API (C言語)
  - [x] RaspberryPi Zero のサポートを廃止
  - [x] Z80 のクロックアップ
  - [ ] RaspberryPi Zero 2W で処理遅延していないことの検証
  - [ ] SAVE/LOAD API (Z80)
    - VGS-Zero で RPG を創ったり STG のスコア保存ができたりするようにするため
  - [ ] bmp2chr のサポートフォーマットを増やす（16色bmpの対応は必要）
- examples
  - [x] Hello, World!
  - [x] グローバル変数の使い方
  - [x] BGM再生
  - [x] SE再生
  - [ ] Map Scroll
  - [x] スプライト
  - [x] スプライト・BG・FG全表示しつつ音楽+効果音（限界性能チェック用）
  - [ ] アセンブリ言語実装例（Helloのみ）
- documents
  - [x] Z80: Memory map
  - [x] Z80: I/O map
  - [x] C API Library
  - [x] `game.rom`
  - [x] `bgm.dat`
  - [x] `se.dat`
  - [x] `game.pkg`

## VGS-Zero Feature

- CPU: Z80 16MHz (16,777,216Hz)
  - Z80 アセンブリ言語でプログラムを記述
  - SDCC を用いて C 言語でもプログラミング可能
  - 最大 2MB (8kb × 256) のプログラムとデータ (※音声データを除く)
  - RAM サイズ 16KB (PV16相当!)
  - [セーブ機能](#save-data)に対応
- VDP (映像処理)
  - VRAM サイズ 16KB (TMS9918A 相当!)
  - 解像度: 240x192 ピクセル
  - 32,768 色中 256 色を同時発色可能
  - 8x8 ピクセルの[キャラクタパターン](#character-pattern-table)を最大 256 枚 (8KB) 定義可能
  - [BG](#bg), [FG](#fg) の[ネームテーブル](#name-table)サイズ: 32x32 (256x256 ピクセル)
  - [ハードウェアスクロール](#hardware-scroll)対応
  - 最大 256 枚の[スプライト](#sprite)を表示可能（水平上限なし）
- DMA (ダイレクトメモリアクセス)
  - [特定の ROM バンクの内容をキャラクタパターンテーブルに高速転送が可能](#rom-to-character-dma)
  - [C言語の `memset` に相当する高速 DMA 転送機能を実装](#memset-dma)
  - [C言語の `memcpy` に相当する高速 DMA 転送機能を実装](#memcpy-dma)
- BGM
  - VGS の MML で記述された BGM を再生可能
  - ゲームプログラム (Z80) 側でのサウンドドライバ実装が不要!
  - ゲームプログラム (Z80) 側の RAM (16KB) を専有不要!
  - 本体 ROM (`game.rom`) とは別アセット（`bgm.dat`）
  - 最大 256 曲
- SE (効果音)
  - 22050Hz 16bit 1ch 形式の PCM 音源で効果音を再生
  - ゲームプログラム (Z80) 側でのサウンドドライバ実装が不要!
  - ゲームプログラム (Z80) 側の RAM (16KB) を専有不要
  - 本体 ROM (`game.rom`) とは別アセット（`se.dat`）
  - 最大 256 個

## How to Execute

### on RaspberryPi Zero 2W

1. FAT32 フォーマットされた SD カードを準備
2. SD カードのルートディレクトリに [./image](./image) 以下のファイルをコピー
3. [game.pkg](#gamepkg) を起動対象のゲームに置き換える
4. SD カードを RaspberryPi Zero に挿入
5. RaspberryPi Zero 2W に USB ゲームパッドを接続
6. RaspberryPi Zero 2W とテレビを HDMI ケーブルで接続
7. RaspberryPi Zero 2W に USB で電源を供給

### on PC for Debug (Linux or macOS)

SDL2 版エミュレータ（[./src/sdl2](./src/sdl2)）をビルドして、コマンドラインオプションに起動対象の [game.pkg](#gamepkg) を指定して実行してください。

## Examples

| Directory | Language | Description |
| :-------- | :------- | :---------- |
| [example/01_hello](./example/01_hello/) | C言語 | `HELLO,WORLD!` を表示 |
| [example/02_global](./example/02_global/) | C言語 | グローバル変数の使用例 |
| [example/03_sound](./example/03_sound/) | C言語 | BGM と効果音の使用例 |
| [example/04_heavy](./example/04_heavy/) | C言語 | エミュレータ側の負荷を最大化する検査用プログラム |

## game.pkg

game.pkg は、再配布可能な VGS-Zero のゲーム実行形式で、ツールチェインの [makepkg コマンド](./tools/makepkg/) で生成することができます。

```
makepkg  -o /path/to/output.pkg
         -r /path/to/game.rom
        [-b /path/to/bgm.dat]
        [-s /path/to/se.dat]
```

- [game.rom](#gamerom): プログラムや画像データ
- [bgm.dat](#bgmdat): BGM データ
- [se.dat](#sedat): 効果音データ

なお、game.pkg のサイズは **16MB以下** でなければなりません。

### game.rom

game.rom は [ROMバンク](#cpu-memory-map)に読み込まれる 8KB 単位の ROM データセットで、ツールチェインの [makerom コマンド](./tools/makepkg/) で生成することができます。

```
usage: makerom output input1 input2 ... input256
```

なお、入力ファイルが 8KB で割り切れない場合、パディングデータが自動的に挿入されます。

### bgm.dat

bgm.dat は、本リポジトリのツールチェインで提供している [vgsmml コマンド](./tools/vgsmml) でコンパイルされた1つ以上の楽曲データを纏めたデータセットで、ツールチェインの [makebgm コマンド](./tools/makebgm/) で生成することができます。

#### (Compile MML)

```
usage: vgsmml /path/to/file.mml /path/to/file.bgm
```

- MMLの仕様: [https://github.com/suzukiplan/vgs-mml-compiler/blob/master/MML-ja.md](https://github.com/suzukiplan/vgs-mml-compiler/blob/master/MML-ja.md)
- MML ファイルは、[tohovgs コマンド](https://github.com/suzukiplan/tohovgs-cli) を用いれば PC 上でプレビューできます

#### (Make bgm.dat)

```
makebgm bgm.dat song1.bgm [song2.bgm [song3.bgm...]]
```

BGM ファイルは最大 256 個指定することができます。

### se.dat

se.dat は、効果音のデータセットで、ツールチェインの [makese コマンド](./tools/makese/) で生成することができます。

```
makese se.dat se1.wav [se2.wav [se3.wav...]]
```

makese コマンドに指定できる .wav ファイルは、次の形式でなければなりません:

- 無圧縮 RIFF 形式
- サンプリングレート: 44100Hz
- ビットレート: 16bits
- チャネル数: 1 (モノラル)

.wav ファイルは最大 256 個指定することができます。

## Programming Guide

### Programming Language

- VGS-Zero のゲームは Z80 アセンブリ言語 または C言語 で記述することができます
  - Z80 アセンブリ言語: [./example/01_hello-asm](./example/01_hello-asm)
  - C言語: [./example/01_hello](./example/01_hello)
- C言語で記述する場合:
  - クロスコンパイラに SDCC (Small Device C) が使用できます
  - 標準ライブラリは使用できません
  - [vgs0.lib](./lib/sdcc/) を使用することができます

### API Manual for Programming Language C

[vgs0lib.h](./lib/sdcc/vgs0lib.h) の実装を参照してください。

実装を見た方が手っ取り早いと思われるので非推奨ですが、以下のコマンドを実行すれば HTML 形式のマニュアルを閲覧できます。

```
sudo apt install doxygen build-essential
git clone https://github.com/suzukiplan/vgszero
cd vgszero/lib/sdcc
make doc
open doc/html/index.html
```

### Create Sound Data

- VGS 形式の Music Macro Language (MML) で音楽データを記述できます
- 効果音には 44100Hz 16bits 1ch (モノラル) の wav ファイルを用いることができます

### How to Debug

デバッグには、パソコン（Linux または macOS）上で動作するエミュレータ（[./src/sdl2](./src/sdl2)）を用いると便利です。

### CPU Memory Map

メインプログラム（Z80）から見えるメモリマップは次の通りです。

| CPU address | Map |
| :---------: | :-- |
| 0x0000 ~ 0x1FFF | ROM Bank 0 |
| 0x2000 ~ 0x3FFF | ROM Bank 1 |
| 0x4000 ~ 0x5FFF | ROM Bank 2 |
| 0x6000 ~ 0x7FFF | ROM Bank 3 |
| 0x8000 ~ 0xBFFF | VRAM |
| 0xC000 ~ 0xFFFF | RAM (16KB) |

- プログラムの ROM データは 8KB 区切りになっていて最大 256 個のバンクを持つことができます
- 電源投入またはリセットすると ROM Bank には 0〜3 がセットされ、ポート B0〜B3 の I/O で [バンク切り替え](#bank-switch) ができます
- スタック領域は 0xFFFE から 0xC000 の方向に向かって使われます
- グローバル変数を使用する場合 0xC000 から順番に使い、スタックによる破壊がされないように気をつけてプログラミングしてください

### VRAM Memory Map

|   CPU address   |  VRAM address   | Map |
| :-------------: | :-------------: | :-- |
| 0x8000 ~ 0x83FF | 0x0000 ~ 0x03FF | [BG](#bg) [Name Table](#name-table) (32 x 32) |
| 0x8400 ~ 0x87FF | 0x0400 ~ 0x07FF | [BG](#bg) [Attribute](#attribute) Table (32 x 32) |
| 0x8800 ~ 0x8BFF | 0x0800 ~ 0x0BFF | [FG](#fg) [Name Table](#name-table) (32 x 32) |
| 0x8C00 ~ 0x8FFF | 0x0C00 ~ 0x0FFF | [FG](#fg) [Attribute](#attribute) Table (32 x 32) |
| 0x9000 ~ 0x93FF | 0x1000 ~ 0x13FF | [OAM](#oam); Object Attribute Memory (4 x 256) |
| 0x9400 ~ 0x95FF | 0x1400 ~ 0x15FF | [Palette](#palette) Table (2 x 16 x 16) |
| 0x9600          | 0x1600	        | Register #0: Vertical [Scanline Counter](#scanline-counter) (read only) |
| 0x9601          | 0x1601          | Register #1: Horizontal [Scanline Counter](#scanline-counter) (read only) |
| 0x9602          | 0x1602          | Register #2: [BG](#bg) [Scroll](#hardware-scroll) X |
| 0x9603          | 0x1603          | Register #3: [BG](#bg) [Scroll](#hardware-scroll) Y |
| 0x9604          | 0x1604          | Register #4: [FG](#fg) [Scroll](#hardware-scroll) X |
| 0x9605          | 0x1605          | Register #5: [FG](#fg) [Scroll](#hardware-scroll) Y |
| 0x9606          | 0x1606          | Register #6: IRQ scanline position (NOTE: 0 is disable) |
| 0x9607          | 0x1607          | Register #7: [Status](#vdp-status) (read only) |
| 0xA000 ~ $BFFF  | 0x2000 ~ 0x3FFF | [Character Pattern Table](#character-pattern-table) (32 x 256) |

VRAM へのアクセスは一般的な VDP とは異なり CPU アドレスへのロード・ストア（LD命令等）で簡単に実行できます。

#### (BG)

- BG (Background Graphics) は、基本となる背景映像です
- [スプライト](#sprite) と [FG](#fg) の背面に表示されます
- ゲームの背景映像として利用することを想定しています
- [ネームテーブル](#name-table) に[キャラクタ番号](#character-pattern-table)と[属性](#attribute)を指定することで表示できます
- 透明色が存在しません
- [属性](#attribute)の指定で描画を非表示（hidden）にすることができません
- [FG](#fg) とは独立した [ハードウェアスクロール](#hardware-scroll) に対応しています

#### (FG)

- FG (Foreground Graphics) は、最前面に表示される映像です
- [BG](#bg) と [スプライト](#sprite) の前面に表示されます
- ゲームのスコアやメッセージウインドウなどの表示に利用することを想定しています
- [ネームテーブル](#name-table) に[キャラクタ番号](#character-pattern-table)と[属性](#attribute)を指定することで表示できます
- [パレット](#palette) の色番号 0 が透明色になります
- [属性](#attribute)の指定で描画を非表示（hidden）にすることができ、デフォルトは非表示になっています
- [BG](#bg) とは独立した [ハードウェアスクロール](#hardware-scroll) に対応しています

#### (Sprite)

- 妖精（Sprite）は、画面上を動き回るキャラクタ映像です
- [BG](#bg) の前面 & [FG](#fg) の背面 に表示されます
- ゲームのキャラクタ表示に利用することを想定しています
- 最大 256 枚を同時に表示できます
- [OAM](#oam) に表示座標、[キャラクタ番号](#character-pattern-table)、[属性](#attribute)を指定することで表示できます
- [属性](#attribute)の指定で描画を非表示（hidden）にすることができ、デフォルトは非表示になっています

#### (Name Table)

- VGS-Zero では 8x8 ピクセルの矩形単位の[キャラクタパターン](#character-pattern-table)番号をネームテーブルに書き込むことでグラフィックスを表示します
- ネームテーブルは 32 行 32 列の二次元配列です（BG/FG 共通）

#### (Attribute)

アトリビュートは、BG, FG, スプライト共通のキャラクタパターン表示属性です。

| Bit-7 | Bit-6 | Bit-5 | Bit-4 | Bit-3 | Bit-2 | Bit-1 | Bit-0 |
| :---: | :---: | :---: | :---: | :---: | :---: | :---: | :---: |
| `VI`  | `UD`  | `LR`  |   -   | `P3`  | `P2`  | `P1`  | `P0`  |

- `VI`: `0` = 非表示, `1` = 表示 _（※ BG に限り 0 でも表示されます）_
- `UD`: `1` にすると上下反転で表示
- `LR`: `1` にすると左右反転で表示
- `P0~P3`: [パレット](#palette)番号 (0 ~ 15)

#### (Palette)

- VGS-Zero では最大 16 個のパレットを使用できます
- 各パレットには 16 色を RGB555 形式で指定できます
- FG と スプライトの場合、色番号 0 は透明色になります

#### (OAM)

OAM はスプライトの表示座標、[キャラクタパターン](#character-pattern-table)番号、[属性](#attribute) の要素を持つ構造体です。

```c
struct OAM {
    unsigned char y;
    unsigned char x;
    unsigned char pattern;
    unsigned char attribute;
} oam[256];
```

VGS-Zero では最大 256 枚のスプライトを同時に表示でき、水平方向の表示数に上限がありません。

#### (Scanline Counter)

- スキャンラインカウンタは、VDP のピクセルレンダリング位置を特定することができる読み取り専用の VDP レジスタです
- `0x9600` が垂直方向で `0x9601` が垂直方向です
- 垂直方向の値を待機することでラスター[スクロール](#hardware-scroll)等の処理を **割り込み無し** で実装することができます
- 水平方向は高速に切り替わるため使い所は無いかもしれません

#### (Hardware Scroll)

- [BG](#bg) は `0x9602` に X 座標, `0x9603` に Y 座標の描画起点座標を指定することができます
- [FG](#fg) は `0x9604` に X 座標, `0x9605` に Y 座標の描画起点座標を指定することができます
- `0x9602` ~ `0x9605` を読み取ることで現在のスクロール位置を取得することもできます

#### (VDP Status)

| Bit-7 | Bit-6 | Bit-5 | Bit-4 | Bit-3 | Bit-2 | Bit-1 | Bit-0 |
| :---: | :---: | :---: | :---: | :---: | :---: | :---: | :---: |
|  BL   |   -   |   -   |   -   |   -   |   -   |   -   |   -   |

- BL: 1 = start vblank

NOTE: Status register always reset after read.

#### (Character Pattern Table)

- キャラクタパターンテーブルには 8x8 ピクセルのキャラクタパターンを最大 256 個定義できます
- 1 キャラクタのサイズは 32 bytes です
- テーブル全体のサイズは 32 x 256 = 8192 bytes で丁度バンクのサイズと一致します
- 特定のバンクの内容をキャラクタパターンテーブルに転送できる高速 DMA 機能が搭載されています

キャラクタパターンテーブルのビットレイアウトは次の通りです。

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

- `Hxx` : 上位 4bit (0 ~ 15 = 色番号) ※xxはバイト位置
- `Lxx` : 下位 4bit (0 ~ 15 = 色番号) ※xxはバイト位置
- FGとスプライトの場合、色番号0は常に透明色です
- 使用するパレット番号は[属性](#attribute)に指定します

### I/O Map

|   Port    |  I  |  O  | Description  |
| :-------: | :-: | :-: | :----------- |
|   0xA0    |  o  |  -  | [ジョイパッド](#joypad) |
|   0xB0    |  o  |  o  | [ROM Bank](#bank-switch) 0 (default: 0x00) |
|   0xB1    |  o  |  o  | [ROM Bank](#bank-switch) 1 (default: 0x01) |
|   0xB2    |  o  |  o  | [ROM Bank](#bank-switch) 2 (default: 0x02) |
|   0xB3    |  o  |  o  | [ROM Bank](#bank-switch) 3 (default: 0x03) |
|   0xC0    |  -  |  o  | [ROM to Character DMA](#rom-to-character-dma) |
|   0xC2    |  -  |  o  | [memset 相当の DMA](#memset-dma) |
|   0xC3    |  -  |  o  | [memcpy 相当の DMA](#memcpy-dma) |
|   0xDA    |  o  |  o  | [データのセーブ・ロード](#save-data) |
|   0xE0    |  -  |  o  | BGM を[再生](#play-bgm) |
|   0xE1    |  -  |  o  | BGM を[中断](#pause-bgm)、[再開](#resume-bgm)、[フェードアウト](#fadeout-bgm) |
|   0xF0    |  -  |  o  | 効果音を再生 |
|   0xF1    |  -  |  o  | 効果音を停止 |
|   0xF2    |  -  |  o  | 効果音が再生中かチェック |

#### (JoyPad)

```z80
IN A, (0xA0)
```

| Bit-7 | Bit-6 | Bit-5 | Bit-4 | Bit-3 | Bit-2 | Bit-1 | Bit-0 |
| :---: | :---: | :---: | :---: | :---: | :---: | :---: | :---: |
| `Up`  | `Down` | `Left` | `Right` | `Start` | `Select` | `A` | `B` |

#### (Bank Switch)

```z80
# Read current bank of ROM Bank 0
IN A, (0xB0)

# Switch ROM Bank 1 to No.17
LD A, 0x11
OUT (0xB1), A
```

#### (ROM to Character DMA)

```z80
 # バンク番号 = 0x22 をキャラクタパターンへ転送
LD A, 0x22
OUT (0xC0), A
```

#### (memset DMA)

```z80
LD BC, 0xC000   # 転送先アドレス
LD HL, 0x2000   # 転送バイト数
LD A,  0xFF     # 転送する値
OUT (0xC2), A   # memset
```

#### (memcpy DMA)

```z80
LD BC, 0xC000   # 転送先アドレス (RAM)
LD DE, 0x6000   # 転送元アドレス (ROM Bank 3)
LD HL, 0x2000   # 転送バイト数 (8KB)
OUT (0xC3), A   # memcpy (※書き込んだ値は無視されるので何でもOK)
```

#### (Save Data)

- ポート 0xDA の I/O でセーブ（OUT）、ロード（IN）ができます
- セーブデータのファイル名は SD カードルートディレクトリ（SDL2の場合はカレントディレクトリ）の `save.dat` 固定です
- RPG のセーブ機能や STG のハイスコア保存機能などで利用することを想定しています

セーブの実装例:

```z80
LD BC, 0xC000   # セーブするデータのアドレスを指定 (RAM 領域のみ指定可能)
LD HL, 0x2000   # セーブするデータのサイズを指定 (最大 16 KB = 0x4000)
OUT (0xDA), A   # セーブ (※書き込んだ値は無視されるので何でもOK)
AND 0xFF        # セーブ結果はレジスタAに格納される
JZ SAVE_SUCCESS # 成功時は 0
JNZ SAVE_FAILED # 失敗時は not 0
```

ロードの実装例:

```z80
LD BC, 0xC000   # ロード先のアドレスを指定 (RAM 領域のみ指定可能)
LD HL, 0x2000   # ロードするデータサイズを指定 (最大 16 KB = 0x4000)
IN A, (0xDA)    # セーブ (※書き込んだ値は無視されるので何でもOK)
JZ LOAD_SUCCESS # ロード成功時は 0
JNZ LOAD_FAILED # ロード失敗時は not 0 (※ロード先は 0x00 で埋められる)
```

注意事項:

- save.dat がロード時に指定したサイズ（HL）よりも小さくてもロードは成功し、この時、データが存在しない領域は 0x00 で埋められます
- スタック領域へのロードを行うとプログラムが暴走する可能性があります
- ユーザが異なるゲームのセーブデータを用いて動かす可能性を考慮するのが望ましいです
- セーブの頻繁な実行は SD カードの寿命を縮めたり、SD カード破損の原因になるため、最小限の実行に留めることが望ましいです
- セーブ中に RaspberryPi Zero 2W の電源断を行うと SD カードが破損する恐れがあります
- カーネルは、セーブとロードが実行された時に SD カードのマウントを行い、処理が完了すると自動的にアンマウントします

#### (Play BGM)

```z80
LD A, 0x01      # 演奏対象のBGM番号を指定
OUT (0xE0), A   # BGMの演奏を開始
```

#### (Pause BGM)

```z80
LD A, 0x00      # オペレーションID: Pause
OUT (0xE1), A   # BGMの演奏を中断
```

#### (Resume BGM)

```z80
LD A, 0x01      # オペレーションID: Resume
OUT (0xE1), A   # BGMの演奏を再開
```

#### (Fadeout BGM)

```z80
LD A, 0x02      # オペレーションID: Fadeout
OUT (0xE1), A   # BGMの演奏をフェードアウト
```

#### (Play Sound Effect)

```z80
LD A, 0x01      # 再生する効果音の番号を指定
OUT (0xF0), A   # 効果音を再生
```

#### (Stop Sound Effect)

```z80
LD A, 0x02      # 停止する効果音の番号を指定
OUT (0xF1), A   # 効果音を停止
```

#### (Check Sound Effect)

```z80
LD A, 0x03      # チェックする効果音の番号を指定
OUT (0xF1), A   # 効果音をチェック (A=0: Stopped, A=1: Playing)
AND 0x01
JNZ EFF03_IS_PILAYING
JZ  EFF03_IS_NOT_PLAYING
```

## How to Sell Your Game

コミックマーケットや通販で VGS-Zero 用のゲームを販売する時は、

1. FAT32 でフォーマットした micro SD カードを準備
2. [./image](./image/) 以下のファイルを micro SD カードのルートディレクトリへコピー
3. [game.pkg](#gamepkg) を差し替え
4. [README](./image/README) を削除
5. README.txt（ゲームの遊び方を記載したテキスト）を格納
6. micro-SD カードを[ケース](https://www.amazon.co.jp/dp/B08TWR47LV/)などに格納
7. ケースにゲームのラベルを塗布

といった形で媒体を準備して販売するのが良いかと思われます。（ダウンロード販売の場合は SD カードに格納するものと同等のものを ZIP で固めて販売）

README.txtの記載凡例:

```
================================================================================
<<<ゲームタイトル>>>
<<<コピーライト>>>
================================================================================

この度は「<<<ゲームタイトル>>>」をお買い上げいただき誠にありがとうございます。
本書は本ゲームの遊び方などを記載していますので、プレイ前にご一読ください。

【必要なハードウェア】
・RaspberryPi Zero 2W: https://www.switch-science.com/products/7600
・RaspberryPi Zero 2W とテレビを接続する HDMI ケーブル
・RaspberryPi Zero 2W に接続する USB ゲームパッド
・RaspberryPi Zero 2W に接続する USB 電源 (5V/2.5A以上を推奨)
・テレビなど（HDMI入力対応、オーディオ有り、リフレッシュレート60Hz）

【起動方法】
・RaspberryPi Zero 2W に本製品（micro SDカード）を挿入
・RaspberryPi Zero 2W の電源を ON

【遊び方】
<<<ゲームの遊び方を記載>>>

【連絡先】
<<<Twitterアカウントやeメールアドレスなどを記載>>>
```

販売により得られた収益の SUZUKIPLAN へのペイバック（ライセンス料）等は一切不要です。

## License

- VGS-Zero 本体は GPLv3 の OSS です: [LICENSE-VGS0.txt](./LICENSE_VGS0.txt)
- VGS-Zero 本体配布イメージには RaspberryPi ブートローダーが含まれます: [LICENCE.broadcom](./LICENCE.broadcom)
- VGS-Zero 本体には Circle（GPLv3）が含まれます: [LICENSE-CIRCLE.txt](./LICENSE-CIRCLE.txt)
- VGS-Zero 本体には LZ4 Library（2か条BSD）が含まれます: [LICENSE-LZ4LIB.txt](./LICENSE-LZ4LIB.txt)
- VGS-Zero 本体には SUZUKI PLAN - Z80 Emulator (MIT) が含まれます: [LICENSE-Z80.txt](./LICENSE-Z80.txt)
- VGS-Zero Library for Z80 は MIT ライセンスの OSS です:[LICENSE-VGS0LIB.txt](./LICENSE_VGS0LIB.txt)
