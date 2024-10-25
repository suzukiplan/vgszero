# Global Variables

グローバル変数の利用例です

![preview](preview.png)

## How to build

```zsh
sudo apt install build-essential libsdl2-dev libasound2 libasound2-dev
git clone https://github.com/suzukiplan/vgszero
cd vgszero/example/02_global-asm
make
```

## ROM structure

```
8KB x 2 banks = 16KB ROM
```

- Bank 0: program
- Bank 1: font.chr
