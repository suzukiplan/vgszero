# 1024 Patterns Mode

1024 パターンを表示する機能を用いる例です。

![preview.png](preview.png)

カーソルで上下左右にスクロールできます。

## How to build

```zsh
sudo apt install build-essential libsdl2-dev libasound2 libasound2-dev
git clone https://github.com/suzukiplan/vgszero
cd vgszero/example/14_1024ptn-asm
make
```

## ROM structure

```
8KB x 2 banks = 16KB ROM
```

- Bank 0: program
- Bank 1: image.chr
