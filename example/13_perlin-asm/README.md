# Perlin Noise

パーリンノイズという地形のようなデータを自動生成に便利な HAGe の利用例です

> パーリンノイズに関する詳細は以下の記事でご確認ください。
> 
> - [実装レベルで簡単に理解できる Qiita の記事](https://qiita.com/gis/items/ba7d715901a0e572b0e9)
> - [原理を理解できる詳しい記事](https://postd.cc/understanding-perlin-noise/)

![preview.png](preview.png)

- カーソルで上下左右にスクロールできます
- スタートボタンを押せば乱数シードを更新して再生成します

## How to build

```zsh
sudo apt install build-essential libsdl2-dev libasound2 libasound2-dev
git clone https://github.com/suzukiplan/vgszero
cd vgszero/example/13_perlin-asm
make
```

## ROM structure

```
8KB x 2 banks = 16KB ROM
```

- Bank 0: program
- Bank 1: font.chr
