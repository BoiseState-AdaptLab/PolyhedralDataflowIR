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
#define arrprnt(name,arr,size) {\
fprintf(stderr,"%s={",(name));\
for(unsigned __i__=0;__i__<(size);__i__++) fprintf(stderr,"%lg,",(arr)[__i__]);\
fprintf(stderr,"}\n");}
#define crow(m) (*crow)[(m)]
#define crp(i) (*crp)[(i)]
#define row(n) row[(n)]

unsigned coo_csf_insp(const unsigned* dims, const unsigned* indices, const unsigned M, const unsigned N, unsigned*** fptr, unsigned*** find);
unsigned coo_csf_insp(const unsigned* dims, const unsigned* indices, const unsigned M, const unsigned N, unsigned*** fptr, unsigned*** find) {
    unsigned t1,t2,t3,t4,t5,t6,t7,t8,t9,t10,t11,t12,t13,t14,t15;
    unsigned n;
    unsigned F = 0;

    unsigned order = N;
    unsigned nnz = M;

    unsigned pos[N] = {0};
    unsigned loc[N] = {0};
    unsigned mark[N] = {0};
    unsigned psizes[order] = {0};

    *fptr = (unsigned**) calloc(N, sizeof(unsigned*));
    *find = (unsigned**) calloc(N, sizeof(unsigned*));

    // Copy dimensions and size up arrays...
    psizes[0] = 2;
    for (n = 1; n < N; n++) {
        psizes[n] = M + 1;            // Worst-case for offsets is NNZ+1
    }
    for (n = 0; n < N; n++) {
        fptr[n] = (unsigned*) calloc(psizes[n], sizeof(int));
        find[n] = (unsigned*) calloc(M, sizeof(int));
    }


// insp_init
#undef s0
#define s0() crow(0)=row(0)
#undef s1
#define s1() R=crp(0)=crp(1)=0

s0();
s1();

// insp_m+insp_crow+insp_R+insp_crp
#undef s0
#define s0(n,i) if ((i) != row((n)-1)) R+=1
#undef s1
#define s1(n,i) crow(R)=(i)
#undef s2
#define s2(n,i) if ((n) >= crp(R+1)) crp(R+1)=(n)+1

//#pragma omp parallel for schedule(auto) private(t2,t4) default(shared)
#pragma omp simd
for(t2 = 1; t2 <= M-1; t2++) {
  t4=row(t2);
  s0(t2,t4);
  s1(t2,t4);
  s2(t2,t4);
}

R+=1;
*crow = (unsigned*) realloc(*crow, R * sizeof(int));
*crp = (unsigned*) realloc(*crp, (R+1) * sizeof(int));

    return (R);
}    // coo_csf_insp

#undef min
#undef max
#undef abs
#undef floord
#undef offset2
#undef offset3
#undef offset4
#undef arrinit
#undef arrprnt
#undef row
#undef crp
#undef crow
