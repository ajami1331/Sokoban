#!/bin/sh
set -e
echo $1
cd $1
cmake -S . -B build-x86_64-windows -G Ninja -DPLATFORM=Desktop \
    -DCMAKE_BUILD_TYPE=Release -DCMAKE_TOOLCHAIN_FILE=cmake/toolchain-x86_64-w64-mingw32-windows.cmake
cmake --build build-x86_64-windows

mkdir -p temp

mkdir -p releases

cp -r build-x86_64-windows/bin temp/bin-x86_64-windows

cd temp

zip -r ../releases/x86_64-windows.zip ./bin-x86_64-windows/