# Video Game Sound .BGM Binary Format - version 3

## Outline

MML のテキストファイルを vgsmml コマンドでコンパイルすることで生成される VGS の音楽データ（.bgm）のバイナリ仕様について纏めます。

例えば、MIDI ファイルを .BGM 形式に変換できるツールの開発などに役立ててください。

## Version History

1. [文献](https://suzukiplan.blogspot.com/2015/10/bgm-on-vgs.html)によると2011年～2012年ごろに仕様策定したらしい
2. 2023年（WAIT命令長を可変化する仕様変更を [tohovgs-pico](https://github.com/suzukiplan/tohovgs-pico) で実施）
3. 2024年（ライセンスと動作の軽量化のためLZ4圧縮を削除する仕様変更を VGS-Zero で実施）

本書ではバージョン 3 の仕様を記します。

## Data Format

|Chunk name|Size    |Description|
|:---------|:------:|:----------|
|EyeCatch  |8 bytes |`VGSBGM-V` (固定)|
|LengthTime|4 bytes |曲の長さ (全 [WAIT](#wait) 値の合計)|
|LoopTime  |4 bytes |ループ起点 (先頭から [JUMP](#jump) 先ノートまでの [WAIT](#wait) 値の合計)|
|Notes     |variable|シーケンスデータ群（複数の[vgsnote](#vgsnote)）|

> `LengthTime` と `LoopTime` は、東方BGM on VGS が楽曲再生時に必要なデータなので、VGS-Zero 向けのゲームであれば必ずしも正しい値を設定しなくても正常に再生することができます。

## Basic Specification

Video Game Sound の基本的な音源仕様を記します。

### Channels

- VGS では 6 チャネルの音声を同時に再生することができます
- [vgsnote](#vgsnote) は __チャネル別ノート__ と __制御ノート__ に分類できます
- チャネル別ノート（先頭 1 バイトの下位 3 ビットでチャネル番号 `0~5` を指定）
  - [`ENV1`](#env1) ... 開始エンベロープ
  - [`ENV2`](#env2) ... 終了エンベロープ
  - [`VOL`](#vol) ... チャネル別ボリューム
  - [`KEYON`](#keyon) ... キーON（発声指示）
  - [`KEYOFF`](#keyoff) ... キーOFF（発声終了指示）
  - [`PDOWN`](#pdown) ... ピッチダウン
- 全体制御ノート
  - [`MVOL`](#mvol) ... マスターボリューム
  - [`JUMP`](#jump) ... ジャンプ（ループ）
  - [`LABEL`](#label) ... ラベル
  - [`WAIT`](#wait) ... ウェイト

### Volume

Video Game Sound は、メモリ上に格納された基礎波形データ（[vgstone](#vgstone)）、[`VOL`](#vol)、[`MVOL`](#mvol) を次の式で乗算して求めた各チャネルボリュームの和（合成波形）を符号付 16-bits のパルス符号として音声出力します。

```
チャネルボリューム = vgstone（-64～63）× VOL × MVOL × Envelope(0%~100%)
出力波形 = 各チャネルボリュームの和
```

### Envelope

Video Game Sound の各チャネルは次のようにエンベロープを形成します:

- [`KEYON`](#keyon) が実行された時
  - [`ENV1`](#env1) で指定された時間内にリニアにチャネルボリュームを増幅
- [`KEYOFF`](#keyoff) が実行された時
  - [`ENV2`](#env2) で指定された時間内にリニアにチャネルボリュームを減衰

### vgstone

以下の4種類の音色があります。

|Tone Code|Tone|
|:--------|:---|
| 0       | 三角波（ベースや笛のような音色）|
| 1       | ノコギリ波（ストリングスなどのような音色）|
| 2       | 矩形波（PSGのような音色）|
| 3       | ノイズ（ドラムや歪んだギターのような音色）|

各音色は 12 平均律でオクターブ 0 の A からオクターブ 7 の G# (A♭) までの 84 音階の音程（ピッチ）があります。

|Pitch Code| +0 | +1 | +2 | +3 | +4 | +5 | +6 | +7 | +8 | +9 | +10 | +11 |
|---------:|:--:|:--:|:--:|:--:|:--:|:--:|:--:|:--:|:--:|:--:|:---:|:---:|
|0         | A0 | A#0| B0 | C1 | C#1| D1 | D#1| E1 | F1 | F#1|  G1 | G#1 |
|12        | A1 | A#1| B1 | C2 | C#2| D2 | D#2| E2 | F2 | F#2|  G2 | G#2 |
|24        | A2 | A#2| B2 | C3 | C#3| D3 | D#3| E3 | F3 | F#3|  G3 | G#3 |
|36        | A3 | A#3| B3 | C4 | C#4| D4 | D#4| E4 | F4 | F#4|  G4 | G#4 |
|48        | A4 | A#4| B4 | C5 | C#5| D5 | D#5| E5 | F5 | F#5|  G5 | G#5 |
|60        | A5 | A#5| B5 | C6 | C#6| D6 | D#6| E6 | F6 | F#6|  G6 | G#6 |
|72        | A6 | A#6| B6 | C7 | C#7| D7 | D#7| E7 | F7 | F#7|  G7 | G#7 |

[KEYON](#keyon)の時に音色コード（Tone Code）と音程コード（Pitch Code）を一緒に指定します。

Video Game Sound は、全ての音色と音程波形が予めメモリにストアしているため、AY-3-8910 などの一般的なチップチューン音源と異なり、ピッチを周波数ではなく 12 平均律で指定する仕様になっている点をご注意ください。この仕様はメモリを沢山使うデメリットがある反面 __CPU負荷が低くなる__ メリットがあります。（波形の音程計算をテーブル参照にしている為）

> 音程は低音ほど安定して高音になるほど（恐らく基礎波形作成時の分周比の関係で）若干音痴になります。

## vgsnote

vgsnote は、例えば「音を再生する（キーON）」などの 1 ～ 5 バイトの可変長のノートデータで、__先頭 1 バイトの上位 4 bits__ で種別を識別します。

|Note Type|Data Code|Data Length|Description|
|:--------|:-------:|:---------:|:----------|
|[`ENV1`](#env1)|`0x1X`|3 bytes|開始エンベロープ|
|[`ENV2`](#env2)|`0x2X`|3 bytes|終了エンベロープ|
|[`VOL`](#vol)|`0x3X`|2 bytes|チャネルボリューム|
|[`MVOL`](#mvol)|`0x4X`|2 bytes|マスターボリューム|
|[`KEYON`](#keyon)|`0x5X`|3 bytes|キーON|
|[`KEYOFF`](#keyoff)|`0x6X`|1 byte|キーOFF|
|[`PDOWN`](#pdown)|`0x8X`|3 bytes|自動ピッチダウン|
|[`JUMP`](#jump)|`0x9X`|5 bytes|再生位置のジャンプ（ループ）|
|[`LABEL`](#label)|`0xAX`|1 byte|何もしない|
|[`WAIT8`](#wait)|`0xBX`|2 bytes|ウェイト|
|[`WAIT16`](#wait)|`0xCX`|3 bytes|ウェイト|
|[`WAIT32`](#wait)|`0xDX`|5 bytes|ウェイト|

### ENV1

ENV1 は [KEYON](#keyon) されたチャンネルが最大音量になるまでの時間を指定します。

ビットレイアウトは次の通りです。

|Sequence|bit-7|bit-6|bit-5|bit-4|bit-3|bit-2|bit-1|bit-0|
|:-------|:---:|:---:|:---:|:---:|:---:|:---:|:---:|:---:|
|byte #1 |  0  |  0  |  0  |  1  |  0  | ch  | ch  | ch  |
|byte #2 | env | env | env | env | env | env | env | env |
|byte #3 | env | env | env | env | env | env | env | env |

- ch は 0 ~ 5 の範囲で指定
- env は時間（0～65535）をリトルエンディアンで指定
- 時間は 22050 で 1 秒（最大約 2.972 秒）

### ENV2

ENV2 は [KEYOFF](#keyoff) されたチャンネルが無音になるまでの時間を指定します。

ビットレイアウトは次の通りです。

|Sequence|bit-7|bit-6|bit-5|bit-4|bit-3|bit-2|bit-1|bit-0|
|:-------|:---:|:---:|:---:|:---:|:---:|:---:|:---:|:---:|
|byte #1 |  0  |  0  |  1  |  0  |  0  | ch  | ch  | ch  |
|byte #2 | env | env | env | env | env | env | env | env |
|byte #3 | env | env | env | env | env | env | env | env |

- ch は 0 ~ 5 の範囲で指定
- env は時間（0～65535）をリトルエンディアンで指定
- 時間は 22050 で 1 秒（最大約 2.972 秒）

### VOL

チャンネル毎の音量です。

> 音量 = 各音色の基礎波形（-64～63）× VOL × [MVOL](#mvol) (符号付 16 bits)

ビットレイアウトは次の通りです。

|Sequence|bit-7|bit-6|bit-5|bit-4|bit-3|bit-2|bit-1|bit-0|
|:-------|:---:|:---:|:---:|:---:|:---:|:---:|:---:|:---:|
|byte #1 |  0  |  0  |  1  |  1  |  0  | ch  | ch  | ch  |
|byte #2 | vol | vol | vol | vol | vol | vol | vol | vol |

- ch は 0 ~ 5 の範囲で指定
- vol は音量値（0～255）を指定

### MVOL

マスターボリュームです。

> 音量 = 各音色の基礎波形（-64～63）× [VOL](#vol) × MVOL (符号付 16 bits)

ビットレイアウトは次の通りです。

|Sequence|bit-7|bit-6|bit-5|bit-4|bit-3|bit-2|bit-1|bit-0|
|:-------|:---:|:---:|:---:|:---:|:---:|:---:|:---:|:---:|
|byte #1 |  0  |  1  |  0  |  0  |  -  |  -  |  -  |  -  |
|byte #2 | vol | vol | vol | vol | vol | vol | vol | vol |

- vol は音量値（0～255）を指定

### KEYON

音の再生指示です。

ビットレイアウトは次の通りです。

|Sequence|bit-7|bit-6|bit-5|bit-4|bit-3|bit-2|bit-1|bit-0|
|:-------|:---:|:---:|:---:|:---:|:---:|:---:|:---:|:---:|
|byte #1 |  0  |  1  |  0  |  1  |  0  | ch  | ch  | ch  |
|byte #2 |tone |tone |tone |tone |tone |tone |tone |tone |
|byte #3 | key | key | key | key | key | key | key | key |

- ch は 0 ~ 5 の範囲で指定
- tone は音色コード（0～3）を指定
  - 0: 三角波（ベースや笛のような音色）
  - 1: ノコギリ波（ストリングスなどのような音色）
  - 2: 矩形波（PSGのような音色）
  - 3: ノイズ（ドラムや歪んだギターのような音色）
- key は音程コード（0～83）を指定
  - 音程コードは小さいほど低く、大きいほど高い12音階で指定します
  - 0 = 一番低い A (ラ)
  - 1 = A# (嬰ラ) / B♭ (変シ)
  - 2 = B (シ)

### KEYOFF

音の停止指示です。

ビットレイアウトは次の通りです。

|Sequence|bit-7|bit-6|bit-5|bit-4|bit-3|bit-2|bit-1|bit-0|
|:-------|:---:|:---:|:---:|:---:|:---:|:---:|:---:|:---:|
|byte #1 |  0  |  1  |  1  |  0  |  0  | ch  | ch  | ch  |

- ch は 0 ~ 5 の範囲で指定

なお、連打する（同じ音程の音を連続して鳴らす）場合は以下のように間に KEYOFF を挟む必要があります。

1. KEYON
2. WAIT
3. KEYOFF
4. WAIT
5. KEYON

異なる音程であれば、KEYOFFを省略することで滑らかなスラーにすることができます。

> vgsmml では `%` 命令で指定したパーセンテージの長さの KEYON を再生後 KEYOFF する仕様になっています。

### PDOWN

PDOWN (ピッチダウン) は再生中の音程を指定時間の間隔で自動的に半音づつ下げる機能です。

この機能を用いることで、ギターやベースのスライドダウンやドラム音のような演奏を少ないノート数で簡易的に実現できます。

ビットレイアウトは次の通りです。

|Sequence|bit-7|bit-6|bit-5|bit-4|bit-3|bit-2|bit-1|bit-0|
|:-------|:---:|:---:|:---:|:---:|:---:|:---:|:---:|:---:|
|byte #1 |  1  |  0  |  0  |  0  |  0  | ch  | ch  | ch  |
|byte #2 |pdown|pdown|pdown|pdown|pdown|pdown|pdown|pdown|
|byte #3 |pdown|pdown|pdown|pdown|pdown|pdown|pdown|pdown|

- ch は 0 ~ 5 の範囲で指定
- pdown は時間（0～65535）をリトルエンディアンで指定
- 時間は 22050 で 1 秒（最大約 2.972 秒）
- pdown で指定された時間が経過する都度 key がデクリメントされ続けます
- key が 0 の場合は何もしません

### JUMP

JUMP はノートの読み取り位置をジャンプするもので、曲をループさせる時に使用することを想定しています。（条件ジャンプの機能はないため 1 BGM データにつき 0 回または 1 回のみ発生します）

ビットレイアウトは次の通りです。

|Sequence|bit-7|bit-6|bit-5|bit-4|bit-3|bit-2|bit-1|bit-0|
|:-------|:---:|:---:|:---:|:---:|:---:|:---:|:---:|:---:|
|byte #1 |  1  |  0  |  0  |  1  |  -  |  -  |  -  |  -  |
|byte #2 |jump |jump |jump |jump |jump |jump |jump |jump |
|byte #3 |jump |jump |jump |jump |jump |jump |jump |jump |
|byte #4 |jump |jump |jump |jump |jump |jump |jump |jump |
|byte #5 |jump |jump |jump |jump |jump |jump |jump |jump |

- jump は vgsnote のジャンプ先バイト位置をリトルエンディアンで指定
- 0 を指定すれば先頭の vgsnote へジャンプします
- ジャンプ先が 0 でない場合、[LABEL](#label)をジャンプ先とすることを推奨

### LABEL

LABEL は何もせずに読み飛ばされます。

|Sequence|bit-7|bit-6|bit-5|bit-4|bit-3|bit-2|bit-1|bit-0|
|:-------|:---:|:---:|:---:|:---:|:---:|:---:|:---:|:---:|
|byte #1 |  1  |  0  |  1  |  0  |  -  |  -  |  -  |  -  |

シーケンサの制御処理などでの利用を想定していて、下位 4 bits は自由に使用できます。

### WAIT

WAIT8, WAIT16, WAIT32 は、音源ドライバ（vgsdecv）がシーケンス（ノートデータの読み取り）処理を終了して再生処理へ制御遷移するためのシグナルになっています。

WAIT8

|Sequence|bit-7|bit-6|bit-5|bit-4|bit-3|bit-2|bit-1|bit-0|
|:-------|:---:|:---:|:---:|:---:|:---:|:---:|:---:|:---:|
|byte #1 |  1  |  0  |  1  |  1  |  -  |  -  |  -  |  -  |
|byte #2 |wait |wait |wait |wait |wait |wait |wait |wait |

WAIT16

|Sequence|bit-7|bit-6|bit-5|bit-4|bit-3|bit-2|bit-1|bit-0|
|:-------|:---:|:---:|:---:|:---:|:---:|:---:|:---:|:---:|
|byte #1 |  1  |  1  |  0  |  0  |  -  |  -  |  -  |  -  |
|byte #2 |wait |wait |wait |wait |wait |wait |wait |wait |
|byte #3 |wait |wait |wait |wait |wait |wait |wait |wait |

WAIT32

|Sequence|bit-7|bit-6|bit-5|bit-4|bit-3|bit-2|bit-1|bit-0|
|:-------|:---:|:---:|:---:|:---:|:---:|:---:|:---:|:---:|
|byte #1 |  1  |  1  |  0  |  1  |  -  |  -  |  -  |  -  |
|byte #2 |wait |wait |wait |wait |wait |wait |wait |wait |
|byte #3 |wait |wait |wait |wait |wait |wait |wait |wait |
|byte #4 |wait |wait |wait |wait |wait |wait |wait |wait |
|byte #5 |wait |wait |wait |wait |wait |wait |wait |wait |

- wait に次のシーケンス処理を実行する時間をリトルエンディアンで指定
- 時間は 22050 で 1 秒

> WAIT8 と WAIT16 はデータサイズを小さくする目的でバージョン 2 から追加された命令で、WAIT32 だけでも（データサイズが大きくなるものの）正常に動作できます。

## Sequence Example

例えば、次のような音声を再生する場合、

- チャンネル0 で 1 分音符を 1 回鳴らす
- チャンネル1 で 2 分音符を 2 回鳴らす
- チャンネル2 で 4 分音符を 4 回鳴らす
- 各音符は 50% で KEYOFF

シーケンスデータは次のようになります:

1. Sequence 1/8
  1. CH0 を KEYON
  1. CH1 を KEYON
  1. CH2 を KEYON
  1. WAIT (8部音符の長さ)
1. Sequence 2/8
  1. CH2 を KEYOFF
  1. WAIT (8部音符の長さ)
1. Sequence 3/8
  1. CH1 を KEYOFF
  1. CH2 を KEYON
  1. WAIT (8部音符の長さ)
1. Sequence 4/8
  1. CH2 を KEYOFF
  1. WAIT (8部音符の長さ)
1. Sequence 5/8
  1. CH0 を KEYOFF
  1. CH1 を KEYON
  1. CH2 を KEYON
  1. WAIT (8部音符の長さ)
1. Sequence 6/8
  1. CH2 を KEYOFF
  1. WAIT (8部音符の長さ)
1. Sequence 7/8
  1. CH1 を KEYOFF
  1. CH2 を KEYON
  1. WAIT (8部音符の長さ)
1. Sequence 8/8
  1. CH2 を KEYOFF
  1. WAIT (8部音符の長さ)
