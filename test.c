#include <stdio.h>
#include "matrix.h"

int main() {
	matrix *a = matrix_new();
	matrix *b = matrix_new();
	matrix *c = matrix_new();
  matrix_make(a, 4, 2,
			1.0, 2.0,
			0.0, 1.0,
			1.0, 2.0,
			0.0, 4.0
	);
  matrix_printf(a);
	matrix_sum_cols(b, a);
  matrix_printf(b);
	matrix_clean(b);
	matrix_make(b, 2, 1, -1, 1);
	matrix_printf(b);
	matrix_mul(c, a , b);
	matrix_printf(c);
  return 0;
}
// vim: set fdm=indent sw=2 ts=2 nocindent autoindent indk=:
