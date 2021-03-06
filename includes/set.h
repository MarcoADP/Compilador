#ifndef SET_H
#define SET_H

#include <stdio.h>
#include <unistd.h>
#include <stdbool.h>

#define SET_TAMANHO_MAX 50

struct set {
  int tamanho;
  char elementos[SET_TAMANHO_MAX];
};

void set_init(struct set *s);
bool set_add(struct set *s, char elem);
bool set_full(struct set *s);
bool set_contains(struct set *s, char elem);
void set_print(struct set *s);

#endif
