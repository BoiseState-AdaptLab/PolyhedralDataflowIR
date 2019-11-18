#ifndef _LINALG_H_
#define _LINALG_H_

#include <stdio.h>
#include <math.h>
#include <gsl/gsl_blas.h>
#include <gsl/gsl_linalg.h>

#define max(x,y) (((x)>(y))?(x):(y))
#define sgn(a,b) ((b) >= 0.0 ? fabs(a) : -fabs(a))
#define lin(i,j,M) ((i)*(M)+(j))

//typedef char bool;
//#define false		0
//#define true 		1

 /**
 * Compute the (Moore-Penrose) pseudo-inverse of a matrix.
 *
 * If the singular value decomposition (SVD) of A = UΣVᵀ then the pseudoinverse A⁻¹ = VΣ⁻¹Uᵀ, where ᵀ indicates transpose and Σ⁻¹ is obtained by taking the reciprocal of each nonzero element on the diagonal, leaving zeros in place. Elements on the diagonal smaller than ``rcond`` times the largest singular value are considered zero.
 *
 * @parameter A		Input matrix. **WARNING**: the input matrix ``A`` is destroyed. However, it is still the responsibility of the caller to free it.
 * @parameter rcond		A real number specifying the singular value threshold for inclusion. NumPy default for ``rcond`` is 1E-15.
 *
 * @returns A_pinv		Matrix containing the result. ``A_pinv`` is allocated in this function and it is the responsibility of the caller to free it.
**/
gsl_matrix* moore_penrose_pinv(gsl_matrix *A, const double rcond);
inline gsl_matrix* moore_penrose_pinv(gsl_matrix *A, const double rcond) {
    gsl_matrix *V, *Sigma_pinv, *U, *A_pinv;
    gsl_matrix *_tmp_mat = NULL;
    gsl_vector *_tmp_vec;
    gsl_vector *u;
    double x, cutoff;
    size_t i, j;
    unsigned int n = A->size1;
    unsigned int m = A->size2;
//    bool was_swapped = false;
//
//    if (m > n) {
//        /* libgsl SVD can only handle the case m <= n - transpose matrix */
//        was_swapped = true;
//        _tmp_mat = gsl_matrix_alloc(m, n);
//        gsl_matrix_transpose_memcpy(_tmp_mat, A);
//        A = _tmp_mat;
//        i = m;
//        m = n;
//        n = i;
//    }

    /* do SVD */
    V = gsl_matrix_alloc(m, m);
    u = gsl_vector_alloc(m);
    _tmp_vec = gsl_vector_alloc(m);
    gsl_linalg_SV_decomp(A, V, u, _tmp_vec);
    gsl_vector_free(_tmp_vec);

    /* compute Σ⁻¹ */
    Sigma_pinv = gsl_matrix_alloc(m, n);
    gsl_matrix_set_zero(Sigma_pinv);
    cutoff = rcond * gsl_vector_max(u);

    for (i = 0; i < m; ++i) {
        if (gsl_vector_get(u, i) > cutoff) {
            x = 1. / gsl_vector_get(u, i);
        }
        else {
            x = 0.;
        }
        gsl_matrix_set(Sigma_pinv, i, i, x);
    }

    /* libgsl SVD yields "thin" SVD - pad to full matrix by adding zeros */
    U = gsl_matrix_alloc(n, n);
    gsl_matrix_set_zero(U);

    for (i = 0; i < n; ++i) {
        for (j = 0; j < m; ++j) {
            gsl_matrix_set(U, i, j, gsl_matrix_get(A, i, j));
        }
    }

//    if (_tmp_mat != NULL) {
//        gsl_matrix_free(_tmp_mat);
//    }

    /* two dot products to obtain pseudoinverse */
    _tmp_mat = gsl_matrix_alloc(m, n);
    gsl_blas_dgemm(CblasNoTrans, CblasNoTrans, 1., V, Sigma_pinv, 0., _tmp_mat);

//    if (was_swapped) {
//        A_pinv = gsl_matrix_alloc(n, m);
//        gsl_blas_dgemm(CblasNoTrans, CblasTrans, 1., U, _tmp_mat, 0., A_pinv);
//    }
//    else {
        A_pinv = gsl_matrix_alloc(m, n);
        gsl_blas_dgemm(CblasNoTrans, CblasTrans, 1., _tmp_mat, U, 0., A_pinv);
//    }

    gsl_matrix_free(_tmp_mat);
    gsl_matrix_free(U);
    gsl_matrix_free(Sigma_pinv);
    gsl_vector_free(u);
    gsl_matrix_free(V);

    return A_pinv;
}

//float* mp_pinv_gsl(const float* a, float* a_inv, int N);
//inline float* mp_pinv_gsl(const float* a, float* a_inv, int N) {
float* mp_pinv(const float* a, float* a_inv, int N);
inline float* mp_pinv(const float* a, float* a_inv, int N) {
    gsl_matrix* A = gsl_matrix_alloc(N, N);
    double* ptr = gsl_matrix_ptr(A, 0, 0);
    //memcpy(ptr, a, N * N * sizeof(float));
    for (unsigned i = 0; i < N * N; i++) {
        ptr[i] = a[i];
    }

    gsl_matrix* A_inv = moore_penrose_pinv(A, 1E-15);
    ptr = gsl_matrix_ptr(A_inv, 0, 0);
    //memcpy(a_inv, ptr, N * N * sizeof(float));
    for (unsigned i = 0; i < N * N; i++) {
        a_inv[i] = ptr[i];
    }

    gsl_matrix_free(A);
    gsl_matrix_free(A_inv);

    return a_inv;
}

float* mp_pinv_div(const float* a, float* a_inv, int N);
inline float* mp_pinv_div(const float* a, float* a_inv, int N) {
    for (unsigned i = 0; i < N * N; i++) {
        if (a[i] != 0.0) {
            a_inv[i] = 1.0 / a[i];
        } else {
            fprintf(stderr,"div by zero at i=%d!\n", i);
        }
    }
}

static double pythag(double a, double b) {
    double at = fabs(a), bt = fabs(b), ct, result;
    if (at > bt)       { ct = bt / at; result = at * sqrt(1.0 + ct * ct); }
    else if (bt > 0.0) { ct = at / bt; result = bt * sqrt(1.0 + ct * ct); }
    else result = 0.0;
    return(result);
}

/*
 * svdcomp - SVD decomposition routine.
 *
 * Takes an mxn matrix a and decomposes it into udv, where u,v are
 * left and right orthogonal transformation matrices, and d is a
 * diagonal matrix of singular values.
 *
 * This routine is adapted from svdecomp.c in XLISP-STAT 2.1 which is
 * code from Numerical Recipes adapted by Luke Tierney and David Betz.
 *
 * Input to dsvd is as follows:
 *   a = mxn matrix to be decomposed, gets overwritten with u
 *   m = row dimension of a
 *   n = column dimension of a
 *   w = returns the vector of singular values of a
 *   v = returns the right orthogonal transformation matrix
*/
void svdcomp(float* a, int m, int n, float* w, float* v, float* rv1);
inline void svdcomp(float* a, int m, int n, float* w, float* v, float* rv1) {
    int flag, i, its, j, jj, k, l, nm;
    double c, f, h, s, x, y, z;
    double anorm = 0.0, g = 0.0, scale = 0.0;

    /* Householder reduction to bidiagonal form */
    for (i = 0; i < n; i++) {
        /* left-hand reduction */
        l = i + 1;
        rv1[i] = scale * g;
        g = s = scale = 0.0;
        if (i < m) {
            for (k = i; k < m; k++)
                scale += fabs((double)a[lin(k,i,m)]);
            if (scale) {
                for (k = i; k < m; k++) {
                    a[lin(k,i,m)] = (float)((double)a[lin(k,i,m)]/scale);
                    s += ((double)a[lin(k,i,m)] * (double)a[lin(k,i,m)]);
                }
                f = (double)a[lin(i,i,m)];
                g = -sgn(sqrt(s), f);
                h = f * g - s;
                a[lin(i,i,m)] = (float)(f - g);
                if (i != n - 1) {
                    for (j = l; j < n; j++) {
                        for (s = 0.0, k = i; k < m; k++)
                            s += ((double)a[lin(k,i,m)] * (double)a[lin(k,j,m)]);
                        f = s / h;
                        for (k = i; k < m; k++)
                            a[lin(k,j,m)] += (float)(f * (double)a[lin(k,i,m)]);
                    }
                }
                for (k = i; k < m; k++)
                    a[lin(k,i,m)] = (float)((double)a[lin(k,i,m)]*scale);
            }
        }
        w[i] = (float)(scale * g);

        /* right-hand reduction */
        g = s = scale = 0.0;
        if (i < m && i != n - 1) {
            for (k = l; k < n; k++)
                scale += fabs((double)a[lin(i,k,m)]);
            if (scale) {
                for (k = l; k < n; k++) {
                    a[lin(i,k,m)] = (float)((double)a[lin(i,k,m)]/scale);
                    s += ((double)a[lin(i,k,m)] * (double)a[lin(i,k,m)]);
                }
                f = (double)a[lin(i,l,m)];
                g = -sgn(sqrt(s), f);
                h = f * g - s;
                a[lin(i,l,m)] = (float)(f - g);
                for (k = l; k < n; k++)
                    rv1[k] = (double)a[lin(i,k,m)] / h;
                if (i != m - 1) {
                    for (j = l; j < m; j++) {
                        for (s = 0.0, k = l; k < n; k++)
                            s += ((double)a[lin(j,k,m)] * (double)a[lin(i,k,m)]);
                        for (k = l; k < n; k++)
                            a[lin(j,k,m)] += (float)(s * rv1[k]);
                    }
                }
                for (k = l; k < n; k++)
                    a[lin(i,k,m)] = (float)((double)a[lin(i,k,m)]*scale);
            }
        }
        anorm = max(anorm, (fabs((double)w[i]) + fabs(rv1[i])));
    }

    /* accumulate the right-hand transformation */
    for (i = n - 1; i >= 0; i--) {
        if (i < n - 1) {
            if (g) {
                for (j = l; j < n; j++)
                    v[lin(j,i,m)] = (float)(((double)a[lin(i,j,m)] / (double)a[lin(i,l,m)]) / g);
                /* double division to avoid underflow */
                for (j = l; j < n; j++) {
                    for (s = 0.0, k = l; k < n; k++)
                        s += ((double)a[lin(i,k,m)] * (double)v[lin(k,j,m)]);
                    for (k = l; k < n; k++)
                        v[lin(k,j,m)] += (float)(s * (double)v[lin(k,i,m)]);
                }
            }
            for (j = l; j < n; j++)
                v[lin(i,j,m)] = v[lin(j,i,m)] = 0.0;
        }
        v[lin(i,i,m)] = 1.0;
        g = rv1[i];
        l = i;
    }

    /* accumulate the left-hand transformation */
    for (i = n - 1; i >= 0; i--) {
        l = i + 1;
        g = (double) w[i];
        if (i < n - 1)
            for (j = l; j < n; j++)
                a[lin(i,j,m)] = 0.0;
        if (g) {
            g = 1.0 / g;
            if (i != n - 1) {
                for (j = l; j < n; j++) {
                    for (s = 0.0, k = l; k < m; k++)
                        s += ((double)a[lin(k,i,m)] * (double)a[lin(k,j,m)]);
                    f = (s / (double)a[lin(i,i,m)]) * g;
                    for (k = i; k < m; k++)
                        a[lin(k,j,m)] += (float)(f * (double)a[lin(k,i,m)]);
                }
            }
            for (j = i; j < m; j++)
                a[lin(j,i,m)] = (float)((double)a[lin(j,i,m)]*g);
        } else {
            for (j = i; j < m; j++)
                a[lin(j,i,m)] = 0.0;
        }
        a[lin(i,i,m)] += 1.0f;
    }

    /* diagonalize the bidiagonal form */
    for (k = n - 1; k >= 0; k--) {                             /* loop over singular values */
        for (its = 0; its < 30; its++) {                       /* loop over allowed iterations */
            flag = 1;
            for (l = k; l >= 0; l--) {                        /* test for splitting */
                nm = l - 1;
                if (fabs(rv1[l]) + anorm == anorm) {
                    flag = 0;
                    break;
                }
                if (fabs((double)w[nm]) + anorm == anorm)
                    break;
            }
            if (flag) {
                c = 0.0;
                s = 1.0;
                for (i = l; i <= k; i++) {
                    f = s * rv1[i];
                    if (fabs(f) + anorm != anorm) {
                        g = (double)w[i];
                        h = pythag(f, g);
                        w[i] = (float)h;
                        h = 1.0 / h;
                        c = g * h;
                        s = (- f * h);
                        for (j = 0; j < m; j++) {
                            y = (double)a[lin(j,nm,m)];
                            z = (double)a[lin(j,i,m)];
                            a[lin(j,nm,m)] = (float)(y * c + z * s);
                            a[lin(j,i,m)] = (float)(z * c - y * s);
                        }
                    }
                }
            }

            z = (double)w[k];
            if (l == k) {                  /* convergence */
                if (z < 0.0) {             /* make singular value nonnegative */
                    w[k] = (float)(-z);
                    for (j = 0; j < n; j++)
                        v[lin(j,k,m)] = -v[lin(j,k,m)];
                }
                break;
            }

            /* shift from bottom 2 x 2 minor */
            x = (double)w[l];
            nm = k - 1;
            y = (double)w[nm];
            g = rv1[nm];
            h = rv1[k];
            f = ((y - z) * (y + z) + (g - h) * (g + h)) / (2.0 * h * y);
            g = pythag(f, 1.0);
            f = ((x - z) * (x + z) + h * ((y / (f + sgn(g, f))) - h)) / x;

            /* next QR transformation */
            c = s = 1.0;
            for (j = l; j <= nm; j++) {
                i = j + 1;
                g = rv1[i];
                y = (double)w[i];
                h = s * g;
                g = c * g;
                z = pythag(f,h);
                rv1[j] = z;
                c = f / z;
                s = h / z;
                f = x * c + g * s;
                g = g * c - x * s;
                h = y * s;
                y = y * c;
                for (jj = 0; jj < n; jj++) {
                    x = (double)v[lin(jj,j,m)];
                    z = (double)v[lin(jj,i,m)];
                    v[lin(jj,j,m)] = (float)(x * c + z * s);
                    v[lin(jj,i,m)] = (float)(z * c - x * s);
                }
                z = pythag(f, h);
                w[j] = (float)z;
                if (z) {
                    z = 1.0 / z;
                    c = f * z;
                    s = h * z;
                }
                f = (c * g) + (s * y);
                x = (c * y) - (s * g);
                for (jj = 0; jj < m; jj++) {
                    y = (double)a[lin(jj,j,m)];
                    z = (double)a[lin(jj,i,m)];
                    a[lin(jj,j,m)] = (float)(y * c + z * s);
                    a[lin(jj,i,m)] = (float)(z * c - y * s);
                }
            }
            rv1[l] = 0.0;
            rv1[k] = f;
            w[k] = (float)x;
        }
    }
}

