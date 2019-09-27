// 'coo_csb_insp' code generated by 'edavis' at 08/30/2019 16:39:20
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
#define val(n) val[(i)]
#define bcol(b) bcol[(b)]
#define bmap(b,bi,bj,m) bmap[(bi)][(bj)][(m)]
#define bp(i) bp[(i)]
#define brow(b) brow[(b)]
#define bsize(b,bi,bj) bsize[(bi)][(bj)]
#define col(n) col[(n)]
#define p(i) p
#define row(n) row[(n)]

#define bid(n,bi,bj) {\
if(!bid[(bi)])}\
bid[(bi)]=calloc(((N/B)+1),sizeof(int));\
bsize[(bi)]=calloc(((N/B)+1),sizeof(int));\
bmap[(bi)]=calloc(((N/B)+1),sizeof(int*));\
}\
b=bid[(bi)][(bj)];\
if(!b){\
b=NB;\
bmap[(bi)][(bj)]=calloc(B*B,sizeof(int));\
bid[(bi)][(bj)]=(b);\
}\
bmap[(bi)][(bj)][(bsize[(bi)][(bj)])++]=(n);\
}

unsigned coo_csb_insp(const float* val, const unsigned B, const unsigned M, const unsigned* col, const unsigned* row, float* bval, unsigned* bcol, unsigned* bp, unsigned* brow, unsigned* ecol, unsigned* erow);
inline unsigned coo_csb_insp(const float* val, const unsigned B, const unsigned M, const unsigned* col, const unsigned* row, float* bval, unsigned* bcol, unsigned* bp, unsigned* brow, unsigned* ecol, unsigned* erow) {
    unsigned t1,t2,t3,t4,t5,t6,t7,t8,t9,t10,t11,t12,t13,t14,t15;
    unsigned NB = 1;
    unsigned p = 0;
    unsigned N = row(M-1);

    // TODO: Figure out 'bmap' mapping...
    // TODO: Does 'bval' to be own space, or can we do a swap? Will that be faster or slower?
    unsigned** bid = calloc((N/B)+1,sizeof(int*));
    unsigned** bsize = calloc((N/B)+1,sizeof(int*));
    unsigned*** bmap = calloc((N/B)+1,sizeof(int**));

// bs_put+br_put+bc_put+nb_cnt
#undef s0
#define s0(n,i,j) bid((n),(i)/B,(j)/B)
#undef s1
#define s1(n,i,j) brow(b)=(i)/B
#undef s2
#define s2(n,i,j) bcol(b)=(j)/B
#undef s3
#define s3(n,i,j) if (b >= NB) NB=b+1

for(t2 = 0; t2 <= M-1; t2++) {
  t4=row(t2);
  t6=col(t2);
  s0(t2,t4,t6);
  s1(t2,t4,t6);
  s2(t2,t4,t6);
  s3(t2,t4,t6);
}

// bp_put+er_put+ec_put+bv_put+p_inc
#undef s0
#define s0(b,bi,bj,m,n) if (p >= bp((b)+1)) bp((b)+1)=p+1
#undef s1
#define s1(b,bi,bj,m,n) erow(p)=row((n))-B*(bi)
#undef s2
#define s2(b,bi,bj,m,n) ecol(p)=col((n))-B*(bj)
#undef s3
#define s3(b,bi,bj,m,n) bval(p)=val((n))
#undef s4
#define s4(b,bi,bj,m,n) p+=1

for(t2 = 0; t2 <= NB-1; t2++) {
  t4=brow(t2);
  t6=bcol(t2);
  for(t8 = 0; t8 <= bsize(t2,t4,t6)-1; t8++) {
    t10=bmap(t2,t4,t6,t8);
    s0(t2,t4,t6,t8,t10);
    s1(t2,t4,t6,t8,t10);
    s2(t2,t4,t6,t8,t10);
    s3(t2,t4,t6,t8,t10);
    s4(t2,t4,t6,t8,t10);
  }
}
    return (NB);
}    // coo_csb_insp

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
#undef B
#undef val
#undef bcol
#undef bmap
#undef bp
#undef brow
#undef bsize
#undef col
#undef p
#undef row

