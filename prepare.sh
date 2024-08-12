#!/bin/bash

# attention! build Raylib 1st and copy binary & header to /lib. each platform is different, probably

git submodule update --init --recursive &&
cp raygui/src/* lib/ &&
mkdir build &&
cd build &&
cmake .. &&
make