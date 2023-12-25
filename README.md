# SUZUKI PLAN - Video Game System Zero

![logo](logo.png)

SUZUKI PLAN - Video Game System Zero (VGS0) は、[FCS80](https://github.com/suzukiplan/fcs80/) と VGS の機能を融合した新しいレトロゲーム機です。

VGS0 向け開発したゲームは RaspberryPi Zero (ベアメタル環境) で動作させることを想定しています。

また、開発中のゲームを Linux または macOS 上のシミュレータで動作確認できるため、効率的なゲーム開発が可能です

なお、VGS0 本体は GPL ライセンスの OSS ですが、VGS0 向けに開発したゲームは自由なライセンスに設定できるため OSS にする必要がありません。（もちろん OSS にすることもできます）

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
  - 最大 256 曲を搭載可能
- 効果音
  - 22050Hz 16bit 1ch 形式の PCM 音源で効果音を再生
  - 最大 256 個を搭載可能

## Required Device

1. RaspberryPi Zero, Zero W, Zero WH or Zero 2W
2. USB 接続のゲームパッド
3. HDMI 接続のモニタ（解像度 640x480 以上、リフレーッシュレート 60 Hz、オーディオあり）
4. SD カード

## License

- VGS0 本体は GPLv3 の OSS です: [LICENSE-VGS0.txt](./LICENSE_VGS0.txt)
- VGS0 本体配布イメージには RaspberryPi ブートローダーが含まれます: [LICENSE.broadcom](./LICENSE.broadcom)
- VGS0 本体には Circle（GPLv3）が含まれます: [LICENSE-CIRCLE.txt](./LICENSE-CIRCLE.txt)
- VGS0 本体には LZ4 Library（2か条BSD）が含まれます: [LICENSE-LZ4LIB.txt](./LICENSE-LZ4LIB.txt)
- VGS0 本体には SUZUKI PLAN - Z80 Emulator (MIT) が含まれます [LICENSE-Z80.txt](./LICENSE-Z80.txt)
- VGS0 Library for Z80 は MIT ライセンスの OSS です:（[LICENSE-VGS0LIB.txt](./LICENSE_VGS0LIB.txt)）
