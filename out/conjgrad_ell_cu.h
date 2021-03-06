// 'conjgrad_ell' code generated by 'edavis' at 09/30/2019 09:53:52
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <math.h>
#include <string.h>
#include <cuda_funcs.h>

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
#define xmin (blockIdx.x*blockDim.x+threadIdx.x)
#define ymin (blockIdx.y*blockDim.y+threadIdx.y)
#define zmin (blockIdx.z*blockDim.z+threadIdx.z)
#define xinc (blockDim.x*gridDim.x)
#define yinc (blockDim.y*gridDim.y)
#define zinc (blockDim.z*gridDim.z)
#define A(k,i) A[offset2((k),(i),N)]
#define lcol(t,k,i) lcol[offset2((k),(i),N)]

typedef struct {
    double* __restrict__ r;
    double* __restrict__ d;
    double* __restrict__ s;
} conjgrad_ell_params;

__global__ void conjgrad_ell_(const double* A, const double* b, const unsigned K, const unsigned N, const unsigned T, const unsigned* lcol, double* x, double* retval, conjgrad_ell_params* params) {
    unsigned t1,t2,t3,t4,t5,t6,t7,t8,t9,t10,t11,t12,t13,t14,t15;
    double ds = 0.0;
    double rs = 0.0;
    double rs0 = 0.0;
    double alpha;
    double beta;

    double* __restrict__ r = params->r;
    double* __restrict__ d = params->d;
    double* __restrict__ s = params->s;

// copy+dinit+sinit+spmv+ddot+rdot0+adiv+xadd+rsub+rdot+bdiv+dadd
//#undef s0
//#define s0(i) r[(i)]=d[(i)]=b[(i)]
//#undef s1
//#define s1(t) ds=rs0=rs=0.000000
#undef s2
#define s2(t,i) s[(i)]=0.000000
#undef s3
#define s3(t,k,i,j) s[(i)]+=A((k),(i))*d[(j)]
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

//#pragma omp parallel for schedule(auto) private(t2,t4,t6,t8)
//for(t2 = 0; t2 <= N-1; t2++) {
//  s0(t2);
//}
//for(t2 = 0; t2 <= T-1; t2++) {
//  s1(t2);
  #pragma omp parallel for schedule(auto) private(t2,t4,t6,t8)
  for(t4 = 0; t4 <= N-1; t4++) {
    s2(t2,t4);
    #pragma omp simd
    for(t6 = 0; t6 <= K-1; t6++) {
      t8 = lcol(t2,t6,t4);
      s3(t2,t6,t4,t8);
    }
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
//}

*retval = rs;
}    // conjgrad_ell

double conjgrad_ell(const double* A, const double* b, const unsigned K, const unsigned N, const unsigned T, const unsigned* lcol, double* x);
inline double conjgrad_ell(const double* A, const double* b, const unsigned K, const unsigned N, const unsigned T, const unsigned* lcol, double* x) {
    double rs;
    double* __restrict__ d_A = (double*) cuda_malloc(K*N * sizeof(double));
    double* __restrict__ d_b = (double*) cuda_malloc(N * sizeof(double));
    unsigned* __restrict__ d_lcol = (unsigned*) cuda_malloc(K*N * sizeof(unsigned));
    double* __restrict__ d_x = (double*) cuda_malloc(N * sizeof(double));
    double* __restrict__ d_rs = (double*) cuda_malloc(sizeof(double));

    conjgrad_ell_params* params = (conjgrad_ell_params*) malloc(sizeof(conjgrad_ell_params));
    conjgrad_ell_params* d_params = (conjgrad_ell_params*) cuda_malloc(sizeof(conjgrad_ell_params));
    params->r = (double*) cuda_malloc((N), sizeof(double));
    params->d = (double*) cuda_malloc((N), sizeof(double));
    params->s = (double*) cuda_calloc((N), sizeof(double));

    // Copy host to device
    cuda_copy_device(params, d_params, sizeof(conjgrad_ell_params));
    cuda_copy_device(A, d_A, K*N * sizeof(double));
    cuda_copy_device(b, d_b, N * sizeof(double));
    cuda_copy_device(lcol, d_lcol, K*N * sizeof(unsigned));
    cuda_copy_device(x, d_x, N * sizeof(double));
    cuda_copy_device(b, params->r, N * sizeof(double));
    cuda_copy_device(b, params->d, N * sizeof(double));

    for(unsigned t = 0; t <= T-1; t++) {
        printf("launching kernel: conjgrad_ell (t=%d)\n", t);
        conjgrad_ell_<<<bsize, gsize>>>(d_A, d_b, K, N, T, d_lcol, d_x, d_rs, d_params);
        cuda_sync();
        gpuCheck("conjgrad_ell");
    }

    // Copy device to host
    cuda_copy_host(d_x, x, N * sizeof(double));
    cuda_copy_host(d_rs, &rs, sizeof(double));

    cuda_free(d_A);
    cuda_free(d_b);
    cuda_free(d_lcol);
    cuda_free(d_x);
    cuda_free(d_rs);

    cuda_free(params->r);
    cuda_free(params->d);
    cuda_free(params->s);
    cuda_free(d_params);
    free(params);

    return (rs);
}

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
#undef A
#undef lcol

