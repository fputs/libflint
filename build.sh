#!/bin/sh -e

# For building outside of CLion

mkdir -p build
cd build
cmake ..
make
cp compile_commands.json ..
cd
