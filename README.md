# [WIP] SUZUKI PLAN - Video Game System Zero

![logo](logo.png)

SUZUKI PLAN - Video Game System Zero (VGS0) は RaspberryPi Zero をコアに用いたゲーム機です。

本リポジトリは、VGS0 の本体コード、配布イメージ、SDK、パソコン（Linux または macOS）で動作するエミュレータを提供します。

## WIP status

- implementation
  - [x] CPU
  - [x] VDP
  - [x] BGM API
  - [ ] Sound Effect API
  - [x] RaspberryPi Zero
  - [ ] RaspberryPi Zero 2W
  - [x] 22050Hz 1ch -> 44100Hz 2ch へ変更が必要かも（HDMIの仕様）
- examples
  - [x] Hello, World!
  - [x] グローバル変数の使い方
  - [x] BGM再生
  - [ ] SE再生
  - [ ] Map Scroll
  - [ ] スプライト
  - [ ] スプライト・BG・FG全表示しつつ音楽+効果音（限界性能チェック用）
  - [ ] アセンブリ言語実装例（Helloのみ）
- documents
  - [ ] Z80: Memory map
  - [ ] Z80: I/O map
  - [ ] C API Library
  - [ ] `game.rom`
  - [ ] `bgm.dat`
  - [ ] `se.dat`

## VGS0 Feature

- CPU: Z80A 互換
  - Z80 アセンブリ言語でプログラムを記述
  - SDCC を用いて C 言語でもプログラミング可能
  - 最大 2MB (8kb × 256) のプログラムとデータ (※音声データを除く)
  - RAM サイズ 16KB (PV16相当!)
- VDP (映像処理)
  - 解像度: 240x192 ピクセル
  - 32,768 色中 256 色を同時発色可能
  - BG, FG, スプライトを合成描画
    - 最大 256 枚のキャラクタパターン (8x8ピクセル)
    - BG, FG のネームテーブルサイズ: 32x32 (256x256 ピクセル)
    - BG, FG: ハードウェアスクロール対応
    - 最大 256 枚のスプライトを表示可能（水平上限なし）
  - VRAM サイズ 16KB (TMS9918A 相当!)
- BGM
  - VGS の MML で記述された BGM を再生可能
  - ゲームプログラム (Z80) 側でのサウンドドライバ実装が不要!
  - ゲームプログラム (Z80) 側の RAM (16KB) を専有不要!
  - 本体 ROM (`game.rom`) とは別アセット（`bgm.dat`）
  - 最大 256 曲 & 8MB 以下を搭載可能
- SE (効果音)
  - 22050Hz 16bit 1ch 形式の PCM 音源で効果音を再生
  - ゲームプログラム (Z80) 側でのサウンドドライバ実装が不要!
  - ゲームプログラム (Z80) 側の RAM (16KB) を専有不要
  - 本体 ROM (`game.rom`) とは別アセット（`se.dat`）
  - 最大 256 個 & 8MB 以下を搭載可能

## RaspberryPi Zero

### Required Device

1. RaspberryPi Zero, Zero W, Zero WH or Zero 2W
2. USB 接続のゲームパッド
3. HDMI 接続のモニタ（解像度 640x480 以上、リフレーッシュレート 60 Hz、オーディオあり）
4. SD カード

### How to Luanch

1. FAT32 フォーマットされた SD カードを準備
2. SD カードのルートディレクトリに [./image/rpizero](./image/rpizero) または [./image/rpizero2](./image/rpizero2) 以下のファイルをコピー
3. `game.rom` を起動対象のゲームの ROM データに置き換える
4. `bgm.dat` を起動対象のゲームの BGM データに置き換える
5. `eff.dat` を起動対象のゲームの効果音データに置き換える
6. SD カードを RaspberryPi Zero に挿入して電源を入れる

## License

- VGS0 本体は GPLv3 の OSS です: [LICENSE-VGS0.txt](./LICENSE_VGS0.txt)
- VGS0 本体配布イメージには RaspberryPi ブートローダーが含まれます: [LICENSE.broadcom](./LICENSE.broadcom)
- VGS0 本体には Circle（GPLv3）が含まれます: [LICENSE-CIRCLE.txt](./LICENSE-CIRCLE.txt)
- VGS0 本体には LZ4 Library（2か条BSD）が含まれます: [LICENSE-LZ4LIB.txt](./LICENSE-LZ4LIB.txt)
- VGS0 本体には SUZUKI PLAN - Z80 Emulator (MIT) が含まれます: [LICENSE-Z80.txt](./LICENSE-Z80.txt)
- VGS0 Library for Z80 は MIT ライセンスの OSS です:[LICENSE-VGS0LIB.txt](./LICENSE_VGS0LIB.txt)
