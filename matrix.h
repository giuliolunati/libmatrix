//// GENERAL STUFF ////
#define REAL float
#define uint unsigned int

//// MATRIX ////
typedef struct {
  REAL *data;
  uint height;
  uint width;
  uint length;
  uint base;
  uint dx;
  uint dy;
} matrix;
// basic //
matrix *matrix_new();
void matrix_init(matrix *out, uint height, uint width);
void matrix_make(matrix *out, uint height, uint width, ...);
void matrix_clean(matrix *m);
void matrix_free(matrix *m);
void matrix_check(matrix *m);
REAL matrix_get(matrix *m, uint c, uint r);
void matrix_set(matrix *m, uint c, uint r, REAL v);
// input/output //
void matrix_fscanf(matrix *out, FILE *f);
void matrix_fprintf(FILE *f, matrix *m);
void matrix_printf(matrix *m);
// arithmetic //
void matrix_add_k(matrix *out, matrix *a, matrix *b, REAL k);
void matrix_add(matrix *out, matrix *a, matrix *b);
void matrix_sub(matrix *out, matrix *a, matrix *b);
void matrix_mul(matrix *out, matrix *a, matrix *b);
// statistical //
void matrix_sum_cols(matrix *out, matrix *m);
void matrix_set_width(matrix *m, uint width);
