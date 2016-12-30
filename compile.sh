#!/usr/bin/env bash

BINARY_PATH="bin"

if [ ! -d "$BINARY_PATH" ]; then
  mkdir "$BINARY_PATH"
fi

gcc -std=gnu99 -Wall -Wno-unused-result -g -o "$BINARY_PATH"/recursao recursao_esquerda.c includes/producao.c includes/set.c includes/err.c
