#!/usr/bin/env bash
set -x

if [ ! -d "../../build" ]; then
    mkdir build
fi

cd ../../build/ && rm -rf * && cmake .. && make -j4
cd ../output/bin/ && ./provider -i test.ini