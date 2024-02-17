# VGS-Zero SDK for Steam

- Steam (Windows) で VGS-Zero のゲームを販売するための SDK です
- ビルド前に [Steamworks](https://partner.steamgames.com/) への加入と Steamworks SDK の入手が必要です
- ビルドは Visual Studio のコマンドライン環境（32bit）で行います
- Visual Studio のバージョンは不問ですがなるべく新しい方が良いかもしれません

## How to Build

1. Steamworks で App クレジットを購入して AppID を入手
2. Steamworks SDK をダウンロードして `public` と `redistributable_bin` フォルダを [./sdk](./sdk) 以下にコピー
3. 販売する `game.pkg` をこのフォルダ配下に配置
4. `steam_appid.txt` ファイルをこのフォルダ配下に作成して AppID を記述
5. [./game_actions_X.vdf](./game_actions_X.vdf) を `C:\Program Files (x86)\Steam\controller_config` 以下にコピーして `X` の箇所を AppID にリネーム（※ `controller_config` フォルダが無い場合は作成してください）
6. アイコンファイルを差し替え（[./src/icon016.ico](./src/icon016.ico), [./src/icon032.ico](./src/icon032.ico), [./src/icon256.ico](./src/icon256.ico)）
7. Visual Studio のコマンドライン環境（32bit）を起動してこのフォルダ配下へ `cd` して `nmake` コマンドを実行してビルド

ビルドが成功すると `game.pkg` のゲームが起動します。

`release` ディレクトリ以下に Steam で配信するファイルが出力されているので、Steam へビルドを登録する時は `release` 以下のファイルを zip に圧縮したものを用いてください。（zip ファイルが 2MB を超える場合は `steamcmd` でビルドを登録してください）

[./src](./src) 以下のファイルは自由に改変できますが、改変したファイルは GPLv3 ライセンスに基づきソースコードの開示義務が生じる点をご注意ください。

## Trouble Shooting

- Q. ビルドが成功したが起動しない
  - A. steam_appid.txt が正しいかご確認ください
- Q. 落ちるetc
  - A. log.txt をご確認ください
- Q. ジョイパッドが効かない
  - A. ジョイパッドの入力は Steam クライアントから起動して SteamInput の設定でレイアウトを指定することで利用できるようになります。詳細は Steamworks で公開されている SteamInput のマニュアルをご確認ください。
- Q. バグを見つけた
  - A. [issues](https://github.com/suzukiplan/vgszero/issues) でチケットを切ってください
- Q. VisualStudio の IDE で動かしたい
  - A. (　･ω･)もきゅ？

## Licenses

販売時に同梱する [README.txt](./README.txt) に次のライセンス情報を必ず記載してください。

1. PicoJSON - a C++ JSON parser / serializer (2-clause BSD)
2. VGS-Zero Library for Z80 (MIT)
3. VGS-Zero (GPLv3)
4. SUZUKIPLAN - Z80 Emulator (MIT)

VGS-Zero のライセンスは GPLv3 ですが game.pkg のバイナリが分割されているため、販売するゲームのソースコード（Z80）のライセンスには影響しません。
