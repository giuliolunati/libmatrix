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
	matrix_scale(a, 2);
  matrix_printf(a);
  return 0;
}
// vim: set fdm=indent sw=2 ts=2 nocindent autoindent indk=:
