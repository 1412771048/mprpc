#!/usr/bin/env bash
set -x

#把第三方依赖打包上传更小
if [ ! -f "thirdparty.tgz" ]; then
    tar cfzv thirdparty.tgz thirdparty/*
fi

rm -rf thirdparty/
git add .
git commit -m "push" 
# 检查git commit命令的返回状态
if [ $? -ne 0 ]; then
    echo "no update, don't need push"
else 
    git pull && git push
fi
