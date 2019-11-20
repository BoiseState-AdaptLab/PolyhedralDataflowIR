// 'cp_als_nd_hicoo' code generated by 'edavis' at 11/13/2019 15:33:04
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <math.h>
#include <string.h>
#include <float.h>
#include <time.h>
#include <linalg.h>
#include <assert.h>

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
#define urand(m) (rand()/nextafter(RAND_MAX,DBL_MAX)+(m))
#define pinv(A,Ainv) (mp_pinv((A),(Ainv),R))
#define A(i,r) A[offset2((i),(r),(R))]
#define mtx(n,i,r) mtx[(n)][offset2((i),(r),(R))]
#define ata(n,r,q) ata[offset3((n),(r),(q),(R),(R))]
#define X(m) X[(m)]
#define V(r,q) V[offset2((r),(q),(R))]
#define Vinv(q,r) Vinv[offset2((q),(r),(R))]
#define ws(i,r) ws[offset2((i),(r),(R))]
#define sums(s) sums[(s)]
#define lmbda(s) lmbda[(s)]
#define crd(t,n,m,r,p) crd[(p)]
#define crd1(t,n,m,r) crd[(n)]
#define dim(n) dim[(n)]
#define dim1(t,n) dim[(n)]
#define bp(b) bp[(b)]
#define bp1(b) bp[(b)+1]
#define ind(t,n,b,m,p) (B*bindices[offset2((b),(p),N)]+eindices[offset2((m),(p),N)])

