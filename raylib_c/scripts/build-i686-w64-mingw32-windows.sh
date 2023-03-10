#!/bin/sh
set -e
echo $1
cd $1
cmake -S . -B build-i686-windows -G Ninja -DPLATFORM=Desktop -DGRAPHICS=GRAPHICS_API_OPENGL_33 \
    -DCMAKE_BUILD_TYPE=Release -DCMAKE_TOOLCHAIN_FILE=cmake/toolchain-i686-w64-mingw32-windows.cmake
cmake --build build-i686-windows

mkdir -p temp

mkdir -p releases

cp -r build-i686-windows/bin temp/bin-i686-windows
cp build-i686-windows/bin/*.exe temp/bin-i686-windows/sokoban-gl33.exe

cmake -S . -B build-i686-windows -G Ninja -DPLATFORM=Desktop -DGRAPHICS=GRAPHICS_API_OPENGL_21 \
    -DCMAKE_BUILD_TYPE=Release -DCMAKE_TOOLCHAIN_FILE=cmake/toolchain-i686-w64-mingw32-windows.cmake
cmake --build build-i686-windows

cp build-i686-windows/bin/*.exe temp/bin-i686-windows/sokoban-gl21.exe

cmake -S . -B build-i686-windows -G Ninja -DPLATFORM=Desktop -DGRAPHICS=GRAPHICS_API_OPENGL_11 \
    -DCMAKE_BUILD_TYPE=Release -DCMAKE_TOOLCHAIN_FILE=cmake/toolchain-i686-w64-mingw32-windows.cmake
cmake --build build-i686-windows

cp build-i686-windows/bin/*.exe temp/bin-i686-windows/sokoban-gl11.exe

cd temp

zip -r ../releases/i686-windows.zip ./bin-i686-windows/