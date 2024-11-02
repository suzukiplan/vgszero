# Joypad

ジョイパッドの入力状態をチェックします

![preview](preview.png)

## How to build

```zsh
sudo apt install build-essential libsdl2-dev libasound2 libasound2-dev
git clone https://github.com/suzukiplan/vgszero
cd vgszero/example/09_joypad-asm
make
```

## ROM structure

```
8KB x 2 banks = 16KB ROM
```

- Bank 0: program
- Bank 1: font.chr
