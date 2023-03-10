#!/bin/bash
set -e
echo $1
cd $1

declare -a opengl_versions=("43" "33" "21" "11")

copied_once=false
app_prefix="sokoban-gl"

for gl_ver in "${opengl_versions[@]}"
do
    echo "Building OpenGL $gl_ver"
    cmake -S . -B build-i686-windows -G Ninja -DPLATFORM=Desktop -DGRAPHICS=GRAPHICS_API_OPENGL_$gl_ver \
    -DCMAKE_BUILD_TYPE=Release -DCMAKE_TOOLCHAIN_FILE=cmake/toolchain-i686-w64-mingw32-windows.cmake

    cmake --build build-i686-windows

    if [ "$copied_once" = false ] ; then
        mkdir -p temp
        mkdir -p releases
        cp -r build-i686-windows/bin temp/bin-i686-windows
        copied_once=true
    fi
    cp build-i686-windows/bin/*.exe temp/bin-i686-windows/$app_prefix$gl_ver.exe
done

cd temp

zip -r ../releases/i686-windows.zip ./bin-i686-windows/