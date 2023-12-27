# VGS0 for SDL2

## How to Build & Execute

### macOS

```bash
# NOTE: XCode CLI Tools required

# install SDL2
brew install sdl2

# download repository
git clone https://github.com/suzukiplan/vgszero

# move to this dir
cd vgszero/src/sdl2

# build
make

# execute
./vgszero ../../example/01_hello/game.pkg
```

### Linux

```bash
# install GNU Make, GCC and other
sudo apt install build-essential

# install SDL2
sudo apt-get install libsdl2-dev

# install ALSA
sudo apt-get install libasound2
sudo apt-get install libasound2-dev

# download repository
git clone https://github.com/suzukiplan/vgszero

# move to this dir
cd vgszero/src/sdl2

# build
make

# execute
./vgszero ../../example/01_hello/game.pkg
```

## Usage

```
usage: vgs0 /path/to/game.pkg ....... Specify game package to be used
            [-g { None .............. GPU: Do not use
                | OpenGL ............ GPU: OpenGL <default>
                | Vulkan ............ GPU: Vulkan
                | Metal ............. GPU: Metal
                }]
            [-f] .................... Full Screen Mode
```