void cp_als_nd_hicoo(const float* X, const unsigned B, const unsigned M, const unsigned N, const unsigned NB, const unsigned R, const unsigned T, const unsigned* dim, const unsigned short* bindices, const unsigned* bp, const unsigned char* eindices, float** mtx, float* lmbda);
inline void cp_als_nd_hicoo(const float* X, const unsigned B, const unsigned M, const unsigned N, const unsigned NB, const unsigned R, const unsigned T, const unsigned* dim, const unsigned short* bindices, const unsigned* bp, const unsigned char* eindices, float** mtx, float* lmbda) {
    int t2,t4,t6,t7,t8,t10,t12,t14,t16,t18,t20,t22;
    unsigned D = 0;
    float* __restrict ata = (float*) calloc((N)*(R)*(R),sizeof(float));
    unsigned* __restrict crd = (unsigned*) calloc((N),sizeof(unsigned));
    float prod;
    float* __restrict V = (float*) calloc((R)*(R),sizeof(float));
    float* __restrict Vinv = (float*) calloc((R)*(R),sizeof(float));
    float* __restrict ws; // = (float*) calloc((D)*(R),sizeof(float));
    float* __restrict sums = (float*) calloc((R),sizeof(float));

    srand(1568224077);

// dmax
#undef s0
#define s0(n) D=max(D,dim((n)))
// init
#undef s1
#define s1(n,i,r) mtx((n),(i),(r))=urand(0)
// ata0
#undef s2
#define s2(n,r,q,i) ata((n),(r),(q))+=mtx((n),(r),(i))*mtx((n),(i),(q))
// icopy
#undef s3
#define s3(t,n,m,p,i) crd[(p)]=(i)
// pinit
#undef s4
#define s4(t,n,m,r) prod=1.000000
// pprod
#undef s5
//#define s5(t,n,m,r,p,i) if ((n) != (p)) prod*=mtx((p),(i),(r))
#define s5(t,n,m,r,p,i) prod*=mtx((p),(i),(r))
// krp
#undef s6
#define s6(t,n,m,r,i) mtx((n),(i),(r))+=X((m))*prod
// vinit
#undef s7
#define s7(t,n,r,q) V((r),(q))=1.000000
// vhad
#undef s8
#define s8(t,n,r,q,p) V((r),(q))*=ata((p),(r),(q))
// inv
#undef s9
#define s9(t,n) Vinv=pinv(V,Vinv)
// mmp
#undef s10
#define s10(t,n,i,r,q) ws((i),(r))+=mtx((n),(i),(q))*Vinv((q),(r))
// wscpy
#undef s11
#define s11(t,n,i,r) mtx((n),(i),(r))=ws((i),(r))
// ssq
#undef s12
#define s12(t,n,i,r) sums((r))+=ws((i),(r))*ws((i),(r))
// wszro
#undef s13
#define s13(t,n,i,r) ws((i),(r))=0.000000
// norm
#undef s14
#define s14(t,n,r) lmbda((r))=sqrt(sums((r)))
// div
#undef s15
#define s15(t,n,r,i) mtx((n),(i),(r))/=lmbda((r))
// zata
#undef s16
#define s16(t,n,r,q) ata((n),(r),(q))=0.000000
// ata
#undef s17
#define s17(t,n,r,q,i) ata((n),(r),(q))+=mtx((n),(r),(i))*mtx((n),(i),(q))

for(t2 = 0; t2 <= N-1; t2++) {
  s0(t2);
  #pragma omp parallel for schedule(auto) firstprivate(t2)
  for(t4 = 0; t4 <= dim(t2)-1; t4++) {
    for(t6 = 0; t6 <= R-1; t6++) {
      s1(t2,t4,t6);
      #pragma omp simd
      for(t8 = 0; t8 <= R-1; t8++) {
        s2(t2,t6,t8,t4);
      }
    }
  }
}
ws = (float*) calloc((D)*(R),sizeof(float));
for(t2 = 0; t2 <= T-1; t2++) {
  for(t4 = 0; t4 <= N-1; t4++) {
    #pragma omp parallel for schedule(auto) firstprivate(t2)
    for (t6 = 0; t6 <= NB-1; t6++) {
      for(t7 = bp(t6); t7 <= bp1(t6)-1; t7++) {
        #pragma omp simd
        for (t8 = 0; t8 <= N-1; t8++) {
          t10=ind(t2,t4,t6,t7,t8);
          s3(t2,t4,t7,t8,t10);
        }
        for (t8 = 0; t8 <= R-1; t8++) {
          s4(t2,t4,t7,t8);
          #pragma omp simd
          for (t10 = 0; t10 <= t4-1; t10++) {
            t12=crd(t2,t4,t7,t8,t10);
            s5(t2,t4,t7,t8,t10,t12);
          }
          #pragma omp simd
          for (t10 = t4+1; t10 <= N-1; t10++) {
            t12=crd(t2,t4,t6,t8,t10);
            s5(t2,t4,t7,t8,t10,t12);
          }
          t10=crd1(t2,t4,t7,t8);
          s6(t2,t4,t7,t8,t10);
        }
      }
    }
    #pragma omp parallel for schedule(auto) firstprivate(t2,t4)
    for(t6 = 0; t6 <= R-1; t6++) {
      for(t8 = 0; t8 <= R-1; t8++) {
        s7(t2,t4,t6,t8);
        #pragma omp simd
        for(t10 = 0; t10 <= t4-1; t10++) {
          s8(t2,t4,t6,t8,t10);
        }
        #pragma omp simd
        for(t10 = t4+1; t10 <= N-1; t10++) {
          s8(t2,t4,t6,t8,t10);
        }
      }
    }
    s9(t2,t4);
    #pragma omp parallel for schedule(auto) firstprivate(t2,t4)
    for(t6 = 0; t6 <= dim1(t2,t4)-1; t6++) {
      for(t8 = 0; t8 <= R-1; t8++) {
        #pragma omp simd
        for(t10 = 0; t10 <= R-1; t10++) {
          s10(t2,t4,t6,t8,t10);
        }
      }
    }
    #pragma omp parallel for schedule(auto) firstprivate(t2,t4)
    for(t6 = 0; t6 <= dim1(t2,t4)-1; t6++) {
      #pragma omp simd
      for(t8 = 0; t8 <= R-1; t8++) {
        s11(t2,t4,t6,t8);
        s12(t2,t4,t6,t8);
        s13(t2,t4,t6,t8);
      }
    }
    //#pragma omp parallel for schedule(auto) firstprivate(t2,t4,t6,t8)
    for(t6 = 0; t6 <= R-1; t6++) {
      s14(t2,t4,t6);
      sums(t6)=0.0;
      //#pragma omp simd
      #pragma omp parallel for schedule(auto) firstprivate(t2,t4,t6)
      for(t8 = 0; t8 <= dim1(t2,t4)-1; t8++) {
        s15(t2,t4,t6,t8);
      }
    }
    //#pragma omp parallel for schedule(auto) firstprivate(t2,t4,t6,t8)
    for(t6 = 0; t6 <= R-1; t6++) {
      for(t8 = 0; t8 <= R-1; t8++) {
        s16(t2,t4,t6,t8);
        //#pragma omp simd
        #pragma omp parallel for schedule(auto) firstprivate(t2,t4,t6,t8)
        for(t10 = 0; t10 <= dim1(t2,t4)-1; t10++) {
          s17(t2,t4,t6,t8,t10);
        }
      }
    }
  }
}

    free(ata);
    free(crd);
    free(V);
    free(Vinv);
    free(ws);
    free(sums);
}    // cp_als_nd

#undef min
#undef max
#undef abs
#undef floord
#undef offset2
#undef offset3
#undef offset4
#undef arrinit
#undef arrprnt
#undef urand
#undef pinv
#undef mtx
#undef ata
#undef X
#undef V
#undef Vinv
#undef ws
#undef sums
#undef lmbda
#undef crd
#undef dim
#undef ind

