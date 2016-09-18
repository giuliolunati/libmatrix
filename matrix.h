#define REAL float
#define uint unsigned int
typedef struct {
  uint height;
  uint width;
  REAL *data;
} matrix;
matrix *matrix_fscanf(FILE * f);
void matrix_fprintf(FILE *f, matrix *self);
