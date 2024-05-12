#!/usr/bin/env bash
set -x

if [ ! -d "build" ]; then
    mkdir build
fi

cd build/ && rm -rf * && cmake -DCMAKE_BUILD_TYPE=Debug .. && make
# cd build/ && cmake .. && make -j4