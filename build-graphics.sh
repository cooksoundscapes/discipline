#!/bin/bash

RAYLIB_PLATFORM="${1:-PLATFORM_DESKTOP}"

echo "Initializing Git submodules..." &&
git submodule update --init --recursive &&

# follow Raylib github's wiki for more info on building
echo "Building Raylib from source..." &&
cd raylib/src &&
make PLATFORM=$RAYLIB_PLATFORM &&

echo "#---------------------------------------------------------------#
Gnerating graphics build & lib directories and adding compiled libs & headers...
#---------------------------------------------------------------#" &&
cd ../../graphics &&
mkdir -p build lib &&
cp ../raygui/src/* lib/ &&
cp ../raylib/src/libraylib.a lib/ &&
cp ../raylib/src/raylib.h lib/ &&

echo "Building discipline_graphics binary..." &&
cd build &&
cmake .. &&
make &&

echo "Cleaning up raylib compilation garbage..." &&
cd ../../raylib/src &&
make clean