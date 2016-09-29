#include <stdio.h>
#include "matrix.h"

int main() {
	matrix *a = matrix_new();
	matrix_init(a, 8, 4, 1);
  matrix_printf(a);
  return 0;
}
// vim: set fdm=indent sw=2 ts=2 nocindent autoindent indk=:
