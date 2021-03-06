#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <math.h>
#include <stdarg.h>
#include "matrix.h"

#define MAX(a, b) ((a) > (b) ? (a) : (b))
#define MIN(a, b) ((a) < (b) ? (a) : (b))

void error(char *msg) {
	fprintf(stderr, msg);
	exit(1);
}

char *wrong_dim = "Wrong dimension\n";
char *null_pointer = "NULL pointer\n";
char *cant_alloc = "Cannot alloc memory\n";
char *invalid_data = "Invalid data\n";
char *few_data = "Too few data\n";

matrix *matrix_new() {
	matrix *m = calloc(1, sizeof(matrix));
	if (! m) error(cant_alloc);
	return m;
}

void matrix_init(matrix *out, uint height, uint width, REAL val) {
	REAL *p, *end;
	if (out->data) free(out->data);
	out->data = calloc(height * width, sizeof(REAL));
	if (!out->data) error(cant_alloc);
	out->shared = 0;
	out->height = height;
	out->dy = out->width = width;
	out->dx = 1;
	out->base = 0;
	out->length = height * width;
	if (val == 0) return;
	p = out->data;
	if (height > width) height = width;
	for (end = out->data + height * (1 + width); p < end; p += 1 + width) *p = val;
}

void matrix_make(matrix *out, uint height, uint width, ...) {
	matrix_init(out, height, width, 0);
	REAL *p;
	REAL *end = out->data + height * width;
	va_list ap;
	va_start(ap, width);
	for (p = out->data; p < end; p++) {
		*p = va_arg(ap, double);
	}
	va_end(ap);
}

void matrix_copy_shallow(matrix *out, matrix *m) {
	if (! out || ! m) error(null_pointer); 
	memcpy(out, m, sizeof(*out));
	out->shared = 1;
}

void matrix_copy_deep(matrix *out, matrix *m) {
	uint l;
	matrix_copy_shallow(out, m);
	l = out->length * sizeof(REAL);
	out->data = malloc(l);
	assert(out->data);
	memcpy(out->data, m->data, l);
}

void matrix_clean(matrix *m) {
	memset(m, 1, sizeof(*m));
}

void matrix_free(matrix *m) {
	if (m->data && ! m->shared) free(m->data);
	free(m);
}

void matrix_check(matrix *m) {
	if (
		m->length <= m->base
		+ m->dx * (m->width - 1)
		+ m->dy * (m->height - 1)
	) error(few_data);
}

REAL matrix_get(matrix *m, uint col, uint row) {
	return m->data[
		m->base + col * m->dy + row * m->dx
	];
}

void matrix_set(matrix *m, uint col, uint row, REAL v) {
	m->data[
		m->base + col * m->dy + row * m->dx
	] = v;
}

void matrix_fscanf(matrix *out, FILE *f) {
	uint h, w, i;
	double r;
	char c;
	if (2 > fscanf(f, " matrix %d %d [", &h, &w)) error(invalid_data);
	matrix_init(out, h, w, 0);
	for (i = 0; i < h * w; i ++) {
		if (1 > fscanf(f, "%lf", &r)) break;
		out->data[i] = r;
	}
	if (i < h * w
			|| 1 > fscanf(f, " %c", &c)
			|| c != ']'
	) {
		error(invalid_data);
	}
}

void matrix_fprintf(FILE *f, matrix *m) {
	matrix_check(m);
	uint h = m->height, w = m->width;
	uint x, y;
	REAL *p = m->data + m->base;
	fprintf(f, "matrix %d %d [\n", h, w);
	for (y = 0; y < h; y ++, p += m->dy - m->dx * w) {
		for (x = 0; x < w; x ++, p += m->dx) fprintf(f, "  %lf", *p);
		fprintf(f, "\n");
	}
	fprintf(f, "]\n");
}

void matrix_printf(matrix *m) {
	matrix_fprintf(stdout, m);
}

