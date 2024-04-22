#!/usr/bin/env bash


# 检查行数是否不等于4
if [ $(git status | wc -l) != 4 ]; then
    git add . && git commit -m "update" && git pull && git push
else
    echo "no update, don't need push"
fi
