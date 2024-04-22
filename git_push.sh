#!/usr/bin/env bash

# 检查git status是否返回4行
if [ $(git status | wc -l) != 4 ]; then
    git add . 
    git commit -m "update" || true  #不论commit是否成功都向下走
    git pull 
    git push
else
    echo "no update, don't need push"
fi