void matrix_add_k_to(matrix *a, matrix *b, REAL k) {
	uint h, w, x, y;
	REAL *pa, *pb;
	h = a->height; w = a->width;
	if (h != b->height || w != b->width) error(wrong_dim);
	pa = a->data + a->base;
	pb = b->data + b->base;
	for (y = 0; y < h; y++) {
		for (x = 0; x < w; x++) {
			*pa += *pb * k;
			pa += a->dx;
			pb += b->dx;
		}
		pa += a->dy - w * a->dx;
		pb += b->dy - w * b->dx;
	}
}

void matrix_add_to(matrix *a, matrix *b) {
	matrix_add_k_to(a, b, 1);
}

void matrix_sub_from(matrix *a, matrix *b) {
	matrix_add_k_to(a, b, -1);
}

void matrix_mul(matrix *out, matrix *a, matrix *b) {
	REAL t, *pa, *pb, *pm;
	uint i, x, y;
	uint h = a->height;
	uint w = b->width;
	uint l = a->width;
	matrix_init(out, h, w, 0);
	pa = a->data + a->base;
	pb = b->data + b->base;
	pm = out->data + out->base;
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
			pm += out->dx;
			pa -= l * a->dx;
			pb += b->dx - l * b->dy;
		}
		pm += out->dy - w * out->dx;
		pa += a->dy;
		pb -= w * b->dx;
	}
}

void matrix_scale(matrix *m, REAL k) {
	uint x, y;
	REAL *pa;
	pa = m->data + m->base;
	for (y = 0; y < m->height; y++) {
		for (x = 0; x < m->width; x++) {
			*pa *= k;
			pa += m->dx;
		}
		pa += m->dy - m->width * m->dx;
	}
}

void matrix_transpose(matrix *m) {
	uint t;
	t = m->dx; m->dx = m->dy; m->dy = t;
	t = m->height; m->height = m->width; m->width = t;
}

void matrix_sum_cols(matrix *out, matrix *m) {
	uint h = m->height;
	uint w = m->width;
	matrix_init(out, 1, w, 0);
	uint x, y;
	REAL *pm = m->data + m->base;
	REAL *ps;
	for (y = 0; y < h; y++) {
		ps = out->data + out->base;
		for (x = 0; x < w; x++) {
			*ps += *pm;
			pm += m->dx;
			ps += out->dx;
		}
		pm += m->dy - w * m->dx;
	}
}

void matrix_mean_cols(matrix *out, matrix *m) {
	REAL k = 1.0 / m->height;
	matrix_sum_cols(out, m);
	matrix_scale(out, k);
}

void matrix_covar_cols(matrix *cov, matrix *mean, matrix *m) {
  matrix t, u;
	t.data = u.data = NULL;
	matrix_copy_shallow(&t, m);
	matrix_transpose(&t);
	matrix_mul(cov, &t, m);
	matrix_scale(cov, 1.0/m->height);
	matrix_mean_cols(mean, m);
	matrix_copy_shallow(&t, mean);
	matrix_transpose(&t);
	matrix_mul(&u, &t, mean);
	matrix_sub_from(cov, &u);
	matrix_clean(&t);
	matrix_clean(&u);
}

void matrix_set_width(matrix *m, uint width) {
	long int t;
	// base + dx * (w - 1) < length
	t = (m->length - m->base) / m->dx; // >= w
	if (width > t) width = t;
	m->width = width;
	// base + dx * (w - 1) + dy * (h - 1) < length
	t = (m->length - m->base - m->dx * (width - 1)) / m->dy; // >= h
	if (m->height > t) m->height = t;
}

