#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <math.h>
#include <stdarg.h>
#include "matrix.h"

void error(char *msg) {
	fprintf(stderr, msg);
	exit(1);
}

char *wrong_dim = "Wrong dimension\n";

matrix *matrix_check(matrix *m) {
	long int w = m->dx * (m->width - 1);
	long int i = m->base;
	if (i >= m->length) return NULL;
	i += w;
	if (i < 0 || i >= m->length) return NULL;
	i += m->dy * (m->height - 1);
	if (i < 0 || i >= m->length) return NULL;
	i -= w;
	if (i < 0 || i >= m->length) return NULL;
	return m;
}

matrix *matrix_free(matrix *m) {
	if (! m) return;
	if (m->data) free(m->data);
	free(m);
	return NULL;
}

matrix *matrix_init(uint height, uint width) {
	matrix *m = malloc(sizeof(matrix));
	if (!m) return NULL;
	m->data = malloc(height * width * sizeof(REAL));
	if (!m->data) return matrix_free(m);
	m->height = height;
	m->dy = m->width = width;
	m->dx = 1;
	m->base = 0;
	m->length = height * width;
	return m;
}

matrix *matrix_make(uint height, uint width, ...) {
	matrix *m = matrix_init(height, width);
	if (!m) return NULL;
	REAL *p;
	REAL *end = m->data + height * width;
	va_list ap;
	va_start(ap, width);
	for (p = m->data; p < end; p++) {
		*p = va_arg(ap, double);
	}
	va_end(ap);
	return m;
}

matrix *matrix_set_width(matrix *m, uint width) {
	long int t;
	// base + dx * (w - 1) < length
	t = (m->length - m->base) / m->dx; // >= w
	if (width > t) width = t;
	m->width = width;
	// base + dx * (w - 1) + dy * (h - 1) < length
	t = (m->length - m->base - m->dx * (width - 1)) / m->dy; // >= h
	if (m->height > t) m->height = t;
}

matrix *matrix_fscanf(FILE *f) {
	uint h, w, i;
	double r;
	char c;
	matrix *m;
	if (2 > fscanf(f, " matrix %d %d [", &h, &w)) return NULL;
	m = matrix_init(h, w);
	for (i = 0; i < h * w; i ++) {
		if (1 > fscanf(f, "%lf", &r)) break;
		m->data[i] = r;
	}
	if (i < h * w
			|| 1 > fscanf(f, " %c", &c)
			|| c != ']'
	) {
		matrix_free(m);
		return NULL;
	}
	return m;
}

void matrix_fprintf(FILE *f, matrix *self) {
	if (! matrix_check(self)) error("invalid geometry\n");
	uint h = self->height, w = self->width;
	uint x, y;
	REAL *p = self->data + self->base;
	fprintf(f, "matrix %d %d [\n", h, w);
	for (y = 0; y < h; y ++, p += self->dy - self->dx * w) {
		for (x = 0; x < w; x ++, p += self->dx) fprintf(f, "  %lf", *p);
		fprintf(f, "\n");
	}
	fprintf(f, "]\n");
}

void matrix_printf(matrix *self) {
	matrix_fprintf(stdout, self);
}

REAL matrix_get(matrix *self, uint col, uint row) {
	return self->data[
		self->base + col * self->dy + row * self->dx
	];
}

void matrix_set(matrix *self, uint col, uint row, REAL v) {
	self->data[
		self->base + col * self->dy + row * self->dx
	] = v;
}

matrix *matrix_add_k(matrix *a, matrix *b, REAL k) {
	uint h, w, x, y;
	matrix *m;
	REAL *pm, *pa, *pb;
	h = a->height; w = a->width;
	if (h != b->height || w != b->width) error(wrong_dim);
	m = matrix_init(h, w);
	pm = m->data + m->base;
	pa = a->data + a->base;
	pb = b->data + b->base;
	for (y = 0; y < h; y++) {
		for (x = 0; x < w; x++) {
			*pm = *pa + *pb * k;
			pm += m->dx;
			pa += a->dx;
			pb += b->dx;
		}
		pm += m->dy - w * m->dx;
		pa += a->dy - w * a->dx;
		pb += b->dy - w * b->dx;
	}
	return m;
}

matrix *matrix_add(matrix *a, matrix *b) {
	return matrix_add_k(a, b, 1);
}

matrix *matrix_sub(matrix *a, matrix *b) {
	return matrix_add_k(a, b, -1);
}

matrix *matrix_mul(matrix *a, matrix *b) {
	REAL t, *pa, *pb, *pm;
	uint i, x, y;
	uint h = a->height;
	uint w = b->width;
	uint l = a->width;
	matrix *m = matrix_init(h, w);
	pa = a->data + a->base;
	pb = b->data + b->base;
	pm = m->data + m->base;
	if (b->height != l) error(wrong_dim);
	for (y = 0; y < h; y++) {
		for (x = 0; x < w; x++) {
			t = 0;
			for (i = 0; i < l; i++) {
				t += *pa * *pb;
				pa += a->dx;
				pb += b->dy;
			}
			*pm = t;
			pm += m->dx;
			pa -= l * a->dx;
			pb += b->dx - l * b->dy;
		}
		pm += m->dy - w * m->dx;
		pa += a->dy;
		pb -= w * b->dx;
	}
	return m;
}

