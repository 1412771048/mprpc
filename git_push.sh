#!/usr/bin/env bash
set -x

git add .
git commit -m "push" 
# 检查git commit命令的返回状态
if [ $? -ne 0 ]; then
    echo "no update, don't need push"
else 
    git pull && git push
fi
