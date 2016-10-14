#!/usr/bin/env bash

BINARY_PATH="bin"

if [ ! -d "$BINARY_PATH" ]; then
  mkdir "$BINARY_PATH"
fi

gcc -std=gnu99 -g -o "$BINARY_PATH"/compilador compilador.c producao.c set.c err.c