#if 0
matrix *calc_givens(REAL a, REAL b, REAL c, REAL *C, REAL *S) {
	/* Calcola i parametri della matrice di givens G = [(C, S), ( - S, C)]che diagonalizza A = [(a, b), (b, c)].\nRETURN:(c, s). */
	REAL m, t;
	if(b == 0) { * C = 1; * S = 0; return; }
	//if(b > 0) *S = 1; else *S = - 1;
	m = (c - a)/(2 * b);
	//if(fabs(m) < 2) *S = - * S;
	if(m > 0) t = (m + sqrt(m * m + 1));
	else t = (m - sqrt(m * m + 1));
	 * C = 1/sqrt(t * t + 1);
	 * S = t * *C;
}

matrix *givens(matrix *mat, REAL c, REAL s, uint j, uint k, uint side) {
	/* Moltiplica mat per la matrice di Givens G = [(c, s)( - s, c)] di indici j, k e angolo a: c = cos(a) s = sin(a).
	 * side = 1: mat * G; side = 2: G' * mat; side = 3: G' * mat * G */
	uint cols = mat->width;
	uint rows = mat->height;
	uint i; REAL *a, *b, p, q;
	if(side == 2 || side == 3) {
		a = mat->data + j * cols;
		b = mat->data + k * cols;
		for(i = 0; i < cols; i++) {
			p = * a; q = * b;
			 * a = c * p + s * q;
			 * b = c * q - s * p;
			a++ ; b++ ;
		}
	}
	if(side == 1 || side == 3) {
		a = mat->data + j;
		b = mat->data + k;
		for(i = 0; i < rows; i++) {
			p = * a; q = * b;
			 * a = c * p + s * q;
			 * b = c * q - s * p;
			a += rows; b += rows;
		}
	}
	return mat;
}

matrix *diagJacobi(matrix *self, matrix *Q, REAL eps) {
	eps *= eps;
	uint height = self->height;
	uint width = self->width;
	REAL r = 1;
	REAL v;
	REAL *data = self->data;
	REAL m, M;
	uint x, y;
	REAL c, s;
	while(r > eps) {
		r = eps;
		M = - 1;
		for(y = 0; y < width - 1; y++) {
			for(x = y + 1; x < height; x++) {
				v = data[x + width * y];
				M = fabs(data[x + width * x] * data[y + width * y]);
				if(v * v > M * r) {
					r = v * v/M;
					calc_givens(data[y + width * y], data[x + width * y], data[x + width * x], &c, &s);
					givens(self, c, s, y, x, 3);
					givens(Q, c, s, y, x, 1);
				}
			}
		}
	}
	return Q;
}

REAL g(uint n, REAL x, REAL k, REAL blur) {
	if(blur != 0)
		return(g(n - 1, x + blur, k, 0) - g(n - 1, x - blur, k, 0))/(2 * blur);
	x *= k;
	if(n == 1) return k * x/(fabs(x) + 1);
	if(n == 2) {
		REAL t = (fabs(x) + 1);
		return k * k/(t * t);
	}
	if(n == 0) {
		x = fabs(x);
		return x - log(x + 1);
	}
	if(n == - 1&&x >= 0)
		return (x * x/2 + x - (x + 1) * log(x + 1))/k;
	if(n == - 1&&x < 0)
		return ( - x * x/2 + x + (1 - x) * log(1 - x))/k;
}

void fastica_step(matrix *D, REAL *w, uint n, REAL blur, REAL *V) {
	uint c = D->width;
	uint r = D->height;
	REAL *x = D->data;
	uint i, j, ic;
	for(i = 0; i < c; i++) V[i] = 0;
	REAL s, v, t; s = v = 0;
	for(i = 0; i < r; i++) {
		// t = w.x( * , i)
		for(j = n, t = 0, ic = i * c; j < c; j++)
			t += w[j] * x[ic + j];
		v += g(2, t, 1.5, blur);
		// V += x( * , i) * g(t)
		t = g(1, t, 1.5, blur);
		for(j = n, ic = i * c; j < c; j++)
			V[j] += t * x[ic + j];
	}
	for(j = n; j < c; j++) V[j] = (V[j] - w[j] * v)/r;
	for(v = 0, i = 0; i < c; i++) v += V[i] * V[i];
	if(v == 0) return;
	if(v != 1) {
		v = sqrt(v);
		for(i = 0; i < c; i++) V[i] /= v;
	}
}

