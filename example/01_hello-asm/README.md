# Hello, World

[01_hello](../01_hello/) をアセンブリ言語のみで実装したものです。

![preview](preview.png)

## How to build

### Pre-request

- GNU make
- clang (for building the depended tools)
- [z88dk](https://z88dk.org/site/)
  - require `z80asm` command in this example

### Build

```zsh
git clone https://github.com/suzukiplan/vgszero
cd vgszero/example/01_hello-asm
make
```