/**
 * Compute the (Moore-Penrose) pseudo-inverse of a matrix.
 *
 * If the singular value decomposition (SVD) of A = UΣVᵀ then the pseudoinverse A⁻¹ = VΣ⁻¹Uᵀ, where ᵀ indicates transpose and Σ⁻¹ is obtained by taking the reciprocal of each nonzero element on the diagonal, leaving zeros in place. Elements on the diagonal smaller than ``rcond`` times the largest singular value are considered zero.
 *
 * @parameter A		Input matrix. **WARNING**: the input matrix ``A`` is destroyed. However, it is still the responsibility of the caller to free it.
 * @parameter rcond		A real number specifying the singular value threshold for inclusion. NumPy default for ``rcond`` is 1E-15.
 *
 * @returns A_pinv		Matrix containing the result.
**/
float* mp_pinv_bad(float* A, unsigned n);
inline float* mp_pinv_bad(float* A, float* A_pinv, unsigned n) {
    float *V, *Sigma_pinv, *U; //, *A_pinv;
    float *tmp = NULL;
    //float *_tmp_vec;
    float *w;
    float x, cutoff;
    unsigned i, j, k;
    const float rcond = 1E-15;
    unsigned m = n;

    V = (float*) malloc(m * m * sizeof(float));     //gsl_matrix_alloc(m, m);
    w = (float*) malloc(m * sizeof(float));         //gsl_vector_alloc(m);
    tmp = (float*) malloc(n*sizeof(float));
    //_tmp_vec = (float*) malloc(m * sizeof(float));  //gsl_vector_alloc(m);
    //gsl_linalg_SV_decomp(A, V, w, _tmp_vec);
    svdcomp(A, m, n, w, V, tmp);
    free(tmp);
    //gsl_vector_free(_tmp_vec);

    /* compute Σ⁻¹ */
    Sigma_pinv = (float*) calloc(m * n, sizeof(float)); //gsl_matrix_alloc(m, n);
    //gsl_matrix_set_zero(Sigma_pinv);
    float wmax = w[0];
    for (i = 1; i < m; i++) {
        if (w[i] > wmax) {
            wmax = w[i];
        }
    }
    cutoff = rcond * wmax;

    for (i = 0; i < m; ++i) {
        if (w[i] > cutoff) {
            Sigma_pinv[lin(i,i,m)] = 1.0 / w[i];
        }
        //gsl_matrix_set(Sigma_pinv, i, i, x);
    }

    /* libgsl SVD yields "thin" SVD - pad to full matrix by adding zeros */
    //U = gsl_matrix_alloc(n, n);
    //gsl_matrix_set_zero(U);
    U = (float*) calloc(n * n, sizeof(float)); //gsl_matrix_alloc(m, n);

    for (i = 0; i < n; ++i) {
        for (j = 0; j < m; ++j) {
            //gsl_matrix_set(U, i, j, gsl_matrix_get(A, i, j));
            U[lin(i,j,n)] = A[lin(i,j,m)];
        }
    }

    /* two dot products to obtain pseudoinverse */
    tmp = (float*) calloc(m * n, sizeof(float)); //gsl_matrix_alloc(m, n);
    //gsl_blas_dgemm(CblasNoTrans, CblasNoTrans, 1., V, Sigma_pinv, 0., tmp);
    for(i = 0; i < n; i++) {
        for(j = 0; j < m; j++) {
            for(k = 0; k < n; k++) {
                tmp[lin(i,j,n)]+=V[lin(i,k,m)]*Sigma_pinv[lin(k,j,n)];
            }
        }
    }

    //A_pinv = (float*) calloc(m * n, sizeof(float)); //gsl_matrix_alloc(m, n);
    //gsl_blas_dgemm(CblasNoTrans, CblasTrans, 1., tmp, U, 0., A_pinv);
    for(i = 0; i < n; i++) {
        for(j = 0; j < m; j++) {
            for(k = 0; k < n; k++) {
                A_pinv[lin(i,j,n)]+=tmp[lin(i,k,m)]*U[lin(k,j,n)];
            }
        }
    }

    free(U);                            //gsl_matrix_free(U);
    free(Sigma_pinv);                   //gsl_matrix_free(Sigma_pinv);
    free(w);                            //gsl_vector_free(w);
    free(V);                            //gsl_matrix_free(V);
    free(tmp);

    return A_pinv;
}

#undef max
#undef lin
#undef sgn

#endif      // _LINALG_H_
