#!/bin/sh 
apt-get update && apt-get install -y \
    libxinerama-dev \
    libxrandr-dev \
    libxcursor-dev \
    libxi-dev \
    libgl1-mesa-dev

apt-get update && apt-get install -y mingw-w64

apt-get update && apt-get install -y cmake ninja-build

apt-get update && apt-get install -y file zip unzip