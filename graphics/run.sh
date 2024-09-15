#!/bin/bash

pkill -f discipline_graphics || true &&
cd build &&
make &&
./discipline_graphics