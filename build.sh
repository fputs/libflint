#!/bin/sh -e

mkdir -p build
cd build
cmake ..
make
cp compile_commands.json ..
cd 
