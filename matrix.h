#define REAL float
#define uint unsigned int
typedef struct {
  uint height;
  uint width;
  REAL *data;
  uint length;
  uint base;
  uint dx;
  uint dy;
} matrix;
matrix *matrix_fscanf(FILE * f);
void matrix_fprintf(FILE *f, matrix *self);
