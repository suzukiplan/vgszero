# Video Game Sound .BGM Binary Format - version 3

## はじめに

MML のテキストファイルを vgsmml コマンドでコンパイルすることができる VGS の音楽データ（.bgm）のバイナリ仕様について纏めます。

例えば、MIDI ファイルを .BGM 形式に変換できるツールの開発などに役立ててください。

## バージョン変更歴

1. [文献](https://suzukiplan.blogspot.com/2015/10/bgm-on-vgs.html)によると2011年～2012年ごろに仕様策定したらしい
2. 2023年（WAIT命令長を可変化する仕様変更を [tohovgs-pico](https://github.com/suzukiplan/tohovgs-pico) で実施）
3. 2024年（ライセンスと動作の軽量化のためLZ4圧縮を削除する仕様変更を VGS-Zero で実施）

本書ではバージョン 3 の仕様を記します。

## 全体構成

|Chunk name|Size    |Description|
|:---------|:------:|:----------|
|EyeCatch  |8 bytes |`VGSBGM-V` (固定)|
|LengthTime|4 bytes |曲の長さ (全 [WAIT](#wait) 値の合計)|
|LoopTime  |4 bytes |ループ起点 (先頭から [JUMP](#jump) 先ノートまでの [WAIT](#wait) 値の合計)|
|Notes     |variable|シーケンスデータ群（複数の[vgsnote](#vgsnote)）|

> `LengthTime` と `LoopTime` は、東方BGM on VGS が楽曲再生時に必要なデータなので、VGS-Zero 向けのゲームであれば必ずしも正しい値を設定しなくても正常に再生することができます。

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
- key は音程コード（0～84）を指定
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
