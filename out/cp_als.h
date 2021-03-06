// 'cp_als' code generated by 'edavis' at 07/06/2019 11:15:57
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
#define A(i,r) A[offset2((i4),(j4),(J))]
#define B(j,r) B[(i5)]
#define C(k,r) C[offset2((i6),(j6),(J))]
#define Csq(q,r) Csq[offset2((i41),(j41),(R))]
#define Bsq(q,r) Bsq[offset2((i40),(j40),(R))]
#define Ahp(q,r) Ahp[offset2((i42),(j42),(R))]
#define Ainv(q,r) Ainv[offset2((i45),(j45),(R))]
#define X(m) X[(i35)]
#define Akr(i,r) Akr[offset2((i36),(j36),(R))]
#define sums(r) sums[(i48)]
#define Asq(q,r) Asq[offset2((i39),(j39),(R))]
#define Bhp(q,r) Bhp[offset2((i43),(j43),(R))]
#define Binv(q,r) Binv[offset2((i46),(j46),(R))]
#define Bkr(j,r) Bkr[offset2((i37),(j37),(R))]
#define Chp(q,r) Chp[offset2((i44),(j44),(R))]
#define Cinv(q,r) Cinv[offset2((i47),(j47),(R))]
#define Ckr(k,r) Ckr[offset2((i38),(j38),(R))]
#define lmbda(r) lmbda[(i49)]
#define ind0(m) ind0[(m)]
#define ind1(m) ind1[(m)]
#define ind2(m) ind2[(m)]

