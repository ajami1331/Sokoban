#!/bin/sh
set -e
echo $1
cd $1
cmake -S . -B build-i686-windows -G Ninja -DPLATFORM=Desktop \
    -DCMAKE_BUILD_TYPE=Release -DCMAKE_TOOLCHAIN_FILE=cmake/toolchain-i686-w64-mingw32-windows.cmake
cmake --build build-i686-windows

mkdir -p temp

mkdir -p releases

cp -r build-i686-windows/bin temp/bin-i686-windows

zip -r releases/i686-windows.zip temp/bin-i686-windows