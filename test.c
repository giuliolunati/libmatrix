#include <stdio.h>
#include "matrix.h"

int main() {
  FILE *f = stdin;
  matrix *m = matrix_fscanf(f);
  //matrix_set_width(m, 7);
  if (m) matrix_printf(m);
  else printf("failed\n");
  return 1;
}
// vim: set fdm=indent sw=2 ts=2 nocindent autoindent indk=:
