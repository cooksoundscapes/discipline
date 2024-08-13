#!/bin/bash

echo "Building PIGPIOD library..."
cd ../../pigpio &&
make &&

echo "#---------------------------------------------------------------#
Generating hardware build & lib directories & adding compiled libs & headers...
#---------------------------------------------------------------#" &&
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



