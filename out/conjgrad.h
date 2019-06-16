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
#define row(n) row[(n)]
#define col(n) col[(n)]

double conj_grad(const unsigned N, const double* b, const unsigned M, const unsigned* row, const unsigned* col, const double* A, const double* x0, double* x);
inline double conj_grad(const unsigned N, const double* b, const unsigned M, const unsigned* row, const unsigned* col, const double* A, const double* x0, double* x) {
    unsigned t1,t2,t3,t4,t5;
    double* r = (double*) calloc((N),sizeof(double));
    double* d0 = (double*) calloc((N),sizeof(double));
    double* s = (double*) calloc((N),sizeof(double));
    double* d = (double*) calloc((N),sizeof(double));
    double ds = 0;
    double rs0 = 0;
    double alpha = 0;
    double rs = 0;
    double beta = 0;

// copy
#undef s0
#undef s1
#define s0(i) r[(i)]=b[(i)]
#define s1(i) d0[(i)]=b[(i)]

for(t1 = 0; t1 <= N-1; t1++) {
  s0(t1);
  s1(t1);
}

// spmv
#undef s0
#define s0(n,i,j) s[(i)]+=A[(n)]*d0[(j)]

for(t1 = 0; t1 <= M-1; t1++) {
  t2=row(t1);
  t3=col(t1);
  s0(t1,t2,t3);
}

// ddot
#undef s0
#define s0(i) ds+=d0[(i)]*s[(i)]

for(t1 = 0; t1 <= N-1; t1++) {
  s0(t1);
}

// rdot0
#undef s0
#define s0(i) rs0+=r[(i)]*r[(i)]

for(t1 = 0; t1 <= N-1; t1++) {
  s0(t1);
}

// adiv
#undef s0
#define s0() alpha=rs0/ds

s0();
// xadd
#undef s0
#define s0(i) x[(i)]=x0[(i)]+alpha*d0[(i)]

for(t1 = 0; t1 <= N-1; t1++) {
  s0(t1);
}

// rsub
#undef s0
#define s0(i) r[(i)]-=alpha*s[(i)]

for(t1 = 0; t1 <= N-1; t1++) {
  s0(t1);
}

// rdot
#undef s0
#define s0(i) rs+=r[(i)]*r[(i)]

for(t1 = 0; t1 <= N-1; t1++) {
  s0(t1);
}

// bdiv
#undef s0
#define s0() beta=rs/rs0

s0();
// dadd
#undef s0
#define s0(i) d[(i)]=r[(i)]+beta*d0[(i)]

for(t1 = 0; t1 <= N-1; t1++) {
  s0(t1);
}

    free(r);
    free(d0);
    free(s);
    free(d);

    return(rs);
}    // conj_grad

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

