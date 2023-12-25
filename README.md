# SUZUKI PLAN - Video Game System Zero

![image](vgs0.png)

SUZUKI PLAN - Video Game System Zero (VGS0) は、FCS80 と VGS の機能を融合した新しいレトロゲーム機です。

- CPU: Z80A 互換
  - Z80 アセンブリ言語でプログラムを記述
  - SDCC を用いて C 言語でもプログラミング可能
  - 最大 2MB (8kb × 256) のプログラムとデータ (ASCII8メガロム相当!)
  - RAM サイズ 16KB (PV16相当!)
- VDP (映像処理)
  - 解像度: 240x192 ピクセル
  - BG, FG, スプライトを合成描画
    - 256 枚のキャラクタパターン (8x8ピクセル)
    - 4bit カラー（16色）
    - 16 色パレット x 16 個（最大 256 色同時発色）
    - パレットの色形式: RGB555
    - BG, FG: ネームテーブルサイズ: 32x32 (256x256 ピクセル)
    - BG, FG: ハードウェアスクロール対応
    - スプライト: 最大 256 枚同時表示可能（水平上限なし）
  - VRAM サイズ 16KB (TMS9918A 相当!)
- BGM
  - VGS の MML で記述された BGM を再生
  - 最大 256 曲
- 効果音
  - 22050Hz 16bit 1ch 形式の PCM 音源で効果音を再生
  - 最大 256 個
