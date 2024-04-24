#!/usr/bin/env bash
set -v


if [ ! -d "build" ]; then
    mkdir build
fi

cd build/ && rm -rf * && cmake .. && make #[-j4]
