//// GENERAL STUFF ////
#define REAL double
#define uint unsigned int

//// MATRIX ////
typedef struct {
  REAL *data;
  uint shared; // if 1 data is shared, don't free it!
  uint height;
  uint width;
  uint length;
  uint base;
  uint dx;
  uint dy;
} matrix;
// basic //
matrix *matrix_new();
void matrix_init(matrix *out, uint height, uint width, REAL val);
void matrix_make(matrix *out, uint height, uint width, ...);
void matrix_copy_shallow(matrix *out, matrix *m);
void matrix_copy_deep(matrix *out, matrix *m);
void matrix_clean(matrix *m);
void matrix_free(matrix *m);
void matrix_check(matrix *m);
REAL matrix_get(matrix *m, uint c, uint r);
void matrix_set(matrix *m, uint c, uint r, REAL v);
// input/output //
void matrix_fscanf(matrix *out, FILE *f);
void matrix_fprintf(FILE *f, matrix *m);
void matrix_printf(matrix *m);
// ops //
void matrix_add_k_to(matrix *a, matrix *b, REAL k);
void matrix_add_to(matrix *a, matrix *b);
void matrix_sub_from(matrix *a, matrix *b);
void matrix_mul(matrix *out, matrix *a, matrix *b);
void matrix_scale(matrix *m, REAL k);
void matrix_transpose(matrix *m);
// statistical //
void matrix_sum_cols(matrix *out, matrix *m);
void matrix_mean_cols(matrix *out, matrix *m);
void matrix_set_width(matrix *m, uint width);
// diagonalize //
void matrix_householder(matrix *m, REAL *u, uint side, uint skip);
void matrix_hessemberg(matrix *a, matrix *b);
#define matrix_tri_diag matrix_hessemberg
void matrix_tri_upper(matrix *a, matrix *b);
void matrix_tri_lower(matrix *a, matrix *b);
