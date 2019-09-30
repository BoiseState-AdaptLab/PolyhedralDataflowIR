// 'coo_csr_insp' code generated by 'edavis' at 09/03/2019 10:24:54
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <math.h>
#include <string.h>

#define min(x,y) (((x)<(y))?(x):(y))
#define max(x,y) (((x)>(y))?(x):(y))
#define abs(x) ((x)<0?-(x):(x))
#define absmin(x,y) ((x)=min(abs((x)),abs((y))))
#define absmax(x,y) ((x)=max(abs((x)),abs((y))))
#define floord(x,y) ((x)/(y))
#define sgn(x) ((x)<0?-1:1)
#define offset2(i,j,M) ((j)+(i)*(M))
#define offset3(i,j,k,M,N) ((k)+((j)+(i)*(M))*(N))
#define offset4(i,j,k,l,M,N,P) ((l)+((k)+((j)+(i)*(M))*(N))*(P))
#define arrinit(ptr,val,size) for(unsigned __i__=0;__i__<(size);__i__++) (ptr)[__i__]=(val)
#define arrprnt(name,arr,size) {\
fprintf(stderr,"%s={",(name));\
for(unsigned __i__=0;__i__<(size);__i__++) fprintf(stderr,"%lg,",(arr)[__i__]);\
fprintf(stderr,"}\n");}
#define row(n) row[(n)]
#define col(n) col[(n)]
#define val(n) val[(n)]

unsigned coo_ell_insp(const unsigned M, const unsigned* row, const unsigned* col, const double* val, unsigned** lcol, double** lval);
inline unsigned coo_ell_insp(const unsigned M, const unsigned* row, const unsigned* col, const double* val, unsigned** lcol, double** lval) {
    unsigned t1,t2,t3,t4,t5,t6,t7,t8,t9,t10,t11,t12,t13,t14,t15;
    unsigned N;
    unsigned k;

// inspN+inspK
#undef s0
#define s0() N=row(M-1)+1
#undef s1
#define s1() k=1

s0();
s1();

*lcol = (unsigned*) calloc(M/2 * N, sizeof(unsigned));
*lval = (double*) calloc(M/2 * N, sizeof(double));

#undef s2
#define s2(n,i) if ((i) > row((n)-1)) { K=max(k,K); k=0; }
#undef s3
#define s3(n,i) k+=1
#undef s4
#define s4(n,i) lcol[offset2((k),(i),N)] = col[(n)]
#undef s5
#define s5(n,i) lval[offset2((k),(i),N)] = val[(n)]

#pragma omp parallel for schedule(auto) private(t2,t4)
for(t2 = 0; t2 <= M-1; t2++) {
  t4=row(t2);
  s2(t2,t4);
  s3(t2,t4);
  s4(t2,t4);
  s5(t2,t4);
}

*lcol = (unsigned*) realloc(*lcol, K * N * sizeof(unsigned));
*lval = (double*)  realloc(*lval, K * N * sizeof(double));

    return (K);
}    // coo_ell_insp

#undef min
#undef max
#undef abs
#undef absmin
#undef absmax
#undef floord
#undef sgn
#undef offset2
#undef offset3
#undef offset4
#undef arrinit
#undef arrprnt
#undef row
#undef col
#undef val
