# VGS-Zero SDK for Steam

- Steam (Windows) で VGS-Zero のゲームを販売するための SDK です
- ビルド前に [Steamworks](https://partner.steamgames.com/) への加入と Steamworks SDK の入手が必要です
- ビルドは Visual Studio のコマンドライン環境（32bit）で行います

## How to Build

1. Steamworks SDK の `public` と `redistributable_bin` フォルダを [./sdk](./sdk) 以下にコピー
2. [./game.pkg](./game.pkg) を Steam で販売する game.pkg に差し替える
3. Visual Studio のコマンドライン環境（32bit）を起動してこのフォルダ配下で `nmake` コマンドを実行

## Licenses

販売時に同梱する [README.txt](./README.txt) に次のライセンス情報を必ず記載してください。

1. PicoJSON - a C++ JSON parser / serializer (2-clause BSD)
2. VGS-Zero SDK for Steam (MIT)
3. VGS-Zero Library for Z80 (MIT)
4. VGS-Zero (GPLv3)
5. SUZUKIPLAN - Z80 Emulator (MIT)

VGS-Zero のライセンスは GPLv3 ですが DLL 形式でバイナリが分割されているため、販売するゲームのソースコード（Z80）のライセンスに影響しません。
