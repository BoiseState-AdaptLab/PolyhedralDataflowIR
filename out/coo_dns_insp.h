// 'coo_dns_insp' code generated by 'edavis' at 11/15/2019 13:03:00
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <math.h>
#include <string.h>

#define min(x,y) (((x)<(y))?(x):(y))
#define max(x,y) (((x)>(y))?(x):(y))
#define abs(x) ((x)<0?-(x):(x))
#define floord(x,y) ((x)/(y))
#define offset2(i,j,M) ((j)+(i)*(M))
//#define offset3(i,j,k,M,N) ((k)+((j)+(i)*(M))*(N))
#define offset3(i,j,k,J,K) ((k)+(K)*((j)+(J)*(i)))
#define offset4(i,j,k,l,M,N,P) ((l)+((k)+((j)+(i)*(M))*(N))*(P))
#define arrinit(ptr,val,size) for(unsigned __i__=0;__i__<(size);__i__++) (ptr)[__i__]=(val)
#define findx(n,m) (*findx)[(n)][(m)]
#define fptr(n,m) (*fptr)[(n)][(m)]
#define pos(n) pos[(n)]
#define loc(n) loc[(n)]
#define mark(n) mark[(n)]

void coo_dns_insp(const float* val, const unsigned* dims, const unsigned* indices, const unsigned M, const unsigned N, float** dval);
void coo_dns_insp(const float* val, const unsigned* dims, const unsigned* indices, const unsigned M, const unsigned N, float** dval) {
    unsigned i,j,k;
    unsigned m,n,p,t;
    unsigned ndx;
    unsigned prod;
    unsigned size;
    unsigned exp;
    unsigned I=dims[0];
    unsigned J=dims[1];
    unsigned K=dims[2];
    unsigned* pos = (unsigned*) calloc(N, sizeof(int));
    unsigned* crd = (unsigned*) calloc(N, sizeof(int));
    int* stack = (int*) malloc(N * sizeof(int));

    size = dims[0];
    for (n = 1; n < N; n++) {
        size *= dims[n];
        stack[n] = -1;
    }

    //fprintf(stderr, "size=%u\n", size);
    *dval = (float*) calloc(size, sizeof(float));

    for (m = 0; m < M; m++) {
        ndx = 0;
        for (n = 0; n < N; n++) {
            p = n + 1;
            ndx += indices[n*M+m];
            if (p < N) {
                ndx *= dims[p];
            }
        }

        fprintf(stderr, "dval[%u]=val[%u]=%f\n", ndx, m, val[m]);
        (*dval)[ndx] = val[m];
    }

//    for (i = 0; i < I; i++) {
//        for (j = 0; j < J; j++) {
//            for (k = 0; k < K; k++) {
//                n = offset3(i,j,k,J,K);
//                if ((*dval)[n] != 0) {
//                    fprintf(stderr, "dval(%u,%u,%u)=%f\n", i, j, k, (*dval)[n]);
//                }
//            }
//        }
//    }

//    mark[N-1] = 1;
//    t = -1;
//    stack[++t] = 0;
//    for (; t >= 0;) {
//        // Pop current dimension...
//        n = stack[t--];
//        crd[n] = pos[n];
//        if (n < N - 1 && pos[n] < dims[n]) {
//            // Push next dimension
//            stack[++t] = n + 1;
//            pos[n] += 1;
//        } else {
//            for (p = 0; p < dims[n]; p++) {
//                // Do something with crd...
//                ndx = offset3(crd[0], crd[1], crd[2], dims[1], dims[2]);
//                if ((*dval)[ndx] != 0) {
//                    fprintf(stderr, "dval(%u,%u,%u)=%f\n", crd[0], crd[1], crd[2], (*dval)[ndx]);
//                }
//                stack[++t] = 0;
//            }
//        }
//    }

    free(pos);
    free(crd);
    free(stack);
}    // coo_csf_insp

#undef min
#undef max
#undef abs
#undef floord
#undef offset2
#undef offset3
#undef offset4