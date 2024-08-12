#!/bin/bash
git submodule update --init --recursive &&
cp raygui/src/* lib/ &&
mkdir build &&
cd build &&
cmake .. &
make