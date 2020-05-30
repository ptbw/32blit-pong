# 32blit-pong
Pong for the 32blit (Very loosely based on https://github.com/chaficnajjar/21st-century-pong)

**------ Once off prep 32Blit with firmware -------**
```
cd 32blit-beta
mkdir build.stm32
cd build.stm32
cmake … -DCMAKE_TOOLCHAIN_FILE=…/32blit.toolchain
make flash-loader
sudo dfu-util -a 0 -s 0x08000000 -D firmware/flash-loaderflash-loader.bin
```
**------ Once off build flasher --------**
```
mkdir build
cd build
cmake ..
make 32Blit
sudo cp tools/src/32Blit /usr/local/bin
```
--------------------------------------------------------------------------------------

**------ Specific to Pong game ------**
```
Git clone to the directory of choice
cd pong
Then edit CMakeLists.txt to reflect path to 32blit-beta/32blit.cmake
```
-- Target PC --
```
mkdir build
cd build
cmake .. 
make
```

-- Target 32Blit --
```
mkdir build.stm32
cd build.stm32
cmake … -DCMAKE_TOOLCHAIN_FILE=~/32blit-beta/32blit.toolchain    # or correct location
make
make pong.flash          # with 32Blit plugged in
```
