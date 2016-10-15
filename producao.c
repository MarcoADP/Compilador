#include "producao.h"

void producoes_init(struct producoes *producoes) {
  producoes->tamanho = 0;
  for (int i = 0; i < PRODUCOES_TAMANHO_MAX; i++) {
    regra_init(&producoes->regras[i]);
  }
}

bool producoes_add(struct producoes *producoes, struct regra *producao) {
  if (producoes_full(producoes)) {
    perror("Produções cheio!\n");
    return false;
  }
  producao->elementos[producao->tamanho] = '\0';
  printf("producao(%s) ", producao->elementos);
  strcpy(producoes->regras[producoes->tamanho].elementos, producao->elementos);
  producoes->regras[producoes->tamanho++].tamanho = producao->tamanho;
  return true;
}

bool producoes_full(struct producoes *producoes) {
  return producoes->tamanho == PRODUCOES_TAMANHO_MAX;
}

void producoes_print(struct producoes *producoes) {
  printf("PRODUCOES: {tamanho: %d,\n", producoes->tamanho);
  for (int i = 0; i < producoes->tamanho; i++) {
    regra_print(&producoes->regras[i]);
  }
  printf("}\n");  
}

void regra_init(struct regra *r) {
  r->tamanho = 0;
}

bool regra_add(struct regra *r, char elem) {
  if (regra_full(r)) {
    perror("regra elementos cheio!\n");
    return false;
  }
  r->elementos[r->tamanho++] = elem;
  return true;
}

bool regra_full(struct regra *r) {
  return r->tamanho == REGRA_TAMANHO_MAX;
}

bool regra_contains(struct regra *r, char elem) {
  for (int i = 0; i < r->tamanho; i++) {
    if (r->elementos[i] == elem) {
      return true;
    }
  }
  return false;
}

void regra_print(struct regra *r) {
  r->elementos[r->tamanho] = '\0';
  printf("REGRA: {tamanho: %d, \"%s\"}\n", r->tamanho, r->elementos);
}
