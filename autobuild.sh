#!/usr/bin/env bash
set -e

if [ ! -d "build" ]; then
    mkdir build
fi

# cd build/ && rm -rf * && cmake -DCMAKE_BUILD_TYPE=Debug .. && make -j16
cd build/ && rm -rf * && cmake .. && make -j16