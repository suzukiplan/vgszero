# Change Log

## [Version 1.1.0 (WIP)](https://github.com/suzukiplan/vgszero/releases/tag/1.1.0)

- OAM の構造を変更
  - widthMinus1 と heightMinus1 を追加して 1x1 〜 16x16 のキャラクタパターンを 1 枚のスプライトとして表示できるようにする
  - bank を追加し OAM レコード毎に異なるキャラクタパターンを使用できるようにする
- OAM の構造変更に伴い [VRAM Memory Map を変更（※破壊的変更）](https://github.com/suzukiplan/vgszero/pull/14/files#diff-b335630551682c19a781afebcf4d07bf978fb1f8ac04c6bf87428ed5106870f5R325-R346)
- SDL2 版エミュレータにデバッグ用機能を追加

## [Version 1.0.0 (2024.01.01)](https://github.com/suzukiplan/vgszero/releases/tag/1.0.0)

初期リリース

