#!/bin/bash

# Get the directory where this script is located
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"

# Use the correct paths relative to this script's location
cd "$SCRIPT_DIR"

pkill -f discipline_graphics || true &&
cd build &&
make &&
./discipline_graphics