#!/usr/bin/env bash


# 检查git status是否返回4行
if [ $(git status | wc -l) != 4 ]; then
    git add . && git commit -m "update" && git pull && git push
else
    echo "no update, don't need push"
fi
