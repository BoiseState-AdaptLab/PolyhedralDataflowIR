// 'cp_als_3d_coo' code generated by 'edavis' at 11/05/2019 11:17:00
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <math.h>
#include <string.h>
#include <float.h>
#include <time.h>

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
#define N 3
#define urand(m) (rand()/nextafter(RAND_MAX,DBL_MAX)+(m))
#define pinv(A,Ainv) (mp_pinv((A),(Ainv),R))
#define A(i,r) A[offset2((i),(r),(R))]
#define B(j,r) B[offset2((j),(r),(R))]
#define C(k,r) C[offset2((k),(r),(R))]
#define V(r,q) V[offset2((r),(q),(R))]
#define Y(r,q) Y[offset2((r),(q),(R))]
#define Anew(i,r) Anew[offset2((i),(r),(R))]
#define X(m) X[(m)]
#define Vinv(r,q) Vinv[offset2((r),(q),(R))]
#define sums(s) sums[(s)]
#define lmbda(r) lmbda[(r)]
#define Bnew(j,r) Bnew[offset2((j),(r),(R))]
#define Cnew(k,r) Cnew[offset2((k),(r),(R))]
#define bp(b) bp[(b)]
#define bp1(b) bp[(b)+1]
#define bind0(b) bindices[offset2((b),0,N)]
#define bind1(b) bindices[offset2((b),1,N)]
#define bind2(b) bindices[offset2((b),2,N)]
#define eind0(b,bi,bj,bk,m) eindices[offset2((m),0,N)]
#define eind1(b,bi,bj,bk,m) eindices[offset2((m),1,N)]
#define eind2(b,bi,bj,bk,m) eindices[offset2((m),2,N)]

