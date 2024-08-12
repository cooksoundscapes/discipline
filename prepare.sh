#!/bin/bash

git submodule update --init --recursive &&

PLATFORM = PLATFORM_DESKTOP

# follow Raylib github's wiki for more info on building
cd raylib/src &&
make PLATFORM=$PLATFORM &&
make clean &&

cd ../graphics &&
mkdir build lib &&
cp ../raygui/src/* lib/ &&
cp ../raylib/src/libraylib.a lib/ &&
cp ../raylib/src/raylib.h lib/

cd build &&
cmake .. &&
make