void matrix_householder(matrix *m, REAL *u, uint side, uint skip) {
	// Moltiplica m per la matrice di Householder di vettore unitario u.
	// side = 0 or 1: moltiplica a dex
	// side = 2: a sin
	// side = 3: entrambi
	// u viene normalizzato.
	// ignora le prime skip colonne/righe
	uint h = m->height;
	uint w = m->width;
	uint n, x, y;
	REAL v = 0, *p;
	if (side == 0) side = 1;
	if (side == 1) n = w;
	else if (side == 2) n = h;
	else if (side == 3) n = (h < w ? h : w);
	for (v = 0, x = skip; x < n; x++) v += u[x] * u[x];
	assert(v > 0);
	if (v != 1) {
		v = sqrt(v);
		for (x = skip; x < n; x++) u[x] /= v;
	}
	if (side != 1) {
		p = m->data + m->base + skip * m->dy;
		for (x = 0; x < w; x++) {
			v = 0;
			for (y = skip; y < n; y++) {
				v += *p * u[y];
				p += m->dy;
			}
			v *= 2;
			p -= (n - skip) * m->dy;
			for (y = skip; y < n; y++) {
				*p -= v * u[y];
				p += m->dy;
			}
			p += m->dx - (n - skip) * m->dy;
		}
	}
	if (side != 2) {
		p = m->data + m->base + skip * m->dx;
		for (y = 0; y < h; y++) {
			v = 0;
			for (x = skip; x < n; x++) {
				v += *p * u[x];
				p += m->dx;
			}
			v *= 2;
			p -= (n - skip) * m->dx;
			for (x = skip; x < n; x++) {
				*p -= v * u[x];
				p += m->dx;
			}
			p += m->dy - (n - skip) * m->dx;
		}
	}
}

void matrix_transform(matrix *a, matrix *b, uint side) {
	// a -> a'
	// b -> b'
	// q is orthonormal, q^ = transpose(q)
	// side = 1: a' = a * q is lower triangular 
	//      and: b' = b * q
	// side = 2: a' = q * a is upper triangular
	//      and: b' = q * b
	// side = 3: a' = q^*a*q is upper hessenberg 
	//      and: b' = b * q
	// most of times, you want init b = identity
	uint n = a->width;
	uint i, j, k, dx, dy;
	REAL H[n], *p;
	double v;
	if (side == 3 && a->height != a->width) error(wrong_dim); 
	k = (side == 3 ? 1 : 0);
	if (side == 1) {dx = a->dy; dy = a->dx;}
	else {dx = a->dx; dy = a->dy;}
	for(i = 0; i < n - 1 - k; i ++) {
		for(j = 0; j < i + k; j ++) H[j] = 0;
		p = a->data + a->base
			+ i * dx + (i + k) * dy;
		for(j = i + k; j < n; j ++) {
			H[j] = *p;
			p += dy;
		}
		for(v = 0, j = i + k; j < n; j ++) v += H[j] * H[j];
		H[i + k] -= sqrt(v);
		matrix_householder(a, H, side, i + k);
		if (b) matrix_householder(b, H, side % 3, i + k);
	}
}

void matrix_tri_lower(matrix *a, matrix *b) {
	matrix_transform(a, b, 1);
}

void matrix_tri_upper(matrix *a, matrix *b) {
	matrix_transform(a, b, 2);
}

void matrix_hessemberg(matrix *a, matrix *b) {
	matrix_transform(a, b, 3);
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

matrix *diagJacobi(matrix *m, matrix *Q, REAL eps) {
	eps *= eps;
	uint height = m->height;
	uint width = m->width;
	REAL r = 1;
	REAL v;
	REAL *data = m->data;
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
					givens(m, c, s, y, x, 3);
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

matrix *diagonalize(matrix *m, matrix *Q, REAL eps) {
	eps *= eps;
	REAL *d = m->data;
	uint n = m->width;
	uint i, j;
	REAL H[n];
	double v;
	for(i = 0; i < n - 2; i++) {
		for(j = 0; j <= i; j += 1) H[j] = 0;
		for(j = i + 1; j < n; j += 1) H[j] = d[i + j * n];
		for(v = 0, j = i + 1; j < n; j++) v += H[j] * H[j];
		H[i + 1] -= sqrt(v);
		householder(m, H, 3, i + 1);
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

matrix *fastica(matrix *m, matrix *T, REAL blur, REAL eps) {
	uint c = m->width;
	uint r = m->height;
	REAL *x = m->data;
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
			fastica_step(m, W, n, blur * v, W1);
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
		householder(m, W, 1, n);
	}
	return m;
}
#endif
// vim: set fdm=indent sw=2 ts=2 nocindent autoindent indk=:
