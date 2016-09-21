#include <stdio.h>
#include <assert.h>
#include <math.h>
#define REAL float
#define uint unsigned int

typedef struct {
	uint width;
  uint height;
  REAL *data;
} matrix;

REAL matrix_get(matrix *self, uint c, uint r) {
	return self->data[c + r * self->width];
}

void matrix_set(matrix *self, uint c, uint r, REAL v) {
	self->data[c + r * self->width] = v;
}

matrix *test(matrix *self) {
	printf("cmatrix %dx%d %s\n", self->width, self->height, self->data);
}
#if 0
matrix *matrix_mul(matrix *aa, matrix *bb, matrix *ab) {
	uint c = bb->width;
	if(c != ab->width) return NULL;
	uint r = aa->height;
	if(r != ab->height) return NULL;
	uint cr = aa->width;
	if(cr != bb->height) return NULL;
	uint i, x, y;
	REAL t, *a, *b;
	for(y = 0; y < r; y++) {
		for(x = 0; x < c; x++) {
			for(i = 0, t = 0, a = aa->data + y * cr, b = bb->data + x;
					i < cr;
					i++ , a++ , b += c)
				t += *a * *b;
			ab->data[x + y * c] = t;
		}
	}
}

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

void addstat(matrix *data, REAL *s, REAL *p) {
	// s[i] += sum_k(data[i, k])
	// p[i, j] += sum_k(data[i, k] * data[j, k])
	REAL *d = data->data;
	uint j, k, kc, i, jc, c = data->width, r = data->height;
	for(kc = 0; kc < r * c; kc += c) {
		for(i = 0; i < c; i++) {
			s[i] += d[i + kc];
			for(j = jc = 0; j <= i; j++ , jc += c) {
				p[i + jc] += d[i + kc] * d[j + kc];
			}
		}
	}
	for(j = 1, jc = c; j < c; j++ , jc += c) {
		for(k = kc = 0; k < j; k++ , kc += c) {
			p[k + jc] = p[j + kc];
		}
	}
}

void fixstat(REAL n, uint c, REAL *s, REAL *p) {
	uint x, y, yc, xc;
	for(x = 0; x < c; x++) s[x] /= n;
	for(x = 0; x < c; x++) {
		for(y = yc = 0; y <= x; y++ , yc++) {
			p[x + yc] /= n;
			p[x + yc] -= s[x] * s[y];
		}
	}
	for(y = yc = 0; y < c; y++ , yc += c) {
		for(x = xc = 0; x < y; x++ , x += c) {
			p[x + yc] = p[y + xc];
		}
	}
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
