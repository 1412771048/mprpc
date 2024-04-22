#!/usr/bin/env bash


# 执行 git status 命令并计算输出的行数
line_count=$(git status | wc -l)

# 输出行数
echo "Git status returned $line_count lines."


# git status

# git add .
# git commit -m "update"
# if [ $? -ne 0 ]; then
#     echo "Git commit failed!"
# fi



# On branch gyl
# Your branch is ahead of 'origin/gyl' by 1 commit.
#   (use "git push" to publish your local commits)

# nothing to commit, working tree clean


# On branch gyl
# Your branch is up to date with 'origin/gyl'.

# nothing to commit, working tree clean

# Untracked files:
#   (use "git add <file>..." to include in what will be committed)
#         git_push.sh