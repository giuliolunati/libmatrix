#include <stdio.h>
#include "matrix.h"

int main() {
  matrix *a = matrix_make(2, 3,
			1.0, 2.0, 0.0,
			0.0, 1.0, 2.0
	);
  matrix *b = matrix_make(2, 3,
			1.0, 2.0, -1.0,
			0.0, 4.0, 7.0
	);
  matrix_printf(a);
  matrix_printf(b);
  matrix_printf(matrix_add(a, b));
  matrix_printf(matrix_sub(a, b));
  matrix_printf(matrix_add_k(a, b, 2));
  return 0;
}
// vim: set fdm=indent sw=2 ts=2 nocindent autoindent indk=:
