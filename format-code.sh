#!/bin/zsh

FORMAT=/Users/kevin/github/dotfiles/clang-format.yml
clang-format -style=file:$FORMAT -i src/**/*.c