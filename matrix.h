//// GENERAL STUFF ////
#define REAL float
#define uint unsigned int

#define MAX(a, b) ((a) > (b) ? (a) : (b))
#define MIN(a, b) ((a) < (b) ? (a) : (b))

void error(char *msg);

//// MATRIX ////
typedef struct {
  uint height;
  uint width;
  REAL *data;
  uint length;
  uint base;
  uint dx;
  uint dy;
} matrix;
// basic //
matrix *matrix_init(uint height, uint width);
matrix *matrix_make(uint height, uint width, ...);
matrix *matrix_free(matrix *m);
matrix *matrix_check(matrix *m);
matrix *matrix_set_width(matrix *m, uint width);
REAL matrix_get(matrix *self, uint c, uint r);
void matrix_set(matrix *self, uint c, uint r, REAL v);
// input/output //
matrix *matrix_fscanf(FILE * f);
void matrix_fprintf(FILE *f, matrix *self);
void matrix_printf(matrix *self);
// arithmetic //
matrix *matrix_add_k(matrix *a, matrix *b, REAL k);
matrix *matrix_add(matrix *a, matrix *b);
matrix *matrix_sub(matrix *a, matrix *b);
