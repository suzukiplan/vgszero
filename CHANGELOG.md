# Change Log

## [Version 1.10.0 (2024.05.27)](https://github.com/suzukiplan/vgszero/releases/tag/1.9.0)

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

