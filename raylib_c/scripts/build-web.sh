#!/bin/sh
set -e
echo $1
cd $1
emcmake cmake -S . -B cmake-build-empscripten -G Ninja -DPLATFORM=Web -DCMAKE_BUILD_TYPE=Release
cmake --build cmake-build-empscripten