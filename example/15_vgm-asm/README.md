# NSF

- NSF (2A03) 形式の BGM を再生・ポーズ・再開・フェードアウト
- 効果音を再生
- ジョイパッドで操作します

![preview](preview.png)

## How to build

```zsh
sudo apt install build-essential libsdl2-dev libasound2 libasound2-dev
git clone https://github.com/suzukiplan/vgszero
cd vgszero/example/15_nsf-asm
make
```

## ROM structure

### Program

- Bank 0: program
- Bank 1: font.chr

### BGM

- BGM 0: [./bgm.nsf](./bgm.nsf)

### Sound Effect structure

- SE 0: [./se_move.wav](./se_move.wav)
- SE 1: [./se_enter.wav](./se_enter.wav)
