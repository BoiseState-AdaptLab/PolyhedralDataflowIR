// 'coo_csf_insp' code generated by 'edavis' at 09/19/2019 12:27:00
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
#define offset3(i,j,k,M,N) ((k)+((j)+(i)*(M))*(N))
#define offset4(i,j,k,l,M,N,P) ((l)+((k)+((j)+(i)*(M))*(N))*(P))
#define arrinit(ptr,val,size) for(unsigned __i__=0;__i__<(size);__i__++) (ptr)[__i__]=(val)
#define findx(n,m) (*findx)[(n)][(m)]
#define fptr(n,m) (*fptr)[(n)][(m)]
#define pos(n) pos[(n)]
#define loc(n) loc[(n)]
#define mark(n) mark[(n)]

void coo_csf_insp(const unsigned* dims, const unsigned* indices, const unsigned M, const unsigned N, unsigned*** fptr, unsigned*** findx);
void coo_csf_insp(const unsigned* dims, const unsigned* indices, const unsigned M, const unsigned N, unsigned*** fptr, unsigned*** findx) {
    unsigned i,m,n;
    unsigned pos[N];// = {0};
    unsigned loc[N];// = {0};
    unsigned mark[N];// = {0};
    unsigned psize = 2;

    *fptr = (unsigned**) calloc(N, sizeof(int*));
    *findx = (unsigned**) calloc(N, sizeof(int*));

    // Copy dimensions and size up arrays...
    for (n = 0; n < N; n++) {
        pos[n] = loc[n] = mark[n] = 0;
        (*fptr)[n] = (unsigned*) calloc(psize, sizeof(int));
        (*findx)[n] = (unsigned*) calloc(M, sizeof(int));
        psize = M + 1;
    }

    // Peel off m == 0 to initialize indices
    for (n = 0; n < N; n++) {
        loc(n) = 0;
        pos(n) = 1;
        findx(n,0) = indices[n*M];
        fptr(n,pos[n])++;
        #pragma omp simd
        for (m = 1; m < M; m++) {
            findx(n,m) = UINT32_MAX;
        }
    }

    for (m = 1; m < M; m++) {
        #pragma omp simd
        for (n = 0; n < N; n++) {
            i = indices[n*M+m];
            if (i != findx(n,loc[n]) || mark(n)) {
                loc(n)++;
                findx(n,loc[n]) = i;
                mark(n) = 0;
                fptr(n,pos[n])++;
                if (n < N-1) {
                    mark(n+1) = 1;
                    pos(n+1)++;
                    fptr(n+1,pos(n+1)) = fptr(n+1,pos(n+1)-1);
                }
            }
        }
    }
}    // coo_csf_insp

#undef min
#undef max
#undef abs
#undef floord
#undef offset2
#undef offset3
#undef offset4
#undef findx
#undef fptr
#undef pos
#undef loc
#undef mark
