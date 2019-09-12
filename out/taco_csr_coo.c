#ifndef TACO_C_HEADERS
#define TACO_C_HEADERS
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <math.h>
#include <complex.h>
#define TACO_MIN(_a,_b) ((_a) < (_b) ? (_a) : (_b))
#define TACO_MAX(_a,_b) ((_a) > (_b) ? (_a) : (_b))
#define TACO_DEREF(_a) (((___context___*)(*__ctx__))->_a)
#ifndef TACO_TENSOR_T_DEFINED
#define TACO_TENSOR_T_DEFINED
typedef enum { taco_mode_dense, taco_mode_sparse } taco_mode_t;
typedef struct {
  int32_t      order;         // tensor order (number of modes)
  int32_t*     dimensions;    // tensor dimensions
  int32_t      csize;         // component size
  int32_t*     mode_ordering; // mode storage ordering
  taco_mode_t* mode_types;    // mode storage types
  uint8_t***   indices;       // tensor index data (per mode)
  uint8_t*     vals;          // tensor values
  int32_t      vals_size;     // values array size
} taco_tensor_t;
#endif
#endif

#define N 4

int assemble(taco_tensor_t *C, taco_tensor_t *A, taco_tensor_t *B) {
  int* __restrict__ C2_pos = (int*)(C->indices[1][0]);
  int* __restrict__ C2_crd = (int*)(C->indices[1][1]);
  double* __restrict__ C_vals = (double*)(C->vals);
  int C_vals_size = C->vals_size;
  int* __restrict__ A2_pos = (int*)(A->indices[1][0]);
  int* __restrict__ A2_crd = (int*)(A->indices[1][1]);

  C2_pos = (int32_t*)malloc(sizeof(int32_t) * N+1);
  C2_pos[0] = 0;
  for (int32_t pC2 = 1; pC2 < N+1; pC2++) {
    C2_pos[pC2] = 0;
  }
  int32_t C2_crd_size = 1048576;
  C2_crd = (int32_t*)malloc(sizeof(int32_t) * C2_crd_size);

  int32_t pC20 = 0;
  for (int32_t i = 0; i < N; i++) {
    int32_t C2_begin = pC20;
    for (int32_t pA2 = A2_pos[i]; pA2 < A2_pos[(i + 1)]; pA2++) {
      int32_t i4A = A2_crd[pA2];
      if (C2_crd_size <= pC20) {
        C2_crd = (int32_t*)realloc(C2_crd, sizeof(int32_t) * C2_crd_size * 2);
        C2_crd_size *= 2;
      }
      C2_crd[pC20] = i4A;
      pC20++;
    }
    C2_pos[i + 1] = pC20 - C2_begin;
  }

  int32_t csC2 = 0;
  for (int32_t pC21 = 1; pC21 < 5; pC21++) {
    csC2 += C2_pos[pC21];
    C2_pos[pC21] = csC2;
  }
  C_vals = (double*)malloc(sizeof(double) * pC20);
  C_vals_size = pC20;

  C->indices[1][0] = (uint8_t*)(C2_pos);
  C->indices[1][1] = (uint8_t*)(C2_crd);
  C->vals = (uint8_t*)C_vals;
  C->vals_size = C_vals_size;
  return 0;
}

int compute(taco_tensor_t *C, taco_tensor_t *A, taco_tensor_t *B) {
  double* __restrict__ C_vals = (double*)(C->vals);
  int* __restrict__ pos = (int*)(A->indices[1][0]);
  int* __restrict__ crd = (int*)(A->indices[1][1]);
  double* __restrict__ A_vals = (double*)(A->vals);
  double* __restrict__ B_vals = (double*)(B->vals);

  int32_t p = 0;
  for (int32_t i = 0; i < N; i++) {
    for (int32_t n = pos[i]; n < pos[(i+1)]; n++) {
      int32_t j = crd[n];
      int32_t m = i*N+j;
      C_vals[p] = A_vals[n] * B_vals[m];
      p++;
    }
  }
  return 0;
}
#include "taco_csr_coo.h"
int _shim_assemble(void** parameterPack) {
  return assemble((taco_tensor_t*)(parameterPack[0]), (taco_tensor_t*)(parameterPack[1]), (taco_tensor_t*)(parameterPack[2]));
}
int _shim_compute(void** parameterPack) {
  return compute((taco_tensor_t*)(parameterPack[0]), (taco_tensor_t*)(parameterPack[1]), (taco_tensor_t*)(parameterPack[2]));
}
