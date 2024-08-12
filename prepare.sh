#!/bin/bash

RAYLIB_PLATFORM="${1:-PLATFORM_DESKTOP}"

echo "Initializing Git submodules..." &&
git submodule update --init --recursive &&

# follow Raylib github's wiki for more info on building
echo "Building Raylib from source..." &&
cd raylib/src &&
make PLATFORM=$RAYLIB_PLATFORM &&

echo "Gnerating graphics build & lib directories and adding compiled libs & headers..." &&
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
make clean &&

echo "Building Graphics successful, entering PiGPIOd directory;" &&
#-----------------------------
echo "Building PIGPIOD library..."
cd ../../pigpio &&
make &&

echo "Generating hardware build & lib directories & adding compiled libs & headers..." &&
cd ../hardware &&
mkdir -p lib build &&
cp ../pigpio/libpigpio.so lib/ &&
cp ../pigpio/libpigpiod_if2.so lib/ &&
cp ../pigpio/libpigpiod_if2.so.1 lib/ &&
cp ../pigpio/pigpio.h lib/ &&
cp ../pigpio/pigpiod_if2.h lib/ &&

echo "Building hardware binary..."
cd build &&
cmake .. &&
make &&

echo "Cleaning pigpio compilation garbage..."
cd ../../pigpio &&
make clean &&

echo "Successfully built hardware binary. Should be ready to go!"



