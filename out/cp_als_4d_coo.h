// 'cp_als_4d_coo' code generated by 'edavis' at 11/08/2019 15:43:31
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <math.h>
#include <string.h>
#include <float.h>
#include <time.h>
#include <linalg.h>

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
#define B(j,r) B[offset2((j),(r),(R))]
#define C(k,r) C[offset2((k),(r),(R))]
#define D(l,r) D[offset2((l),(r),(R))]
#define V(r,q) V[offset2((r),(q),(R))]
#define Y(r,q) Y[offset2((r),(q),(R))]
#define Anew(i,r) Anew[offset2((i),(r),(R))]
#define X(m) X[(m)]
#define Vinv(r,q) Vinv[offset2((r),(q),(R))]
#define sums(s) sums[(s)]
#define lmbda(r) lmbda[(r)]
#define Bnew(j,r) Bnew[offset2((j),(r),(R))]
#define Cnew(k,r) Cnew[offset2((k),(r),(R))]
#define Dnew(l,r) Dnew[offset2((l),(r),(R))]
#define ind0(t,m) ind0[(m)]
#define ind1(t,m) ind1[(m)]
#define ind2(t,m) ind2[(m)]
#define ind3(t,m) ind3[(m)]

void cp_als_4d_coo(const float* X, const unsigned I, const unsigned J, const unsigned K, const unsigned L, const unsigned M, const unsigned R, const unsigned T, const unsigned* ind0, const unsigned* ind1, const unsigned* ind2, const unsigned* ind3, float* A, float* B, float* C, float* D, float* lmbda);
inline void cp_als_4d_coo(const float* X, const unsigned I, const unsigned J, const unsigned K, const unsigned L, const unsigned M, const unsigned R, const unsigned T, const unsigned* ind0, const unsigned* ind1, const unsigned* ind2, const unsigned* ind3, float* A, float* B, float* C, float* D, float* lmbda) {
    unsigned t1,t2,t3,t4,t5,t6,t7,t8,t9,t10,t11,t12,t13,t14,t15;
    float* __restrict V = (float*) calloc((R)*(R),sizeof(float));
    float* __restrict Y = (float*) calloc((R)*(R),sizeof(float));
    float* __restrict Anew = (float*) calloc((I)*(R),sizeof(float));
    float* __restrict Vinv = (float*) calloc((R)*(R),sizeof(float));
    float* __restrict sums = (float*) calloc((R),sizeof(float));
    float* __restrict Bnew = (float*) calloc((J)*(R),sizeof(float));
    float* __restrict Cnew = (float*) calloc((K)*(R),sizeof(float));
    float* __restrict Dnew = (float*) calloc((L)*(R),sizeof(float));
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
// Dinit
#undef s3
#define s3(r,l) D((l),(r))=urand(0)
// Vinit0
#undef s4
#define s4(t,r,q) V((r),(q))=1.000000
// Yinit0
#undef s5
#define s5(t,r,q) Y((r),(q))=0.000000
// BYmm0
#undef s6
#define s6(t,r,q,j) Y((r),(q))+=B((q),(j))*B((j),(r))
// VYhp0
#undef s7
#define s7(t,r,q) V((r),(q))*=Y((r),(q))
// Yinit1
#undef s8
#define s8(t,r,q) Y((r),(q))=0.000000
// CYmm1
#undef s9
#define s9(t,r,q,k) Y((r),(q))+=C((q),(k))*C((k),(r))
// VYhp1
#undef s10
#define s10(t,r,q) V((r),(q))*=Y((r),(q))
// Yinit2
#undef s11
#define s11(t,r,q) Y((r),(q))=0.000000
// DYmm2
#undef s12
#define s12(t,r,q,l) Y((r),(q))+=D((q),(l))*D((l),(r))
// VYhp2
#undef s13
#define s13(t,r,q) V((r),(q))*=Y((r),(q))
// Akrp
#undef s14
#define s14(t,m,i,j,k,l,r) Anew((i),(r))+=X((m))*D((l),(r))*C((k),(r))*B((j),(r))
// Apinv
#undef s15
#define s15(t) Vinv=pinv(V,Vinv)
// Ammp
#undef s16
#define s16(t,i,r,q) Anew((i),(r))+=Anew((i),(q))*Vinv((r),(q))
// Assq
#undef s17
#define s17(t,s,i) sums((s))+=Anew((i),(s))*Anew((i),(s))
// Anorm
#undef s18
#define s18(t,r) lmbda((r))=sqrt(sums((r)))
// Adiv
#undef s19
#define s19(t,s,i) A((i),(s))=Anew((i),(s))/lmbda((s))
// Vinit1
#undef s20
#define s20(t,r,q) V((r),(q))=1.000000
// Yinit3
#undef s21
#define s21(t,r,q) Y((r),(q))=0.000000
// AYmm3
#undef s22
#define s22(t,r,q,i) Y((r),(q))+=A((q),(i))*A((i),(r))
// VYhp3
#undef s23
#define s23(t,r,q) V((r),(q))*=Y((r),(q))
// Yinit4
#undef s24
#define s24(t,r,q) Y((r),(q))=0.000000
// CYmm4
#undef s25
#define s25(t,r,q,k) Y((r),(q))+=C((q),(k))*C((k),(r))
// VYhp4
#undef s26
#define s26(t,r,q) V((r),(q))*=Y((r),(q))
// Yinit5
#undef s27
#define s27(t,r,q) Y((r),(q))=0.000000
// DYmm5
#undef s28
#define s28(t,r,q,l) Y((r),(q))+=D((q),(l))*D((l),(r))
// VYhp5
#undef s29
#define s29(t,r,q) V((r),(q))*=Y((r),(q))
// Bkrp
#undef s30
#define s30(t,m,i,j,k,l,r) Bnew((j),(r))+=X((m))*D((l),(r))*C((k),(r))*A((i),(r))
// Bpinv
#undef s31
#define s31(t) Vinv=pinv(V,Vinv)
// Bmmp
#undef s32
#define s32(t,j,r,q) Bnew((j),(r))+=Bnew((j),(q))*Vinv((r),(q))
// Bssq
#undef s33
#define s33(t,s,j) sums((s))+=Bnew((j),(s))*Bnew((j),(s))
// Bnorm
#undef s34
#define s34(t,r) lmbda((r))=sqrt(sums((r)))
// Bdiv
#undef s35
#define s35(t,s,j) B((j),(s))=Bnew((j),(s))/lmbda((s))
// Vinit2
#undef s36
#define s36(t,r,q) V((r),(q))=1.000000
// Yinit6
#undef s37
#define s37(t,r,q) Y((r),(q))=0.000000
// AYmm6
#undef s38
#define s38(t,r,q,i) Y((r),(q))+=A((q),(i))*A((i),(r))
// VYhp6
#undef s39
#define s39(t,r,q) V((r),(q))*=Y((r),(q))
// Yinit7
#undef s40
#define s40(t,r,q) Y((r),(q))=0.000000
// BYmm7
#undef s41
#define s41(t,r,q,j) Y((r),(q))+=B((q),(j))*B((j),(r))
// VYhp7
#undef s42
#define s42(t,r,q) V((r),(q))*=Y((r),(q))
// Yinit8
#undef s43
#define s43(t,r,q) Y((r),(q))=0.000000
// DYmm8
#undef s44
#define s44(t,r,q,l) Y((r),(q))+=D((q),(l))*D((l),(r))
// VYhp8
#undef s45
#define s45(t,r,q) V((r),(q))*=Y((r),(q))
// Ckrp
#undef s46
#define s46(t,m,i,j,k,l,r) Cnew((k),(r))+=X((m))*D((l),(r))*B((j),(r))*A((i),(r))
// Cpinv
#undef s47
#define s47(t) Vinv=pinv(V,Vinv)
// Cmmp
#undef s48
#define s48(t,k,r,q) Cnew((k),(r))+=Cnew((k),(q))*Vinv((r),(q))
// Cssq
#undef s49
#define s49(t,s,k) sums((s))+=Cnew((k),(s))*Cnew((k),(s))
// Cnorm
#undef s50
#define s50(t,r) lmbda((r))=sqrt(sums((r)))
// Cdiv
#undef s51
#define s51(t,s,k) C((k),(s))=Cnew((k),(s))/lmbda((s))
// Vinit3
#undef s52
#define s52(t,r,q) V((r),(q))=1.000000
// Yinit9
#undef s53
#define s53(t,r,q) Y((r),(q))=0.000000
// AYmm9
#undef s54
#define s54(t,r,q,i) Y((r),(q))+=A((q),(i))*A((i),(r))
// VYhp9
#undef s55
#define s55(t,r,q) V((r),(q))*=Y((r),(q))
// Yinit10
#undef s56
#define s56(t,r,q) Y((r),(q))=0.000000
// BYmm10
#undef s57
#define s57(t,r,q,j) Y((r),(q))+=B((q),(j))*B((j),(r))
// VYhp10
#undef s58
#define s58(t,r,q) V((r),(q))*=Y((r),(q))
// Yinit11
#undef s59
#define s59(t,r,q) Y((r),(q))=0.000000
// CYmm11
#undef s60
#define s60(t,r,q,k) Y((r),(q))+=C((q),(k))*C((k),(r))
// VYhp11
#undef s61
#define s61(t,r,q) V((r),(q))*=Y((r),(q))
// Dkrp
#undef s62
#define s62(t,m,i,j,k,l,r) Dnew((l),(r))+=X((m))*C((k),(r))*B((j),(r))*A((i),(r))
// Dpinv
#undef s63
#define s63(t) Vinv=pinv(V,Vinv)
// Dmmp
#undef s64
#define s64(t,l,r,q) Dnew((l),(r))+=Dnew((l),(q))*Vinv((r),(q))
// Dssq
#undef s65
#define s65(t,s,l) sums((s))+=Dnew((l),(s))*Dnew((l),(s))
// Dnorm
#undef s66
#define s66(t,r) lmbda((r))=sqrt(sums((r)))
// Ddiv
#undef s67
#define s67(t,s,l) D((l),(s))=Dnew((l),(s))/lmbda((s))

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
  for(t4 = 0; t4 <= L-1; t4++) {
    s3(t2,t4);
  }
}
for(t2 = 0; t2 <= T-1; t2++) {
  for(t4 = 0; t4 <= R-1; t4++) {
    for(t6 = 0; t6 <= R-1; t6++) {
      s4(t2,t4,t6);
      s5(t2,t4,t6);
      s8(t2,t4,t6);
      s11(t2,t4,t6);
      s20(t2,t4,t6);
      s21(t2,t4,t6);
      s24(t2,t4,t6);
      s27(t2,t4,t6);
      s36(t2,t4,t6);
      s37(t2,t4,t6);
      s40(t2,t4,t6);
      s43(t2,t4,t6);
      s52(t2,t4,t6);
      s53(t2,t4,t6);
      s56(t2,t4,t6);
      s59(t2,t4,t6);
    }
  }
  for(t4 = 0; t4 <= R-1; t4++) {
    for(t6 = 0; t6 <= R-1; t6++) {
      for(t8 = 0; t8 <= J-1; t8++) {
        s6(t2,t4,t6,t8);
      }
      s7(t2,t4,t6);
    }
  }
  for(t4 = 0; t4 <= R-1; t4++) {
    for(t6 = 0; t6 <= R-1; t6++) {
      for(t8 = 0; t8 <= K-1; t8++) {
        s9(t2,t4,t6,t8);
      }
      s10(t2,t4,t6);
      for(t8 = 0; t8 <= K-1; t8++) {
        s25(t2,t4,t6,t8);
      }
    }
  }
  for(t4 = 0; t4 <= R-1; t4++) {
    for(t6 = 0; t6 <= R-1; t6++) {
      for(t8 = 0; t8 <= L-1; t8++) {
        s12(t2,t4,t6,t8);
      }
      s13(t2,t4,t6);
      for(t8 = 0; t8 <= L-1; t8++) {
        s28(t2,t4,t6,t8);
      }
      for(t8 = 0; t8 <= L-1; t8++) {
        s44(t2,t4,t6,t8);
      }
    }
  }
  for(t4 = 0; t4 <= M-1; t4++) {
    t6=ind0(t2,t4);
    t8=ind1(t2,t4);
    t10=ind2(t2,t4);
    t12=ind3(t2,t4);
    for(t14 = 0; t14 <= R-1; t14++) {
      s14(t2,t4,t6,t8,t10,t12,t14);
    }
  }
  s15(t2);
  for(t4 = 0; t4 <= I-1; t4++) {
    for(t6 = 0; t6 <= R-1; t6++) {
      for(t8 = 0; t8 <= R-1; t8++) {
        s16(t2,t4,t6,t8);
      }
    }
  }
  for(t4 = 0; t4 <= R-1; t4++) {
    for(t6 = 0; t6 <= I-1; t6++) {
      s17(t2,t4,t6);
    }
  }
  for(t4 = 0; t4 <= R-1; t4++) {
    s18(t2,t4);
  }
  for(t4 = 0; t4 <= R-1; t4++) {
    for(t6 = 0; t6 <= I-1; t6++) {
      s19(t2,t4,t6);
    }
  }
  for(t4 = 0; t4 <= R-1; t4++) {
    for(t6 = 0; t6 <= R-1; t6++) {
      for(t8 = 0; t8 <= I-1; t8++) {
        s22(t2,t4,t6,t8);
      }
      s23(t2,t4,t6);
      s26(t2,t4,t6);
      s29(t2,t4,t6);
      for(t8 = 0; t8 <= I-1; t8++) {
        s38(t2,t4,t6,t8);
      }
      s39(t2,t4,t6);
      for(t8 = 0; t8 <= I-1; t8++) {
        s54(t2,t4,t6,t8);
      }
      s55(t2,t4,t6);
    }
  }
  for(t4 = 0; t4 <= M-1; t4++) {
    t6=ind0(t2,t4);
    t8=ind1(t2,t4);
    t10=ind2(t2,t4);
    t12=ind3(t2,t4);
    for(t14 = 0; t14 <= R-1; t14++) {
      s30(t2,t4,t6,t8,t10,t12,t14);
    }
  }
  s31(t2);
  for(t4 = 0; t4 <= J-1; t4++) {
    for(t6 = 0; t6 <= R-1; t6++) {
      for(t8 = 0; t8 <= R-1; t8++) {
        s32(t2,t4,t6,t8);
      }
    }
  }
  for(t4 = 0; t4 <= R-1; t4++) {
    for(t6 = 0; t6 <= J-1; t6++) {
      s33(t2,t4,t6);
    }
  }
  for(t4 = 0; t4 <= R-1; t4++) {
    s34(t2,t4);
  }
  for(t4 = 0; t4 <= R-1; t4++) {
    for(t6 = 0; t6 <= J-1; t6++) {
      s35(t2,t4,t6);
    }
  }
  for(t4 = 0; t4 <= R-1; t4++) {
    for(t6 = 0; t6 <= R-1; t6++) {
      for(t8 = 0; t8 <= J-1; t8++) {
        s41(t2,t4,t6,t8);
      }
      s42(t2,t4,t6);
      s45(t2,t4,t6);
      for(t8 = 0; t8 <= J-1; t8++) {
        s57(t2,t4,t6,t8);
      }
      s58(t2,t4,t6);
    }
  }
  for(t4 = 0; t4 <= M-1; t4++) {
    t6=ind0(t2,t4);
    t8=ind1(t2,t4);
    t10=ind2(t2,t4);
    t12=ind3(t2,t4);
    for(t14 = 0; t14 <= R-1; t14++) {
      s46(t2,t4,t6,t8,t10,t12,t14);
    }
  }
  s47(t2);
  for(t4 = 0; t4 <= K-1; t4++) {
    for(t6 = 0; t6 <= R-1; t6++) {
      for(t8 = 0; t8 <= R-1; t8++) {
        s48(t2,t4,t6,t8);
      }
    }
  }
  for(t4 = 0; t4 <= R-1; t4++) {
    for(t6 = 0; t6 <= K-1; t6++) {
      s49(t2,t4,t6);
    }
  }
  for(t4 = 0; t4 <= R-1; t4++) {
    s50(t2,t4);
  }
  for(t4 = 0; t4 <= R-1; t4++) {
    for(t6 = 0; t6 <= K-1; t6++) {
      s51(t2,t4,t6);
    }
  }
  for(t4 = 0; t4 <= R-1; t4++) {
    for(t6 = 0; t6 <= R-1; t6++) {
      for(t8 = 0; t8 <= K-1; t8++) {
        s60(t2,t4,t6,t8);
      }
      s61(t2,t4,t6);
    }
  }
  for(t4 = 0; t4 <= M-1; t4++) {
    t6=ind0(t2,t4);
    t8=ind1(t2,t4);
    t10=ind2(t2,t4);
    t12=ind3(t2,t4);
    for(t14 = 0; t14 <= R-1; t14++) {
      s62(t2,t4,t6,t8,t10,t12,t14);
    }
  }
  s63(t2);
  for(t4 = 0; t4 <= L-1; t4++) {
    for(t6 = 0; t6 <= R-1; t6++) {
      for(t8 = 0; t8 <= R-1; t8++) {
        s64(t2,t4,t6,t8);
      }
    }
  }
  for(t4 = 0; t4 <= R-1; t4++) {
    for(t6 = 0; t6 <= L-1; t6++) {
      s65(t2,t4,t6);
    }
  }
  for(t4 = 0; t4 <= R-1; t4++) {
    s66(t2,t4);
  }
  for(t4 = 0; t4 <= R-1; t4++) {
    for(t6 = 0; t6 <= L-1; t6++) {
      s67(t2,t4,t6);
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
    free(Dnew);
}    // cp_als_4d_coo

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
#undef D
#undef V
#undef Y
#undef Anew
#undef X
#undef Vinv
#undef sums
#undef lmbda
#undef Bnew
#undef Cnew
#undef Dnew
#undef ind0
#undef ind1
#undef ind2
#undef ind3

