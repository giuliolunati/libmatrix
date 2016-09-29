#include <stdio.h>
#include "matrix.h"

int main() {
	matrix *a = matrix_new();
	matrix *b = matrix_new();
	matrix *c = matrix_new();
	matrix *d = matrix_new();
	matrix_make(a, 4, 4,
		1.0, 2.0, 3.0, 4.0,
		0.0, 9.0, 6.0, 8.0,
		7.0, 5.0, 3.0, 1.0,
		9.0, 3.0, 2.0, 6.0
	);
	printf("a: ");
	matrix_printf(a);
	matrix_init(b, 4, 4, 1);
	matrix_tri_lower(a, b);
	printf("lower triangular: ");
	matrix_printf(a);
	printf("q: ");
	matrix_printf(b);
	matrix_transpose(b);
	matrix_mul(d, a, b);
	matrix_copy_deep(a, d);
	printf("a: ");
	matrix_printf(a);
	matrix_init(b, 4, 4, 1);
	matrix_tri_upper(a, b);
	printf("upper triangular: ");
	matrix_printf(a);
	printf("q: ");
	matrix_printf(b);
	matrix_transpose(b);
	matrix_mul(d, b, a);
	matrix_copy_deep(a, d);
	printf("a: ");
	matrix_printf(a);
	matrix_init(b, 4, 4, 1);
	matrix_hessemberg(a, b);
	printf("up hessemberg: ");
	matrix_printf(a);
	printf("q: ");
	matrix_printf(b);
	matrix_copy_deep(c, b);
	matrix_transpose(c);
	matrix_mul(d, a, c);
	matrix_mul(a, b, d);
	printf("a: ");
	matrix_printf(a);
	return 0;
}
// vim: set fdm=indent sw=2 ts=2 nocindent autoindent indk=:
