#include <stdlib.h>
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
  strcpy(producoes->regras[producoes->tamanho].elementos, producao->elementos);
  producoes->regras[producoes->tamanho++].tamanho = producao->tamanho;
  return true;
}

bool producoes_add2(struct producoes *producoes, struct regra *producao, char NT) {
  if (producoes_full(producoes)) {
    perror("Produções cheio!\n");
    return false;
  }

  producao->elementos[producao->tamanho] = '\0';

  int i = producoes->tamanho - 1;
  while (producoes->regras[i].elementos[0] != NT) {
    producoes->regras[i+1] = producoes->regras[i];
    i--;
  }

  strcpy(producoes->regras[i+1].elementos, producao->elementos);
  producoes->regras[i+1].tamanho = producao->tamanho;
  producoes->tamanho++;
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

void producoes_print_formatado(struct producoes *producoes) {
  for (int i = 0; i < producoes->tamanho; i++) {
    char *formatado = formata_producao(producoes->regras[i].elementos);
    printf("%s\n", formatado);
    free(formatado);
  }
}

void producoes_copy(struct producoes *origem, struct producoes *destino) {
  for (int i = 0; i < origem->tamanho; ++i) {
    producoes_add(destino, &origem->regras[i]);
  }
}

bool producoes_remove(struct producoes *producoes, struct regra *producao) {
  bool equal = false;
  for (int i = 0; i < producoes->tamanho; ++i) {
    if (regra_equal(&producoes->regras[i], producao) || equal) {
      equal = true;
      if (i == (producoes->tamanho - 1)) break;
      producoes->regras[i] = producoes->regras[i+1];
    }
  }
  if (equal) {
    producoes->tamanho--;
  }
  return equal;
}

void producoes_get(char NT, struct producoes *origem, struct producoes *destino) {
  for (int i = 0; i < origem->tamanho; ++i) {
    if (origem->regras[i].elementos[0] == NT) {
      producoes_add(destino, &origem->regras[i]);
    }
  }
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

bool regra_equal(struct regra *r1, struct regra *r2) {
  if (r1->tamanho != r2->tamanho) {
    return false;
  }

  for (int i = 0; i < r1->tamanho; ++i) {
    if (r1->elementos[i] != r2->elementos[i]) {
      return false;
    }
  }
  return true;
}

char *formata_producao(char *producao) {
  char *formatado = malloc(sizeof(char) * REGRA_TAMANHO_MAX);
  sprintf(formatado, "%c -> %s", producao[0], &producao[1]);
  return formatado;
}
