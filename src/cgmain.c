#include <stdio.h>
#include <util/mmio.h>
#include "../out/conjgrad.h"

int main(int argc, const char **argv) {
    const char* mtxpath = argv[1];
    double rs = 0.0;
    double tol = atof(argv[2]);

    unsigned T = atoi(argv[3]);
    unsigned N, M, C;
    unsigned* row;
    unsigned* col;
    real* A;

    mm_read(mtxpath, &N, &row, &C, &col, &M, &A);

    real* x = calloc(N, sizeof(real));
    real* x0 = calloc(N, sizeof(real));
    real* d = calloc(N, sizeof(real));
    real* b = calloc(N, sizeof(real));
    for (unsigned i = 1; i < N; i++) {
        b[i] = (real) i;
    }

#define check(t) (rs > tol)

    //for (t = 0; t < tmax && check(t) > 0; t++) {
    for (unsigned t = 0; t < T; t++) {
        conj_grad(N, b, M, row, col, A, x0, x, d);
        real* swap = x0;
        x0 = x;
        x = swap;
    }

    free(b);
    free(x);
    free(x0);
    mm_free(&row, &col, &A);

    return 0;
}
