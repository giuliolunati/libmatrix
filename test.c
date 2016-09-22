#include <stdio.h>
#include "matrix.h"

int main() {
  matrix *m = matrix_make(2, 3,
			1.0, 2.0, 0.0,
			0.0, 1.0, 2.0
	);
  if (m) matrix_printf(m);
  else {
		printf("failed\n");
		return 1;
	}
  return 0;
}
// vim: set fdm=indent sw=2 ts=2 nocindent autoindent indk=:
