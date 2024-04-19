#!/usr/bin/env bash
set -e


if [ ! -d "build" ]; then
    mkdir build
fi

cd build/ && rm -rf * && cmake .. && make #[-j4]
