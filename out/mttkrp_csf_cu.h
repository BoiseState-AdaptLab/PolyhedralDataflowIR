// 'mttkrp_csf' code generated by 'edavis' at 09/04/2019 15:43:50
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <math.h>
#include <string.h>
#include <cuda_funcs.h>

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
#define xmin (blockIdx.x*blockDim.x+threadIdx.x)
#define ymin (blockIdx.y*blockDim.y+threadIdx.y)
#define zmin (blockIdx.z*blockDim.z+threadIdx.z)
#define xinc (blockDim.x*gridDim.x)
#define yinc (blockDim.y*gridDim.y)
#define zinc (blockDim.z*gridDim.z)
#define A(i,r) A[offset2((i),(r),(R))]
#define B(j,r) B[offset2((j),(r),(R))]
#define C(k,r) C[offset2((k),(r),(R))]
#define X(n) X[(n)]
#define ind0(p) ind0[(p)]
#define ind1(p,i,q) ind1[(q)]
#define ind2(p,i,q,j,n) ind2[(n)]
#define pos0(p) pos0[(p)]
#define pos0_1(p) pos0[(p+1)]
#define pos1(p,i,q) pos1[(q)]
#define pos1_1(p,i,q) pos1[(q+1)]

__global__ void mttkrp_csf_(const float* B, const float* C, const float* X, const unsigned F, const unsigned R, const unsigned* ind0, const unsigned* ind1, const unsigned* ind2, const unsigned* pos0, const unsigned* pos1, float* A) {
    unsigned t1,t2,t3,t4,t5,t6,t7,t8,t9,t10,t11,t12,t13,t14,t15;
// krp
#undef s0
#define s0(p,i,q,j,n,k,r) A((i),(r))+=X((n))*C((k),(r))*B((j),(r))

  t2 = xmin + 0;            // xmin + lb(t2)
  if (t2 >= F) {    // t2 >= ub(t2)
    return;
  }

//for(t2 = 0; t2 <= F-1; t2++) {
  t4=ind0(t2);
  for(t6 = pos0(t2); t6 <= pos0_1(t2)-1; t6++) {
    t8=ind1(t2,t4,t6);
    for(t10 = pos1(t2,t4,t6); t10 <= pos1_1(t2,t4,t6)-1; t10++) {
      t12=ind2(t2,t4,t6,t8,t10);
      for(t14 = 0; t14 <= R-1; t14++) {
        s0(t2,t4,t6,t8,t10,t12,t14);
      }
    }
  }
//}

}    // mttkrp_csf (device)

void mttkrp_csf(const float* B, const float* C, const float* X, const unsigned F, const unsigned R, const unsigned* ind0, const unsigned* ind1, const unsigned* ind2, const unsigned* pos0, const unsigned* pos1, float* A);
inline void mttkrp_csf(const float* B, const float* C, const float* X, const unsigned F, const unsigned R, const unsigned* ind0, const unsigned* ind1, const unsigned* ind2, const unsigned* pos0, const unsigned* pos1, float* A) {
  const float* __restrict__ d_B = (float*) cuda_malloc(J*R * sizeof(float));
  const float* __restrict__ d_C = (float*) cuda_malloc(K*R * sizeof(float));
  const float* __restrict__ d_X = (float*) cuda_malloc(M * sizeof(float));
  const unsigned* __restrict__ d_ind0 = (unsigned*) cuda_malloc(M * sizeof(unsigned));  // ind0_size?
  const unsigned* __restrict__ d_ind1 = (unsigned*) cuda_malloc(M * sizeof(unsigned));  // ind1_size?
  const unsigned* __restrict__ d_ind2 = (unsigned*) cuda_malloc(M * sizeof(unsigned));  // ind2_size?
  const unsigned* __restrict__ d_pos0 = (unsigned*) cuda_malloc(M * sizeof(unsigned));  // pos0_size?
  const unsigned* __restrict__ d_pos1 = (unsigned*) cuda_malloc(M * sizeof(unsigned));  // pos1_size?
  float* __restrict__ d_A = (float*) cuda_calloc(I*R, sizeof(float));

  dim3 bdim((F - 0 + 255) / 256); // outerloop upper - lower + 255
  dim3 gdim(256);

  // Copy host to device
  cuda_copy_device(B, d_B, J*R * sizeof(float));
  cuda_copy_device(C, d_C, K*R * sizeof(float));
  cuda_copy_device(X, d_X, M * sizeof(float));
  cuda_copy_device(ind0, d_ind0, M * sizeof(unsigned));   // ind0_size?
  cuda_copy_device(ind1, d_ind1, M * sizeof(unsigned));   // ind1_size?
  cuda_copy_device(ind2, d_ind2, M * sizeof(unsigned));   // ind2_size?
  cuda_copy_device(pos0, d_pos0, M * sizeof(unsigned));   // pos0_size?
  cuda_copy_device(pos1, d_pos1, M * sizeof(unsigned));   // pos1_size?

  mttkrp_csf_<<<bdim, gdim>>>(d_B, d_C, d_X, F, R, d_ind0, d_ind1, d_ind2, d_pos0, d_pos1, d_A);
  cuda_sync();
  cuda_check("mttkrp_csf");

  // Copy device to host
  cuda_copy_host(d_A, A, I*R * sizeof(float));

  cuda_free(d_A);
  cuda_free(d_pos1);
  cuda_free(d_pos0);
  cuda_free(d_ind2);
  cuda_free(d_ind1);
  cuda_free(d_ind0);
  cuda_free(d_X);
  cuda_free(d_C);
  cuda_free(d_B);
}    // mttkrp_csf (host)

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
#undef tid
#undef A
#undef B
#undef C
#undef X
#undef ind0
#undef ind1
#undef ind2
#undef pos0
#undef pos0_1
#undef pos1
#undef pos1_1
