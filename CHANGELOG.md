# Change Log

## [Version 1.17.0](https://github.com/suzukiplan/vgszero/releases/tag/1.17.0)

## [Version 1.16.0](https://github.com/suzukiplan/vgszero/releases/tag/1.16.0)

BG, FG, スプライトのインタレース描画機能を追加しました。

0x9F0C の設定により BG/FG の描画をインタレース（奇数行または奇数ピクセルの描画をスキップ）することができます。

__(0x9F0C)__

| Bit-7 | Bit-6 | Bit-5 | Bit-4 | Bit-3 | Bit-2 | Bit-1 | Bit-0 |
| :---: | :---: | :---: | :---: | :---: | :---: | :---: | :---: |
|   -   |   -   |   -   |   -   | `FH`  | `FV`  | `BH`  | `BV`  |

- `FH` : FG の奇数スキャンラインの描画をスキップ（横インタレース）
- `FV` : FG の奇数ピクセルの描画をスキップ（縦インタレース）
- `BH` : BG の奇数スキャンラインの描画をスキップ（横インタレース）
- `BV` : BG の奇数ピクセルの描画をスキップ（縦インタレース）

スプライトは OAM の `attr2` の設定によりオブジェクト毎にインタレース描画を設定することができます。

__(Attribute2)__

| Bit-7 | Bit-6 | Bit-5 | Bit-4 | Bit-3 | Bit-2 | Bit-1 | Bit-0 |
| :---: | :---: | :---: | :---: | :---: | :---: | :---: | :---: |
|   -   |   -   |   -   |   -   |   -   |   -   | `IH`  | `IV`  |

- `IH` : 奇数スキャンラインの描画をスキップ（横インタレース）
- `IV` : 奇数ピクセルの描画をスキップ（縦インタレース）

## [Version 1.15.1](https://github.com/suzukiplan/vgszero/releases/tag/1.15.1)

- [vgsasm](https://github.com/suzukiplan/vgsasm) を 1.2.2 から 1.2.4 に更新
  - Cannot specify nested structures with the `offset` operator.
  - `offset` calculation results for multi-count fields are not as expected.

## [Version 1.15.0](https://github.com/suzukiplan/vgszero/releases/tag/1.15.0)

- [vgsasm](https://github.com/suzukiplan/vgsasm) を version 1.0.0 から 1.2.2 に更新
  - `LD E, {IXH|IXL|IYH|IYL}` are incorrectly assembled to `LD C, {IXH|IXL|IYH|IYL}`.
  - Support `label+n` expression.
  - Support nested struct access.
- ROM to Memory DMA (OUT $C1) が想定通りに動作をしない不具合を修正
- マクロ `dma2mem` を vgszero.inc へ追加 

## [Version 1.14.0](https://github.com/suzukiplan/vgszero/releases/tag/1.14.0)

- ツールチェインに [vgsasm](https://github.com/suzukiplan/vgsasm) を追加
- 推奨アセンブラを z88dk から [vgsasm](https://github.com/suzukiplan/vgsasm) に変更
- 全ての example にアセンブリ言語版を追加

## [Version 1.13.0](https://github.com/suzukiplan/vgszero/releases/tag/1.13.0)

- [OAM16](./README.md#oam16) 機能を追加
- [example/17_clip](./example/17_clip/) を追加
- VGS0LIB に次の API を追加
  - `vgs0_oam_set16` ... OAM16 を用いたスプライト属性の一括設定マクロ
  - `VGS0_ADDR_OAM16` ... OAM16 の先頭アドレス
  - `OAM16` ... OAM16 構造体

## [Version 1.12.0](https://github.com/suzukiplan/vgszero/releases/tag/1.12.0)

Attribute の bit-4 を指定することで DPM のパターン番号を +1 する機能を追加
  
| Bit-7 | Bit-6 | Bit-5 | **Bit-4** | Bit-3 | Bit-2 | Bit-1 | Bit-0 |
| :---: | :---: | :---: | :---: | :---: | :---: | :---: | :---: |
| `VI`  | `LR`  | `UD`  | **`PTN`** | `P3`  | `P2`  | `P1`  | `P0`  |

- 本機能は BG, FG, スプライトの全てで使用可能
- 本機能は DPM (Direct Pattern Mapping) を使用時に限り有効

## [Version 1.11.2](https://github.com/suzukiplan/vgszero/releases/tag/1.11.2)

- 未サポートの音楽データ形式を再生しようとするとクラッシュする不具合を修正
- RaspberryPi で NSF を再生し続けた時に `Assertion error` でクラッシュする場合がある不具合を修正
- NSF を再生した時にデフォルトトラック（NSF の先頭から 8 バイト目で指定されたトラック）を再生するように修正
- NSFPlayのコードリファクタ（不要コードの削除）
- example/03_sound を VGSBGM と NSF を混在させた形に変更
- README.md に推奨開発ツールの一覧を掲載
- README.md でマルチトラック NSF の使い方について詳述

## [Version 1.11.0](https://github.com/suzukiplan/vgszero/releases/tag/1.11.0)

- Extra RAM Bank の複製機能 `OUT ($B5)` を追加
- NSF 形式 のデータを用いた BGM (ファミコン標準音源 + VRC6) の再生をサポート（bgm.dat に .nsf を組み込みそれを指定して再生することで再生可能）
  - NSF 対応についての参考記事: https://note.com/suzukiplan/n/n94ea503ff2c8

## [Version 1.10.0 (2024.05.27)](https://github.com/suzukiplan/vgszero/releases/tag/1.10.0)

- [SDL2 版エミュレータ](./src/sdl2/) にデバッグオプション `-d` を追加

## [Version 1.9.0 (2024.05.26)](https://github.com/suzukiplan/vgszero/releases/tag/1.9.0)

- Extra RAM Bank (2MB RAM) を追加
- vgs0lib に上記機能を使用できる `vgs0_rambank_switch` 関数と `vgs0_rambank_get` 関数を追加

## [Version 1.8.0 (2024.05.24)](https://github.com/suzukiplan/vgszero/releases/tag/1.8.0)

- 特定の ROM バンクの内容を任意メモリに任意サイズだけ転送する DMA 機能（ROM to Memory DMA）を追加
- vgs0lib に上記機能を使用できる `vgs0_dma_ram` 関数を追加

## [Version 1.7.0 (2024.02.29)](https://github.com/suzukiplan/vgszero/releases/tag/1.7.0)

- BGM/SE のボリューム設定インタフェースを追加

## [Version 1.6.0 (2024.02.23)](https://github.com/suzukiplan/vgszero/releases/tag/1.6.0)

- [VGS-Zero SDK for Steam](https://github.com/suzukiplan/vgszero-steam) のリポジトリを分割

## [Version 1.5.0 (2024.02.19)](https://github.com/suzukiplan/vgszero/releases/tag/1.5.0)

- BG/FG に 1024 パターンモード を追加

## [Version 1.4.0 (2024.02.18)](https://github.com/suzukiplan/vgszero/releases/tag/1.4.0)

- VGS-Zero SDK for Steam を追加
- HAGe に収束型乱数（8bits, 16bits）を取得する機能を追加
- HAGe にパーリンノイズを取得する機能を追加
- vgs0lib に以下の関数を追加:
  - `vgs0_srand8` : 8-bits 乱数シードを設定
  - `vgs0_rand8` : 8-bits 乱数を取得
  - `vgs0_srand16` : 16-bits 乱数シードを設定
  - `vgs0_rand16` : 16-bits 乱数を取得
  - `vgs0_noise_seed` : パーリンノイズのシードを設定
  - `vgs0_noise_limitX` : パーリンノイズの X 座標縮尺を設定
  - `vgs0_noise_limitY` : パーリンノイズの Y 座標縮尺を設定
  - `vgs0_noise` : パーリンノイズを取得
  - `vgs0_noise_oct` : パーリンノイズを取得（オクターブあり）

## [Version 1.3.0 (2024.02.12)](https://github.com/suzukiplan/vgszero/releases/tag/1.3.0)

- vgs0lib に `vgs0_exit` 関数（割り込み禁止状態で HALT）を追加
- ラズパイ版で GPIO ジョイパッドによる入力に対応
- ラズパイ版で検証済みの USB ジョイパッドをドキュメント記載
- SDL2 版で `DI` している状態での `HALT` を検出した時にプロセスを停止するように変更
- SDL2 版でウィンドウの閉じるボタン（×）を押してもプロセスが終了しない不具合を修正
- SDL2 版でプログレッシブ表示（240x192）のキャプチャ保存に対応
- SDL2 版のウィンドウタイトル表示変更: VGS0 -> VGS-Zero

## [Version 1.2.3 (2024.01.30)](https://github.com/suzukiplan/vgszero/releases/tag/1.2.3)

- ラズパイで config.sys が正常に読み込まれない不具合を修正

## [Version 1.2.2 (2024.01.26)](https://github.com/suzukiplan/vgszero/releases/tag/1.2.2)

- ラズパイでリセット時に 16 フレームの空白フレームを挿入

## [Version 1.2.1 (2024.01.25)](https://github.com/suzukiplan/vgszero/releases/tag/1.2.1)

- ラズパイでBGMが再生されない問題を修正

## [Version 1.2.0 (2024.01.25)](https://github.com/suzukiplan/vgszero/releases/tag/1.2.0)

- LZ4 を削除

## [Version 1.1.0 (2024.01.23)](https://github.com/suzukiplan/vgszero/releases/tag/1.1.0)

- OAM の構造を変更
  - widthMinus1 と heightMinus1 を追加して 1x1 〜 16x16 のキャラクタパターンを 1 枚のスプライトとして表示できるようにする
  - bank を追加し OAM レコード毎に異なるキャラクタパターンを使用できるようにする
- OAM の構造変更に伴い [VRAM Memory Map を変更（※破壊的変更）](https://github.com/suzukiplan/vgszero/pull/14/files#diff-b335630551682c19a781afebcf4d07bf978fb1f8ac04c6bf87428ed5106870f5R325-R346)
- `vgs0_memset` が期待通りに動作しない不具合を修正
- 当たり判定のハードウェア機能を追加
- 乗算、除算、剰余残のハードウェア機能を追加
- 三角関数のハードウェア機能を追加
- ツールチェイン bmp2chr でパレットバイナリ出力に対応
- 画面表示をインタレース表示に変更
- SDCC 版 API に `vgs0_putstr` を追加
- ハードウェアリセット機能を追加（START+SELECT+A+B同時押しでリセット）
- SDL2 版エミュレータにデバッグ用機能を追加
  - R: リセット
  - S: キャプチャ（ram.bin, vram.bin, screen.bmp）

## [Version 1.0.0 (2024.01.01)](https://github.com/suzukiplan/vgszero/releases/tag/1.0.0)

初期リリース