void cp_als(const double* X, const double* r, const unsigned I, const unsigned J, const unsigned K, const unsigned M, const unsigned R, const unsigned* ind0, const unsigned* ind1, const unsigned* ind2, double* lmbda);
inline void cp_als(const double* X, const double* r, const unsigned I, const unsigned J, const unsigned K, const unsigned M, const unsigned R, const unsigned* ind0, const unsigned* ind1, const unsigned* ind2, double* lmbda) {
    unsigned t1,t2,t3,t4,t5;
    double* restrict A = malloc((I)*(J)*sizeof(double));
    unsigned* restrict B = malloc((NNZ)*sizeof(unsigned));
    unsigned* restrict C = malloc((K)*(J)*sizeof(unsigned));
    double* restrict Csq = malloc((R)*(R)*sizeof(double));
    double* restrict Bsq = malloc((R)*(R)*sizeof(double));
    double* restrict Ahp = malloc((R)*(R)*sizeof(double));
    double* restrict Ainv = malloc((R)*(R)*sizeof(double));
    double* restrict Akr = malloc((I)*(R)*sizeof(double));
    double* restrict sums = malloc((R)*sizeof(double));
    double* restrict Asq = malloc((R)*(R)*sizeof(double));
    double* restrict Bhp = malloc((R)*(R)*sizeof(double));
    double* restrict Binv = malloc((R)*(R)*sizeof(double));
    double* restrict Bkr = malloc((J)*(R)*sizeof(double));
    double* restrict Chp = malloc((R)*(R)*sizeof(double));
    double* restrict Cinv = malloc((R)*(R)*sizeof(double));
    double* restrict Ckr = malloc((K)*(R)*sizeof(double));

// Ainit
#undef s0
#define s0(i,r) A((i),(r))=urand(1)

for(t1 = 0; t1 <= I-1; t1++) {
  for(t2 = 0; t2 <= R-1; t2++) {
    s0(t1,t2);
  }
}

// Binit
#undef s0
#define s0(j,q) B((j),r)=urand(1)

for(t1 = 0; t1 <= J-1; t1++) {
  for(t2 = 0; t2 <= R-1; t2++) {
    s0(t1,t2);
  }
}

// Cinit
#undef s0
#define s0(k,r) C((k),(r))=urand(1)

for(t1 = 0; t1 <= K-1; t1++) {
  for(t2 = 0; t2 <= R-1; t2++) {
    s0(t1,t2);
  }
}

// Cmm
#undef s0
#define s0(q,r,k) Csq((q),(r))+=C((q),(k))*C((k),(r))

for(t1 = 0; t1 <= R-1; t1++) {
  for(t2 = 0; t2 <= R-1; t2++) {
    for(t3 = 0; t3 <= K-1; t3++) {
      s0(t1,t2,t3);
    }
  }
}

// Bmm
#undef s0
#define s0(q,r,j) Bsq((q),(r))+=B((q),(j))*B((j),(r))

for(t1 = 0; t1 <= R-1; t1++) {
  for(t2 = 0; t2 <= R-1; t2++) {
    for(t3 = 0; t3 <= J-1; t3++) {
      s0(t1,t2,t3);
    }
  }
}

// Ahad
#undef s0
#define s0(q,r) Ahp((q),(r))=Csq((q),(r))*Bsq((q),(r))

for(t1 = 0; t1 <= R-1; t1++) {
  for(t2 = 0; t2 <= R-1; t2++) {
    s0(t1,t2);
  }
}

// Apinv
#undef s0
#define s0(q,r) Ainv((q),(r))=1.000000/Ahp((q),(r))

for(t1 = 0; t1 <= R-1; t1++) {
  for(t2 = 0; t2 <= R-1; t2++) {
    s0(t1,t2);
  }
}

// Akrp
#undef s0
#define s0(m,i,j,k,r) Akr((i),(r))+=X((m))*C((k),(r))*B((j),(r))

for(t1 = 0; t1 <= M-1; t1++) {
  t2=ind0(t1);
  t3=ind1(t1);
  t4=ind2(t1);
  for(t5 = 0; t5 <= R-1; t5++) {
    s0(t1,t2,t3,t4,t5);
  }
}

// Apmm
#undef s0
#define s0(i,q,r) A((i),(q))+=Akr((i),(r))*Ainv((q),(r))

for(t1 = 0; t1 <= I-1; t1++) {
  for(t2 = 0; t2 <= R-1; t2++) {
    for(t3 = 0; t3 <= R-1; t3++) {
      s0(t1,t2,t3);
    }
  }
}

// Assq
#undef s0
#define s0(i,r) sums((r))+=A((i),(r))*A((i),(r))

for(t1 = 0; t1 <= I-1; t1++) {
  for(t2 = 0; t2 <= R-1; t2++) {
    s0(t1,t2);
  }
}

// Amm
#undef s0
#define s0(q,r,i) Asq((q),(r))+=A((q),(i))*A((i),(r))

for(t1 = 0; t1 <= R-1; t1++) {
  for(t2 = 0; t2 <= R-1; t2++) {
    for(t3 = 0; t3 <= I-1; t3++) {
      s0(t1,t2,t3);
    }
  }
}

// Bhad
#undef s0
#define s0(q,r) Bhp((q),(r))=Csq((q),(r))*Asq((q),(r))

for(t1 = 0; t1 <= R-1; t1++) {
  for(t2 = 0; t2 <= R-1; t2++) {
    s0(t1,t2);
  }
}

// Bpinv
#undef s0
#define s0(q,r) Binv((q),(r))=1.000000/Bhp((q),(r))

for(t1 = 0; t1 <= R-1; t1++) {
  for(t2 = 0; t2 <= R-1; t2++) {
    s0(t1,t2);
  }
}

// Bkrp
#undef s0
#define s0(m,i,j,k,r) Bkr((j),(r))+=X((m))*C((k),(r))*A((i),(r))

for(t1 = 0; t1 <= M-1; t1++) {
  t2=ind0(t1);
  t3=ind1(t1);
  t4=ind2(t1);
  for(t5 = 0; t5 <= R-1; t5++) {
    s0(t1,t2,t3,t4,t5);
  }
}

// Bpmm
#undef s0
#define s0(j,q,r) B((j),(q))+=Bkr((j),(r))*Binv((q),(r))

for(t1 = 0; t1 <= J-1; t1++) {
  for(t2 = 0; t2 <= R-1; t2++) {
    for(t3 = 0; t3 <= R-1; t3++) {
      s0(t1,t2,t3);
    }
  }
}

// Bssq
#undef s0
#define s0(j,q) sums(r)+=B((j),r)*B((j),r)

for(t1 = 0; t1 <= J-1; t1++) {
  for(t2 = 0; t2 <= R-1; t2++) {
    s0(t1,t2);
  }
}

// Chad
#undef s0
#define s0(q,r) Chp((q),(r))=Bsq((q),(r))*Asq((q),(r))

for(t1 = 0; t1 <= R-1; t1++) {
  for(t2 = 0; t2 <= R-1; t2++) {
    s0(t1,t2);
  }
}

// Cpinv
#undef s0
#define s0(q,r) Cinv((q),(r))=1.000000/Chp((q),(r))

for(t1 = 0; t1 <= R-1; t1++) {
  for(t2 = 0; t2 <= R-1; t2++) {
    s0(t1,t2);
  }
}

// Ckrp
#undef s0
#define s0(m,i,j,k,r) Ckr((k),(r))+=X((m))*B((j),(r))*A((i),(r))

for(t1 = 0; t1 <= M-1; t1++) {
  t2=ind0(t1);
  t3=ind1(t1);
  t4=ind2(t1);
  for(t5 = 0; t5 <= R-1; t5++) {
    s0(t1,t2,t3,t4,t5);
  }
}

// Cpmm
#undef s0
#define s0(k,q,r) C((k),(q))+=Ckr((k),(r))*Cinv((r),(r))

for(t1 = 0; t1 <= K-1; t1++) {
  for(t2 = 0; t2 <= R-1; t2++) {
    for(t3 = 0; t3 <= R-1; t3++) {
      s0(t1,t2,t3);
    }
  }
}

// Cssq
#undef s0
#define s0(k,r) sums((r))+=C((k),(r))*C((k),(r))

for(t1 = 0; t1 <= K-1; t1++) {
  for(t2 = 0; t2 <= R-1; t2++) {
    s0(t1,t2);
  }
}

// norm
#undef s0
#define s0(r) lmbda((r))=sqrt(sums((r)))

for(t1 = 0; t1 <= R-1; t1++) {
  s0(t1);
}

    free(A);
    free(B);
    free(C);
    free(Csq);
    free(Bsq);
    free(Ahp);
    free(Ainv);
    free(Akr);
    free(sums);
    free(Asq);
    free(Bhp);
    free(Binv);
    free(Bkr);
    free(Chp);
    free(Cinv);
    free(Ckr);
}    // cp_als

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
#undef A
#undef B
#undef C
#undef Csq
#undef Bsq
#undef Ahp
#undef Ainv
#undef X
#undef Akr
#undef sums
#undef Asq
#undef Bhp
#undef Binv
#undef Bkr
#undef Chp
#undef Cinv
#undef Ckr
#undef lmbda
#undef ind0
#undef ind1
#undef ind2

