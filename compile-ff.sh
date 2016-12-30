#!/usr/bin/env bash

BINARY_PATH="bin"

if [ ! -d "$BINARY_PATH" ]; then
  mkdir "$BINARY_PATH"
fi

gcc -std=gnu99 -Wall -Wno-unused-result -g -o "$BINARY_PATH"/first-follow first-follow.c includes/producao.c includes/set.c includes/err.c