void cp_als_3d_hicoo(const float* X, const unsigned BS, const unsigned I, const unsigned J, const unsigned K, const unsigned M, const unsigned NB, const unsigned R, const unsigned T, const unsigned short* bindices, const unsigned* bp, const unsigned char* eindices, float* A, float* B, float* C, float* lmbda);
inline void cp_als_3d_hicoo(const float* X, const unsigned BS, const unsigned I, const unsigned J, const unsigned K, const unsigned M, const unsigned NB, const unsigned R, const unsigned T, const unsigned short* bindices, const unsigned* bp, const unsigned char* eindices, float* A, float* B, float* C, float* lmbda) {
    unsigned t1,t2,t3,t4,t5,t6,t7,t8,t9,t10,t11,t12,t13,t14,t15;
    float* __restrict V = (float*) calloc((R)*(R),sizeof(float));
    float* __restrict Y = (float*) calloc((R)*(R),sizeof(float));
    float* __restrict Anew = (float*) calloc((I)*(R),sizeof(float));
    float* __restrict Vinv = (float*) calloc((R)*(R),sizeof(float));
    float* __restrict sums = (float*) calloc((R),sizeof(float));
    float* __restrict Bnew = (float*) calloc((J)*(R),sizeof(float));
    float* __restrict Cnew = (float*) calloc((K)*(R),sizeof(float));
    float dot;

    srand(1568224077);

// Ainit
#undef s0
#define s0(r,i) A((i),(r))=0.000000
// Binit
#undef s1
#define s1(r,j) B((j),(r))=urand(0)
// Cinit
#undef s2
#define s2(r,k) C((k),(r))=urand(0)
// Vinit0
#undef s3
#define s3(t,r,q) V((r),(q))=1.000000
// Yinit0
#undef s4
#define s4(t,r,q) Y((r),(q))=0.000000
// BYmm0
#undef s5
#define s5(t,r,q,j) Y((r),(q))+=B((q),(j))*B((j),(r))
// VYhp0
#undef s6
#define s6(t,r,q) V((r),(q))*=Y((r),(q))
// Yinit1
#undef s7
#define s7(t,r,q) Y((r),(q))=0.000000
// CYmm1
#undef s8
#define s8(t,r,q,k) Y((r),(q))+=C((q),(k))*C((k),(r))
// VYhp1
#undef s9
#define s9(t,r,q) V((r),(q))*=Y((r),(q))
// Akrp
#undef s10
//#define s10(t,m,i,j,k,r) Anew((i),(r))+=X((m))*C((k),(r))*B((j),(r))
#define s10(t,b,bi,bj,bk,m,ei,ej,ek,i,j,k,r) Anew((i),(r))+=X((m))*C((k),(r))*B((j),(r))
// Apinv
#undef s11
#define s11(t) Vinv=pinv(V,Vinv)
// Ammp
#undef s12
#define s12(t,i,r,q) dot+=Anew((i),(q))*Vinv((r),(q))
// Assq
#undef s13
#define s13(t,s,i) sums((s))+=Anew((i),(s))*Anew((i),(s))
// Anorm
#undef s14
#define s14(t,r) lmbda((r))=sqrt(sums((r)))
// Adiv
#undef s15
#define s15(t,s,i) A((i),(s))=Anew((i),(s))/lmbda((s))
// Vinit1
#undef s16
#define s16(t,r,q) V((r),(q))=1.000000
// Yinit2
#undef s17
#define s17(t,r,q) Y((r),(q))=0.000000
// AYmm2
#undef s18
#define s18(t,r,q,i) Y((r),(q))+=A((q),(i))*A((i),(r))
// VYhp2
#undef s19
#define s19(t,r,q) V((r),(q))*=Y((r),(q))
// Yinit3
#undef s20
#define s20(t,r,q) Y((r),(q))=0.000000
// CYmm3
#undef s21
#define s21(t,r,q,k) Y((r),(q))+=C((q),(k))*C((k),(r))
// VYhp3
#undef s22
#define s22(t,r,q) V((r),(q))*=Y((r),(q))
// Bkrp
#undef s23
//#define s23(t,m,i,j,k,r) Bnew((j),(r))+=X((m))*C((k),(r))*A((i),(r))
#define s23(t,b,bi,bj,bk,m,ei,ej,ek,i,j,k,r) Bnew((j),(r))+=X((m))*C((k),(r))*A((i),(r))
// Bpinv
#undef s24
#define s24(t) Vinv=pinv(V,Vinv)
// Bmmp
#undef s25
#define s25(t,j,r,q) dot+=Bnew((j),(q))*Vinv((r),(q))
// Bssq
#undef s26
#define s26(t,s,j) sums((s))+=Bnew((j),(s))*Bnew((j),(s))
// Bnorm
#undef s27
#define s27(t,r) lmbda((r))=sqrt(sums((r)))
// Bdiv
#undef s28
#define s28(t,s,j) B((j),(s))=Bnew((j),(s))/lmbda((s))
// Vinit2
#undef s29
#define s29(t,r,q) V((r),(q))=1.000000
// Yinit4
#undef s30
#define s30(t,r,q) Y((r),(q))=0.000000
// AYmm4
#undef s31
#define s31(t,r,q,i) Y((r),(q))+=A((q),(i))*A((i),(r))
// VYhp4
#undef s32
#define s32(t,r,q) V((r),(q))*=Y((r),(q))
// Yinit5
#undef s33
#define s33(t,r,q) Y((r),(q))=0.000000
// BYmm5
#undef s34
#define s34(t,r,q,j) Y((r),(q))+=B((q),(j))*B((j),(r))
// VYhp5
#undef s35
#define s35(t,r,q) V((r),(q))*=Y((r),(q))
// Ckrp
#undef s36
//#define s36(t,m,i,j,k,r) Cnew((k),(r))+=X((m))*B((j),(r))*A((i),(r))
#define s36(t,b,bi,bj,bk,m,ei,ej,ek,i,j,k,r) Cnew((k),(r))+=X((m))*B((j),(r))*A((i),(r))
// Cpinv
#undef s37
#define s37(t) Vinv=pinv(V,Vinv)
// Cmmp
#undef s38
#define s38(t,k,r,q) dot+=Cnew((k),(q))*Vinv((r),(q))
// Cssq
#undef s39
#define s39(t,s,k) sums((s))+=Cnew((k),(s))*Cnew((k),(s))
// Cnorm
#undef s40
#define s40(t,r) lmbda((r))=sqrt(sums((r)))
// Cdiv
#undef s41
#define s41(t,s,k) C((k),(s))=Cnew((k),(s))/lmbda((s))

for(t2 = 0; t2 <= R-1; t2++) {
  for(t4 = 0; t4 <= I-1; t4++) {
    s0(t2,t4);
  }
  for(t4 = 0; t4 <= J-1; t4++) {
    s1(t2,t4);
  }
  for(t4 = 0; t4 <= K-1; t4++) {
    s2(t2,t4);
  }
}
for(t2 = 0; t2 <= T-1; t2++) {
  for(t4 = 0; t4 <= R-1; t4++) {
    for(t6 = 0; t6 <= R-1; t6++) {
      s3(t2,t4,t6);
    }
  }
  for(t4 = 0; t4 <= R-1; t4++) {
    for(t6 = 0; t6 <= R-1; t6++) {
      s4(t2,t4,t6);
      for(t8 = 0; t8 <= J-1; t8++) {
        s5(t2,t4,t6,t8);
      }
    }
  }
  for(t4 = 0; t4 <= R-1; t4++) {
    for(t6 = 0; t6 <= R-1; t6++) {
      s6(t2,t4,t6);
    }
  }
  for(t4 = 0; t4 <= R-1; t4++) {
    for(t6 = 0; t6 <= R-1; t6++) {
      s7(t2,t4,t6);
      for(t8 = 0; t8 <= K-1; t8++) {
        s8(t2,t4,t6,t8);
      }
    }
  }
  for(t4 = 0; t4 <= R-1; t4++) {
    for(t6 = 0; t6 <= R-1; t6++) {
      s9(t2,t4,t6);
    }
  }
  for(t4 = 0; t4 < NB; t4++) {
    t5=bind0(t4);
    t6=bind1(t4);
    t7=bind2(t4);
    for(t8 = bp(t4); t8 < bp1(t4); t8++) {
      t9=eind0(t4,t5,t6,t7,t8);
      t10=eind1(t4,t5,t6,t7,t8);
      t11=eind2(t4,t5,t6,t7,t8);
      t12=BS*t5+t9;
      t13=BS*t6+t10;
      t14=BS*t7+t11;
      //#pragma omp simd
      for(t15 = 0; t15 < R; t15++) {
        s10(t2,t4,t5,t6,t7,t8,t9,t10,t11,t12,t13,t14,t15);
      }
    }
  }
  s11(t2);
  for(t4 = 0; t4 <= I-1; t4++) {
    for(t6 = 0; t6 <= R-1; t6++) {
      dot=0.0;
      for(t8 = 0; t8 <= R-1; t8++) {
        s12(t2,t4,t6,t8);
      }
      Anew((t4),(t6))=dot;
    }
  }
  for(t4 = 0; t4 <= R-1; t4++) {
    for(t6 = 0; t6 <= I-1; t6++) {
      s13(t2,t4,t6);
    }
  }
  for(t4 = 0; t4 <= R-1; t4++) {
    s14(t2,t4);
  }
  for(t4 = 0; t4 <= R-1; t4++) {
    for(t6 = 0; t6 <= I-1; t6++) {
      s15(t2,t4,t6);
    }
  }
  for(t4 = 0; t4 <= R-1; t4++) {
    for (t6 = 0; t6 <= R - 1; t6++) {
      s16(t2, t4, t6);
    }
  }
  for(t4 = 0; t4 <= R-1; t4++) {
    for (t6 = 0; t6 <= R-1; t6++) {
      s17(t2, t4, t6);
      for (t8 = 0; t8 <= I-1; t8++) {
        s18(t2,t4,t6,t8);
      }
    }
  }
  for(t4 = 0; t4 <= R-1; t4++) {
    for(t6 = 0; t6 <= R-1; t6++) {
      s19(t2,t4,t6);
    }
  }
  for(t4 = 0; t4 <= R-1; t4++) {
    for(t6 = 0; t6 <= R-1; t6++) {
      s20(t2,t4,t6);
      for(t8 = 0; t8 <= K-1; t8++) {
        s21(t2,t4,t6,t8);
      }
    }
  }
  for(t4 = 0; t4 <= R-1; t4++) {
    for(t6 = 0; t6 <= R-1; t6++) {
      s22(t2,t4,t6);
    }
  }
  for(t4 = 0; t4 < NB; t4++) {
    t5=bind0(t4);
    t6=bind1(t4);
    t7=bind2(t4);
    for(t8 = bp(t4); t8 < bp1(t4); t8++) {
      t9=eind0(t4,t5,t6,t7,t8);
      t10=eind1(t4,t5,t6,t7,t8);
      t11=eind2(t4,t5,t6,t7,t8);
      t12=BS*t5+t9;
      t13=BS*t6+t10;
      t14=BS*t7+t11;
      //#pragma omp simd
      for(t15 = 0; t15 < R; t15++) {
        s23(t2,t4,t5,t6,t7,t8,t9,t10,t11,t12,t13,t14,t15);
      }
    }
  }
  s24(t2);
  for(t4 = 0; t4 <= J-1; t4++) {
    for(t6 = 0; t6 <= R-1; t6++) {
      dot = 0.0;
      for(t8 = 0; t8 <= R-1; t8++) {
        s25(t2,t4,t6,t8);
      }
      Bnew((t4),(t6)) = dot;
    }
  }
  for(t4 = 0; t4 <= R-1; t4++) {
    for(t6 = 0; t6 <= J-1; t6++) {
      s26(t2,t4,t6);
    }
  }
  for(t4 = 0; t4 <= R-1; t4++) {
    s27(t2,t4);
  }
  for(t4 = 0; t4 <= R-1; t4++) {
    for(t6 = 0; t6 <= J-1; t6++) {
      s28(t2,t4,t6);
    }
  }
  for(t4 = 0; t4 <= R-1; t4++) {
    for (t6 = 0; t6 <= R-1; t6++) {
      s29(t2,t4,t6);
    }
  }
  for(t4 = 0; t4 <= R-1; t4++) {
    for(t6 = 0; t6 <= R-1; t6++) {
      s30(t2,t4,t6);
      for(t8 = 0; t8 <= I-1; t8++) {
        s31(t2,t4,t6,t8);
      }
    }
  }
  for(t4 = 0; t4 <= R-1; t4++) {
    for (t6 = 0; t6 <= R-1; t6++) {
      s32(t2,t4,t6);
    }
  }
  for(t4 = 0; t4 <= R-1; t4++) {
    for(t6 = 0; t6 <= R-1; t6++) {
      s33(t2,t4,t6);
      for(t8 = 0; t8 <= J-1; t8++) {
        s34(t2,t4,t6,t8);
      }
    }
  }
  for(t4 = 0; t4 <= R-1; t4++) {
    for(t6 = 0; t6 <= R-1; t6++) {
      s35(t2,t4,t6);
    }
  }
  for(t4 = 0; t4 < NB; t4++) {
    t5=bind0(t4);
    t6=bind1(t4);
    t7=bind2(t4);
    for(t8 = bp(t4); t8 < bp1(t4); t8++) {
      t9=eind0(t4,t5,t6,t7,t8);
      t10=eind1(t4,t5,t6,t7,t8);
      t11=eind2(t4,t5,t6,t7,t8);
      t12=BS*t5+t9;
      t13=BS*t6+t10;
      t14=BS*t7+t11;
      //#pragma omp simd
      for(t15 = 0; t15 < R; t15++) {
        s36(t2,t4,t5,t6,t7,t8,t9,t10,t11,t12,t13,t14,t15);
      }
    }
  }
  s37(t2);
  for(t4 = 0; t4 <= K-1; t4++) {
    for(t6 = 0; t6 <= R-1; t6++) {
      dot = 0.0;
      for(t8 = 0; t8 <= R-1; t8++) {
        s38(t2,t4,t6,t8);
      }
      Cnew((t4),(t6)) = dot;
    }
  }
  for(t4 = 0; t4 <= R-1; t4++) {
    for(t6 = 0; t6 <= K-1; t6++) {
      s39(t2,t4,t6);
    }
  }
  for(t4 = 0; t4 <= R-1; t4++) {
    s40(t2,t4);
  }
  for(t4 = 0; t4 <= R-1; t4++) {
    for(t6 = 0; t6 <= K-1; t6++) {
      s41(t2,t4,t6);
    }
  }
}

    free(V);
    free(Y);
    free(Anew);
    free(Vinv);
    free(sums);
    free(Bnew);
    free(Cnew);
}    // cp_als_3d_coo

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
#undef A
#undef B
#undef C
#undef V
#undef Y
#undef Anew
#undef X
#undef Vinv
#undef sums
#undef lmbda
#undef Bnew
#undef Cnew
#undef ind0
#undef ind1
#undef ind2

