#!/bin/sh
set -e
echo $1
cd $1
cmake -S . -B build-x86_64-windows -G Ninja -DPLATFORM=Desktop -DGRAPHICS=GRAPHICS_API_OPENGL_33 \
    -DCMAKE_BUILD_TYPE=Release -DCMAKE_TOOLCHAIN_FILE=cmake/toolchain-x86_64-w64-mingw32-windows.cmake

cmake --build build-x86_64-windows

mkdir -p temp

mkdir -p releases

cp -r build-x86_64-windows/bin temp/bin-x86_64-windows

cp build-x86_64-windows/bin/*.exe temp/bin-x86_64-windows/sokoban-gl33.exe

cmake -S . -B build-x86_64-windows -G Ninja -DPLATFORM=Desktop -DGRAPHICS=GRAPHICS_API_OPENGL_21 \
    -DCMAKE_BUILD_TYPE=Release -DCMAKE_TOOLCHAIN_FILE=cmake/toolchain-x86_64-w64-mingw32-windows.cmake

cmake --build build-x86_64-windows

cp build-x86_64-windows/bin/*.exe temp/bin-x86_64-windows/sokoban-gl21.exe

cmake -S . -B build-x86_64-windows -G Ninja -DPLATFORM=Desktop -DGRAPHICS=GRAPHICS_API_OPENGL_11 \
    -DCMAKE_BUILD_TYPE=Release -DCMAKE_TOOLCHAIN_FILE=cmake/toolchain-x86_64-w64-mingw32-windows.cmake

cmake --build build-x86_64-windows

cp build-x86_64-windows/bin/*.exe temp/bin-x86_64-windows/sokoban-gl11.exe

cd temp

zip -r ../releases/x86_64-windows.zip ./bin-x86_64-windows/