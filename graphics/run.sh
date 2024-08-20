#!/bin/bash

pkill -f discipline_graphics || true &&
cd build &&
make &&
{
    ./discipline_graphics &
    sleep .4
    jack_connect pure_data:output_4 discipline:input_1
}