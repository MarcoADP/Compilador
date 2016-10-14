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
  set_print(s);
  return true;
}

bool set_full(struct set *s) {
  return s->tamanho == SET_TAM_MAX;
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
  printf("\n\nSET: \nSize: %d\n", s->tamanho);
  write(1, s->elementos, s->tamanho);
  printf("\n\n");
}
