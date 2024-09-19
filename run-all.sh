#!/bin/bash

./graphics/run.sh &
(
    sleep 1 &&
    ./system_handler/run.sh
)