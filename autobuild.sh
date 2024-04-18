#!/usr/bin/env bash
set -x

cd build/ && rm -rf * && cmake .. && make #[-j4]