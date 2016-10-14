#ifndef PRODUCAO_H
#define PRODUCAO_H
#define PRODUCOES_TAMANHO_MAX 10
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include "set.h"

struct producao {
  int tamanho;
  struct set regras[PRODUCOES_TAMANHO_MAX];
};

void producoes_init(struct producao *producoes);
bool producoes_add(struct producao *producoes, struct set *producao);
bool producoes_full(struct producao *producoes);
void producoes_print(struct producao *producoes);
#endif
