//// GENERAL STUFF ////
#define REAL float
#define uint unsigned int
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
matrix *matrix_free(matrix *m);
matrix *matrix_check(matrix *m);
matrix *matrix_set_width(matrix *m, uint width);
REAL matrix_get(matrix *self, uint c, uint r);
void matrix_set(matrix *self, uint c, uint r, REAL v);
// input/output //
matrix *matrix_fscanf(FILE * f);
void matrix_fprintf(FILE *f, matrix *self);
void matrix_printf(matrix *self);
