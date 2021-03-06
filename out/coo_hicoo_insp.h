// 'coo_hicoo_insp' code generated by 'edavis' at 08/30/2019 16:39:20
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <math.h>
#include <string.h>
//#include <assert.h>
#include <limits.h>

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
#define offset5(i,j,k,l,m,M,N,P,Q) ((m)+((l)+((k)+((j)+(i)*(M))*(N))*(P))*(Q))
#define arrinit(ptr,val,size) for(unsigned __i__=0;__i__<(size);__i__++) (ptr)[__i__]=(val)
#define arrprnt(name,arr,size) {\
fprintf(stderr,"%s={",(name));\
for(unsigned __i__=0;__i__<(size);__i__++) fprintf(stderr,"%lg,",(arr)[__i__]);\
fprintf(stderr,"}\n");}
#define val(n) val[(n)]
#define bval(p) (*bval)[(p)]
#define bmap(b,bi,bj,bk,m) bmap[(bi)][(bj)][(bk)][(m)]
#define bp(b) (*bp)[(b)]
#define bsize(b,bi,bj,bk) bsize[(bi)][(bj)][(bk)]
#define p(i) p
#define ind(m,n) indices[offset2((m),(n),M)]
#define bind(m,b) (*bindices)[offset2((b),(m),ord)]
#define eind(m,n) (*eindices)[offset2((n),(m),ord)]

unsigned ilog2(unsigned x);
inline unsigned ilog2(unsigned x) {
#define clz(x) __builtin_clz((x))
    return sizeof(uint32_t) * CHAR_BIT - clz(x) - 1;
}

//#define bid(m,bi,bj,bk) {\
//if(!bmap[(bi)]){\
//bsize[(bi)]=(unsigned**)calloc(((J/B)+1),sizeof(unsigned*));\
//bid[(bi)]=(unsigned**)calloc(((J/B)+1),sizeof(unsigned*));\
//bmap[(bi)]=(unsigned***)calloc(((J/B)+1),sizeof(unsigned**));\
//}\
//if(!bmap[(bi)][(bj)]){\
//bsize[(bi)][(bj)]=(unsigned*)calloc(((K/B)+1),sizeof(unsigned));\
//bid[(bi)][(bj)]=(unsigned*)calloc(((K/B)+1),sizeof(unsigned));\
//bmap[(bi)][(bj)]=(unsigned**)calloc((K/B)+1,sizeof(unsigned*));\
//}\
//if(!bmap[(bi)][(bj)][(bk)]){\
//bid[(bi)][(bj)][(bk)]=(NB);\
//bmap[(bi)][(bj)][(bk)]=(unsigned*)calloc(B*B*B,sizeof(unsigned));\
//}\
//b=bid[(bi)][(bj)][(bk)];\
//bmap[(bi)][(bj)][(bk)][(bsize[(bi)][(bj)][(bk)])++]=(m);\
//}

typedef unsigned int uint;
typedef unsigned short ushort;
typedef unsigned char byte;
typedef float real;

