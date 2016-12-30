#include "err.h"

char buffer_err[512];

void errexit(char *msg) {
  perror(msg);
  exit(EXIT_FAILURE);
}
