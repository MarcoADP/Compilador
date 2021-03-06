#ifndef PRODUCAO_H
#define PRODUCAO_H

#include <stdio.h>
#include <string.h>
#include <stdbool.h>

#define PRODUCOES_TAMANHO_MAX 1000
#define REGRA_TAMANHO_MAX 50

struct regra {
  int tamanho;
  char elementos[REGRA_TAMANHO_MAX];
};

struct producoes {
  int tamanho;
  struct regra regras[PRODUCOES_TAMANHO_MAX];
};

void producoes_init(struct producoes *producoes);
bool producoes_add(struct producoes *producoes, struct regra *producao);
bool producoes_add2(struct producoes *producoes, struct regra *producao, char NT);
bool producoes_full(struct producoes *producoes);
void producoes_print(struct producoes *producoes);
void producoes_print_formatado(struct producoes *producoes);
void producoes_copy(struct producoes *origem, struct producoes *destino);
bool producoes_remove(struct producoes *producoes, struct regra *producao);
void producoes_get(char NT, struct producoes *origem, struct producoes *destino);
void regra_init(struct regra *regra);
bool regra_add(struct regra *regra, char elem);
bool regra_full(struct regra *regra);
void regra_print(struct regra *regra);
bool regra_equal(struct regra *r1, struct regra *r2);
char *formata_producao(char *producao);

#endif
