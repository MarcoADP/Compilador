#include "producao.h"

void producoes_init(struct producao *producoes) {
  producoes->tamanho = 0;
  for (int i = 0; i < PRODUCOES_TAMANHO_MAX; i++) {
    set_init(&producoes->regras[i]);
  }
}

bool producoes_add(struct producao *producoes, struct set *producao) {
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

bool producoes_full(struct producao *producoes) {
  return producoes->tamanho == PRODUCOES_TAMANHO_MAX;
}

void producoes_print(struct producao *producoes) {
  for (size_t i = 0; i < producoes->tamanho; i++) {
    set_print(&producoes->regras[i]);
  }
}