unsigned coo_hicoo_insp(const float* val, const unsigned B, const unsigned M, const unsigned N, const unsigned* dim, const unsigned* indices, real** bval, unsigned short** bindices, unsigned** bp, unsigned char** eindices);
inline unsigned coo_hicoo_insp(const float* val, const unsigned B, const unsigned M, const unsigned N, const unsigned* dim, const unsigned* indices, real** bval, unsigned short** bindices, unsigned** bp, unsigned char** eindices) {
    unsigned t1,t2,t3,t4,t5,t6,t7,t8,t9,t10,t11,t12,t13,t14,t15;
    unsigned nnz = M;
    unsigned ord = N;
    unsigned bs = B;

    *bp = (uint*) calloc(nnz, sizeof(uint));
    *eindices = (byte*) calloc(nnz * ord, sizeof(byte));
    *bindices = (ushort*) calloc(nnz * ord, sizeof(ushort));
    *bval = (real*) calloc(nnz, sizeof(real));

    uint n, m = 0, c, i, j, k, b = 0, nb = 0, p = 0, pmax = 0, z;
    ushort bc, bi, bj, bk;
    byte r, ec, ei, ej, ek;
    ushort bcrd[ord];
    uint crd[ord];
    byte ecrd[ord];
    ushort bdim[ord];
    byte bs_bits = ilog2(bs);
    uint bsize = (bs*bs)/8;

    // Unroll...
    bdim[0] = (dim[0] >> bs_bits) + 1;
    bdim[1] = (dim[1] >> bs_bits) + 1;
    bdim[2] = (dim[2] >> bs_bits) + 1;
    uint size = bdim[0] * bdim[1] * bdim[2];// * bdim[3];
    if (ord > 3) {
        bdim[3] = (dim[3] >> bs_bits) + 1;
        size *= bdim[3];
    }
    if (ord > 4) {
        bdim[4] = (dim[4] >> bs_bits) + 1;
        size *= bdim[4];
    }

    uint* __restrict b_id = (uint*) malloc(size * sizeof(uint));
    uint* __restrict b_cnt = (uint*) calloc(size, sizeof(uint));
    uint* __restrict b_sz = (uint*) calloc(size, sizeof(uint));
    uint** __restrict b_map = (uint**) calloc(size, sizeof(uint*));
    while (size > 0 && (b_map == NULL || b_cnt == NULL || b_sz == NULL || b_id == NULL)) {
        size /= 2;
        b_id = (uint*) malloc(size * sizeof(uint));
        b_cnt = (uint*) calloc(size, sizeof(uint));
        b_map = (uint**) calloc(size, sizeof(uint*));
        b_sz = (uint*) calloc(size, sizeof(uint));
    }

    memset(b_id, UINT_MAX, size * sizeof(uint));
    nb = 0;

    //#pragma omp parallel for schedule(auto)
    #pragma omp simd
    for (n = 0; n < nnz; n++) {
        //  Unroll...
        bcrd[0] = (ind(0,n)) >> bs_bits;
        bcrd[1] = (ind(1,n)) >> bs_bits;
        bcrd[2] = (ind(2,n)) >> bs_bits;
        if (ord > 3) bcrd[3] = (ind(3,n)) >> bs_bits;
        if (ord > 4) bcrd[4] = (ind(4,n)) >> bs_bits;

        if (ord > 4) {
            p = offset5(bcrd[0],bcrd[1],bcrd[2],bcrd[3],bcrd[4],bdim[1],bdim[2],bdim[3],bdim[4]);
        } else if (ord > 3) {
            p = offset4(bcrd[0],bcrd[1],bcrd[2],bcrd[3],bdim[1],bdim[2],bdim[3]);
        } else {
            p = offset3(bcrd[0],bcrd[1],bcrd[2],bdim[1],bdim[2]);
        }

        //if(b_id[p] == USHRT_MAX) {
        if (b_id[p] == UINT_MAX) {
            b_id[p] = nb;
            b_map[nb] = (uint*) calloc(bsize,sizeof(uint));
            b_sz[nb] = bsize;
            //assert(b_map[nb] != NULL);
        }

        b = b_id[p];

        if (b_cnt[b] == b_sz[b]) {
            b_sz[b] *= 2;
            b_map[b] = (uint*) realloc(b_map[b],b_sz[b]*sizeof(uint));
//            assert(tmp != NULL);
//            b_map[b] = tmp;
        }

        b_map[b][b_cnt[b]++] = n;
        if (b >= nb) nb = b + 1;

        // Unroll...
        bind(0,b) = bcrd[0];
        bind(1,b) = bcrd[1];
        bind(2,b) = bcrd[2];
        if (ord > 3) bind(3,b) = bcrd[3];
        if (ord > 4) bind(4,b) = bcrd[4];
    }

    p = 0;
    //#pragma omp parallel for schedule(runtime)
    for (b = 0; b < nb; b++) {
        //#pragma omp parallel for schedule(runtime)
        for (m = 0; m < b_cnt[b]; m++) {
            n = b_map[b][m];

            // Unroll...
            eind(0,p) = ind(0,n) < (bind(0,b) << bs_bits) ? ind(0,n) : ind(0,n) - (bind(0,b) << bs_bits);
            eind(1,p) = ind(1,n) < (bind(1,b) << bs_bits) ? ind(1,n) : ind(1,n) - (bind(1,b) << bs_bits);
            eind(2,p) = ind(2,n) < (bind(2,b) << bs_bits) ? ind(2,n) : ind(2,n) - (bind(2,b) << bs_bits);
            if (ord > 3) eind(3,p) = ind(3,n) < (bind(3,b) << bs_bits) ? ind(3,n) : ind(3,n) - (bind(3,b) << bs_bits);
            if (ord > 4) eind(4,p) = ind(4,n) < (bind(4,b) << bs_bits) ? ind(4,n) : ind(4,n) - (bind(4,b) << bs_bits);

//            assert(bs*bind(0,b)+eind(0,p)==ind(0,n));
//            assert(bs*bind(1,b)+eind(1,p)==ind(1,n));
//            assert(bs*bind(2,b)+eind(2,p)==ind(2,n));
//            assert(bs*bind(3,b)+eind(3,p)==ind(3,n));

            if (p >= bp(b+1)) {
                bp(b+1) = p+1;
            }

            bval(p) = val(n);
            p += 1;
        }
    }

    *bindices = (ushort*) realloc(*bindices, nb * ord * sizeof(ushort));
    *bp = (uint*) realloc(*bp, (nb+1) * sizeof(uint));
    //fprintf(stderr, "size=%u,nb=%u\n", size, nb);

    for (b = 0; b < nb; b++)
        if (b_map[b])
            free(b_map[b]);
    free(b_map);
    free(b_cnt);
    free(b_id);
    free(b_sz);

    return (nb);
}    // coo_hicoo_insp

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
#undef bid
#undef bmap
#undef bp
#undef bind
#undef bsize
#undef p
#undef ind
#undef eind
