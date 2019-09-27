// 'conjgrad_csb' code generated by 'edavis' at 09/27/2019 11:52:03
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <math.h>
#include <string.h>
#include <omp.h>

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
#define tid omp_get_thread_num()
#define bcol(t,m) bcol[(m)]
#define bp(t,m) bp[(m)]
#define bp1(t,m) bp[(m+1)]
#define brow(t,m) brow[(m)]
#define ecol(t,m,n) ecol[(n)]
#define erow(t,m,n) erow[(n)]

double conjgrad_csb(const double* A, const double* b, const unsigned B, const unsigned N, const unsigned NB, const unsigned T, const unsigned* bp, const unsigned* brow, const unsigned* bcol, const unsigned char* erow, const unsigned char* ecol, double* x);
inline double conjgrad_csb(const double* A, const double* b, const unsigned B, const unsigned N, const unsigned NB, const unsigned T, const unsigned* bp, const unsigned* brow, const unsigned* bcol, const unsigned char* erow, const unsigned char* ecol, double* x) {
    unsigned t1,t2,t3,t4,t5,t6,t7,t8,t9,t10,t11,t12,t13,t14,t15;
    double* __restrict r = (double*) calloc((N),sizeof(double));
    double* __restrict d = (double*) calloc((N),sizeof(double));
    double ds;
    double rs;
    double rs0;
    double* __restrict s = (double*) calloc((N),sizeof(double));
    double alpha;
    double beta;

// copy+dinit+sinit+spmv+ddot+rdot0+adiv+xadd+rsub+rdot+bdiv+dadd
#undef s0
#define s0(i) r[(i)]=d[(i)]=b[(i)]
#undef s1
#define s1(t) ds=rs0=rs=0.000000
#undef s2
#define s2(t,i) s[(i)]=0.000000
#undef s3
#define s3(t,m,n,i,j) s[(i)]+=A[(n)]*d[(j)]
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

#pragma omp parallel for schedule(auto) private(t2,t4,t6,t8)
for(t2 = 0; t2 <= N-1; t2++) {
  s0(t2);
}
for(t2 = 0; t2 <= T-1; t2++) {
  s1(t2);
  #pragma omp parallel for schedule(auto) private(t2,t4,t6,t8)
  for(t4 = 0; t4 <= N-1; t4++) {
    s2(t2,t4);
  }
  #pragma omp parallel for schedule(auto) private(t2,t4,t6,t8)
  for (t4 = 0; t4 < NB; t4++) {
    #pragma omp simd
    for(t6 = bp(t2,t4); t6 <= bp1(t2,t4)-1; t6++) {
      t8 = B * brow(t2,t4) + erow(t2,t4,t6);
      t10 = B * bcol(t2,t4) + ecol(t2,t4,t6);
      s3(t2,t4,t6,t8,t10);
    }
  }
  #pragma omp parallel for schedule(auto) private(t2,t4,t6,t8)
  for(t4 = 0; t4 <= N-1; t4++) {
    s4(t2,t4);
    s5(t2,t4);
  }
  s6(t2);
  #pragma omp parallel for schedule(auto) private(t2,t4,t6,t8,alpha)
  for(t4 = 0; t4 <= N-1; t4++) {
    s7(t2,t4);
    s8(t2,t4);
    s9(t2,t4);
  }
  s10(t2);
  #pragma omp parallel for schedule(auto) private(t2,t4,t6,t8,beta)
  for(t4 = 0; t4 <= N-1; t4++) {
    s11(t2,t4);
  }
}

    free(r);
    free(d);
    free(s);

    return (rs);
}    // conjgrad_csb

#undef min
#undef max
#undef abs
#undef floord
#undef offset2
#undef offset3
#undef offset4
#undef arrinit
#undef arrprnt
#undef tid
#undef bcol
#undef bp
#undef bp1
#undef brow
#undef ecol
#undef erow