void householder(matrix *self, REAL *u, uint side, uint skip) {
	// Moltiplica self per la matrice di Householder di vettore unitario u.
	// side = 1: moltiplica a dex; side = 2: a sin; side = 3: entrambi
	// u viene normalizzato.
	// ignora le prime skip colonne/righe
	uint r = self->height;
	uint c = self->width;
	uint i, n, x, y;
	REAL v = 0;
	if(side == 1) n = c;
	else if(side == 2) n = r;
	else if(side == 3) n = (r < c?r:c);
	for(v = 0, i = skip; i < n; i++) v += u[i] * u[i];
	if(v == 0) return;
	if(v != 1) {
		v = sqrt(v);
		for(i = skip; i < n; i++) u[i] /= v;
	}
	if(side == 2 || side == 3) {
		for(x = 0; x < c; x++) {
			for(v = 0, i = x + skip * r, y = skip; y < n; y++) {
				v += self->data[i] * u[y];
				i += r;
			}
			v *= 2;
			for(i = x + skip * r, y = skip; y < n; y++) {
				self->data[i] -= v * u[y];
				i += r;
			}
		}
	}
	if(side == 1 || side == 3) {
		for(y = 0; y < r; y++) {
			for(v = 0, i = y * c + skip, x = skip; x < n; x++) {
				v += self->data[i] * u[x];
				i++ ;
			}
			v *= 2;
			for(i = y * c + skip, x = skip; x < n; x++) {
				self->data[i] -= v * u[x];
				i++ ;
			}
		}
	}
}

matrix *diagonalize(matrix *self, matrix *Q, REAL eps) {
	eps *= eps;
	REAL *d = self->data;
	uint n = self->width;
	uint i, j;
	REAL H[n];
	double v;
	for(i = 0; i < n - 2; i++) {
		for(j = 0; j <= i; j += 1) H[j] = 0;
		for(j = i + 1; j < n; j += 1) H[j] = d[i + j * n];
		for(v = 0, j = i + 1; j < n; j++) v += H[j] * H[j];
		H[i + 1] -= sqrt(v);
		householder(self, H, 3, i + 1);
		householder(Q, H, 1, i + 1);
	}
	REAL t;
	double C[n];
	double S[n];
	double a, b;
	double c, s;
	while(1) {
		t = - 1;
		for(i = 0; i < n; i++) {
			s = d[i + i * n];
			if(t < 0 || s < t)t = s;
		}
		for(i = 0; i < n; i++) d[i + i * n] -= t;
		for(i = n - 1; i > 0; i--) {
			s = d[i - 1 + i * n];
			c = d[i + i * n];
			v = hypot(c, s);
			if(v > 0) {c /= v; s /= v; }
			C[i] = c; S[i] = s;
			givens(Q, c, - s, i - 1, i, 1);
			for(j = (i - 2) * n; j <= i * n; j += n) {
				if(j < 0)continue;
				a = c * d[i - 1 + j] - s * d[i + j];
				b = s * d[i - 1 + j] + c * d[i + j];
				d[i - 1 + j] = a;
				d[i + j] = b;
			}
		}
		for(i = n - 1; i > 0; i--) {
			s = S[i]; c = C[i];
			for(j = i - 1; j <= i + 1; j++) {
				if(j < 0 || j >= n)continue;
				a = c * d[j + i * n - n] - s * d[j + i * n];
				b = s * d[j + i * n - n] + c * d[j + i * n];
				d[j + i * n - n] = a;
				d[j + i * n] = b;
			}
		}
		for(i = 0; i < n; i++) d[i + i * n] += t;
		v = 0;
		for(i = 0; i < n; i++) {
			c = fabs(d[i + i * n]);
			if(v == 0 || c < v)v = c;
			C[i] = c;
		}
		for(i = 0; i < n; i++) {
			if(C[i] == 0)C[i] = v;
			assert(C[i] > 0);
		}
		v = 0;
		for(i = 0; i < n - 1; i++) {
			c = d[i + i * n + n];
			c *= c/(C[i] * C[i + 1]);
			if(c > v)v = c;
		}
		//printf("v = %g\n", v);
		if(v < eps) break;
	}
	return Q;
}

matrix *fastica(matrix *self, matrix *T, REAL blur, REAL eps) {
	uint c = self->width;
	uint r = self->height;
	REAL *x = self->data;
	REAL W[c], W1[c];
	uint i, j, n;
	REAL d, d0, e, s, t, v;
	for(n = 0; n < c; n++) {
		for(i = 0; i < c; i++) W[i] = W1[i] = 0;
		W[n] = 1;
		d = e = eps;
		d0 = 100 * d;
		while(1) {
			v = 0;
			for(i = 0; i < c; i++) {
				t = 0;
				for(j = 0; j < c; j++) {
					t += T->data[j + i * c] * W[j];
				}
				v += t * t;
			}
			v = sqrt(v);
			e *= 1.02;
			fastica_step(self, W, n, blur * v, W1);
			d = 0.0;
			for(j = 0; j < c; j++) {
				d += fabs(W[j] - W1[j]);
			}
			d /= c;
			if(d < e)break;
			s = d + d0; v = 0;
			for(j = 0; j < c; j++) {
				t = (d0 * W1[j] + d * W[j])/s;
				W[j] = t;
				v += t * t;
			}
			if(v == 0) break;
			v = sqrt(v);
			for(j = 0; j < c; j++) W[j] /= v;
			d0 = d;
		}
		W[n] -= 1;
		householder(T, W, 1, n);
		householder(self, W, 1, n);
	}
	return self;
}
#endif
// vim: set fdm=indent sw=2 ts=2 nocindent autoindent indk=:
