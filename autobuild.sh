#!/usr/bin/env bash
set -x

#解压第三方依赖
if [ ! -d "thirdparty" ]; then
   tar xfzv thirdparty.tgz
fi
rm -f thirdparty.tgz

if [ ! -d "build" ]; then
    mkdir build
fi

cd build/ && rm -rf * && cmake .. && make -j4 && rm -rf *