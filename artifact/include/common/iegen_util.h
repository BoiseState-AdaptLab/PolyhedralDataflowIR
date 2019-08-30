#ifndef _IEGEN_UTIL_
#define _IEGEN_UTIL_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>

// #define _NEW_ARGS(T, ...) T ## _init(malloc(sizeof(T)), __VA_ARGS__)
// #define _NEW_STRUCT(T) T ## _init(malloc(sizeof(T)))
// #define NEW_STRUCT(...) IF_1_ELSE(__VA_ARGS__)(_NEW_STRUCT(__VA_ARGS__))(_NEW_ARGS(__VA_ARGS__))

#define EPS 0.01

#define abs(x) (((x) < 0.0) ? -(x) : (x))
#define offset2(i,j,N) (i)*(N)+(j)
#define offset3(i,j,k,M,N) ((k)+(N)*((j)+(M)*(i)))
#define array_init(ptr,val,size) for(unsigned __i__=0; __i__<(size);__i__++) (ptr)[__i__]=(val)

#define GET_TIME(now) { \
   struct timeval tv; \
   gettimeofday(&tv, NULL); \
   now = tv.tv_sec + tv.tv_usec * 1E-6; \
}

void csr_init(csr_data_t *csr, int nr, int nc, int nnz) {
    csr->N_R = nr;
    csr->N_C = nc;
    csr->NNZ = nnz;

    if (nr > 0 && nnz > 0) {
        csr->index = calloc(nr + 1, sizeof(int));
        csr->col = calloc(nnz, sizeof(int));
        csr->A = calloc(nnz, sizeof(real));
    }
}

void csr_read(const char *fpath, csr_data_t *csr) {
    FILE *in;
    char data[1024];
    int i, max_row_len, nrow, ncol, nnz = 0;
    in = fopen(fpath, "r");
    if(in==NULL){
        fprintf(stderr, "something might be wrong with the file\n");
        return;
    }

    fgets(data, 1024, in);
    fscanf(in, "%d %d %d\n", &nrow, &ncol, &nnz);

    csr->index = (int *) malloc(sizeof(int) * (nrow + 1));
    csr->col = (int *) malloc(sizeof(int) * nnz);
    csr->A = (float *) malloc(sizeof(float) * nnz);
    csr->N_R = nrow;
    csr->N_C = ncol;
    csr->NNZ = nnz;
    max_row_len = -1;

    for(i = 0; i <= nrow; i++){
        int temp;
        fscanf(in, "%d", &temp);
        temp--;
        csr->index[i] = temp;
        if(i > 0)
            if(csr->index[i] - csr->index[i-1] > max_row_len)
                max_row_len = csr->index[i] - csr->index[i-1];
    }

    for(i = 0; i < nnz; i++){
        int temp;
        fscanf(in, "%d", &temp);
        temp--;
        //fprintf(stderr, "%d\n", temp);
        csr->col[i] = temp;
        if (csr->col[i] < 0)
            fprintf(stderr, "%d\n", csr->col[i]);
    }

    for(i = 0; i < nnz; i++){
        //char temp[20];
        fscanf(in, "%s", data);
        //fprintf(stderr, "%f\n", data);
        csr->A[i] = atof(data); //1.0;
    }

    fclose(in);
}

void csr_spmv(csr_data_t const *csr, real const *x, real *y) {
  for (unsigned i = 0; i < csr->N_R; i++) {
        #pragma ivdep
        for (unsigned j = csr->index[i]; j < csr->index[i+1]; j++) {
            unsigned k = csr->col[j];
            y[i] += csr->A[j] * x[k];
        }
    }
}

int csr_bsr_verify(csr_data_t const *csr, bsr_data_t const *bsr, real const *x, real const *y) {
    real* restrict y2 = calloc(csr->N_R, sizeof(real));
    csr_spmv(csr, x, y2);

    for (unsigned i = 0; i < csr->N_R; i++) {
        if (abs((y2[i] - y[i])/y2[i]) >= EPS) {
            //fprintf(stderr, "Values don't match at %u, expected %f obtained %f\n", i, y2[i], y[i]);
            return i;
        }
    }

    return -1;
} // csr_bsr_verify

#endif
