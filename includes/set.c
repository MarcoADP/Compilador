#include "set.h"

void set_init(struct set *s) {
  s->tamanho = 0;
}

bool set_add(struct set *s, char elem) {
  if (set_full(s)) {
    perror("set cheio!\n");
    return false;
  }
  if (!set_contains(s, elem)) {
    s->elementos[s->tamanho++] = elem;
  }
  return true;
}

bool set_full(struct set *s) {
  return s->tamanho == SET_TAMANHO_MAX;
}

bool set_contains(struct set *s, char elem) {
  for (int i = 0; i < s->tamanho; i++) {
    if (s->elementos[i] == elem) {
      return true;
    }
  }
  return false;
}

void set_print(struct set *s) {
  s->elementos[s->tamanho] = '\0';
  printf("SET: {tamanho: %d, \"%s\"}\n", s->tamanho, s->elementos);
}
