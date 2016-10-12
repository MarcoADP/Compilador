#!/usr/bin/env bash

BINARY_PATH="bin"

if [ ! -d "$BINARY_PATH" ]; then
  mkdir "$BINARY_PATH"
fi

gcc -o "$BINARY_PATH"/compilador compilador.c
