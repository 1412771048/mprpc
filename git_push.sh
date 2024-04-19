#!/usr/bin/env bash
set -x

#把第三方依赖打包上传更小
if [ ! -f "thirdparty.tgz" ]; then
    tar cfzv thirdparty.tgz thirdparty/*
fi

rm -rf thirdparty/

git add . && git commit -m "push" && git pull && git push