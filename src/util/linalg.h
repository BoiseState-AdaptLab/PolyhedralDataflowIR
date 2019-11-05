#ifndef _LINALG_H_
#define _LINALG_H_

#include <stdio.h>
#include <gsl/gsl_blas.h>
#include <gsl/gsl_linalg.h>

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

#endif      // _LINALG_H_
