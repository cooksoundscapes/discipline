#!/bin/bash

sho 

PLATFORM="{1:-PLATFORM_DESKTOP}"

git submodule update --init --recursive &&

# follow Raylib github's wiki for more info on building
cd raylib/src &&
make PLATFORM=$PLATFORM &&

cd ../../graphics &&
mkdir -p build lib &&
cp ../raygui/src/* lib/ &&
cp ../raylib/src/libraylib.a lib/ &&
cp ../raylib/src/raylib.h lib/ &&

cd build &&
cmake .. &&
make &&
cd ../../raylib/src &&
make clean
