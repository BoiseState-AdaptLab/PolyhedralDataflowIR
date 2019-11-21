// 'conjgrad_csr' code generated by 'edavis' at 07/09/2019 11:56:08
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
#define col(t,i,n) col[(n)]
#define rp(t,i) rp[(i)]
#define rp1(t,i) rp[(i+1)]

double conjgrad_csr(const double* A, const double* b, const unsigned N, const unsigned T, const unsigned* col, const unsigned* rp, double* x);
inline double conjgrad_csr(const double* A, const double* b, const unsigned N, const unsigned T, const unsigned* col, const unsigned* rp, double* x) {
    unsigned t1,t2,t3,t4,t5,t6,t7,t8,t9,t10;
    double* __restrict d = (double*) malloc((N)*sizeof(double));
    double* __restrict r = (double*) malloc((N)*sizeof(double));
    double* __restrict s = (double*) malloc((N)*sizeof(double));
    double ds;
    double rs0;
    double alpha;
    double rs;
    double beta;

// copy+mset+spmv+ddot+rdot0+adiv+xadd+rsub+rdot+bdiv+bmul+dadd
#undef s0
#define s0(i) r[(i)]=d[(i)]=b[(i)]
#undef s1
#define s1(t) ds=rs0=rs=0.000000
#undef s2
#define s2(t,i) s[(i)]=0.0
#undef s3
#define s3(t,i,n,j) s[(i)]+=A[(n)]*d[(j)]
#undef s4
#define s4(t,i) ds+=d[(i)]*s[(i)]
#undef s5
#define s5(t,i) rs0+=r[(i)]*r[(i)]
#undef s6
#define s6(t) alpha=rs0/ds
#undef s7
#define s7(t,i) x[(i)]+=alpha*d[(i)]
#undef s8
#define s8(t,i) r[(i)]-=alpha*s[(i)]
#undef s9
#define s9(t,i) rs+=r[(i)]*r[(i)]
#undef s10
#define s10(t) beta=rs/rs0
#undef s11
#define s11(t,i) d[(i)]=r[(i)]+beta*d[(i)]

#pragma omp parallel for schedule(auto)
for(t2 = 0; t2 <= N-1; t2++) {
  s0(t2);
}
for(t2 = 1; t2 <= T; t2++) {
  s1(t2);
  #pragma omp parallel for schedule(auto) reduction(+:ds,rs0)
  for(t4 = 0; t4 <= N-1; t4++) {
    s2(t2,t4);
    #pragma omp simd
    for(t6 = rp(t2,t4); t6 <= rp1(t2,t4)-1; t6++) {
      t8=col(t2,t4,t6);
      s3(t2,t4,t6,t8);
    }
    s4(t2,t4);
    s5(t2,t4);
  }
  s6(t2);
  #pragma omp parallel for schedule(auto) reduction(+:rs)
  for(t4 = 0; t4 <= N-1; t4++) {
    s7(t2,t4);
    s8(t2,t4);
    s9(t2,t4);
  }
  s10(t2);
  #pragma omp parallel for schedule(auto)
  for(t4 = 0; t4 <= N-1; t4++) {
    s11(t2,t4);
  }
}

    free(d);
    free(r);
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

