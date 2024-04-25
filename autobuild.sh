#!/usr/bin/env bash
set -u

if [ ! -d "build" ]; then
    mkdir build
fi

cd build/ && rm -rf * && cmake .. && make -j4 && rm -rf *
