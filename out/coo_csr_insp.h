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
#define rp(i) rp[(i)]

unsigned coo_csr_insp(const unsigned NNZ, const unsigned* row, unsigned* rp);
inline unsigned coo_csr_insp(const unsigned NNZ, const unsigned* row, unsigned* rp) {
    unsigned t1,t2,t3,t4,t5,t6,t7,t8,t9,t10,t11,t12,t13,t14,t15;
    unsigned N;

// inspN
#undef s0
#define s0() N=row(NNZ-1)+1

s0();

// insp_rp+insp_rp2
#undef s0
#define s0(n,i) if ((n) >= rp((i)+1)) rp((i)+1)=(n)+1
#undef s1
#define s1(n,i) if (rp((i)) > rp((i)+1)) rp((i)+1)=rp((i))

for(t2 = 0; t2 <= NNZ-1; t2++) {
  t4=row(t2);
  s0(t2,t4);
  s1(t2,t4);
}


    return (N);
}    // coo_csr_insp

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
#undef rp

