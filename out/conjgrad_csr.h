// 'conjgrad_csr' code generated by 'edavis' at 06/23/2019 13:35:50
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <math.h>

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
#define col(i,n) col[(n)]
#define rp(i) rp[(i)]
#define rp1(i) rp[(i+1)]

double conjgrad_csr(const double* A, const unsigned N, const unsigned* col, const unsigned* rp, double* d, double* r, double* x);
inline double conjgrad_csr(const double* A, const unsigned N, const unsigned* col, const unsigned* rp, double* d, double* r, double* x) {
    unsigned t1,t2,t3,t4,t5;
    double* s = (double*) calloc((N),sizeof(double));
    double ds = 0;
    double rs0 = 0;
    double alpha = 0;
    double rs = 0;
    double beta = 0;

// spmv+ddot+rdot0
#undef s0
#define s0(i,n,j) s[(i)]+=A[(n)]*d[(j)]
#undef s1
#define s1(i) ds+=d[(i)]*s[(i)]
#undef s2
#define s2(i) rs0+=r[(i)]*r[(i)]

#pragma omp parallel for schedule(auto) private(t1,t3,t4)
for(t1 = 0; t1 <= N-1; t1++) {
  for(t4 = rp(t1); t4 <= rp1(t1)-1; t4++) {
    t3=col(t1,t4);
    s0(t1,t4,t3);
  }
  s1(t1);
  s2(t1);
}

// adiv
#undef s0
#define s0() alpha=rs0/ds

s0();

// xadd+rsub+rdot
#undef s0
#define s0(i) x[(i)]+=alpha*d[(i)]
#undef s1
#define s1(i) r[(i)]-=alpha*s[(i)]
#undef s2
#define s2(i) rs+=r[(i)]*r[(i)]

#pragma omp parallel for schedule(auto) private(t1)
for(t1 = 0; t1 <= N-1; t1++) {
  s0(t1);
  s1(t1);
  s2(t1);
}

// bdiv
#undef s0
#define s0() beta=rs/rs0

s0();

// bmul+dadd
#undef s0
#define s0(i) d[(i)]*=beta
#undef s1
#define s1(i) d[(i)]+=r[(i)]

#pragma omp parallel for schedule(auto) private(t1)
for(t1 = 0; t1 <= N-1; t1++) {
  s0(t1);
  s1(t1);
}

    free(s);

    return (rs);
}    // conjgrad_csr

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
#undef col
#undef rp
#undef rp1

