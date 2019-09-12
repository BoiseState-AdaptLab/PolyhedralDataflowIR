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

int assemble(taco_tensor_t *C, taco_tensor_t *A, taco_tensor_t *B) {
  int* __restrict__ C1_pos = (int*)(C->indices[0][0]);
  int* __restrict__ C1_crd = (int*)(C->indices[0][1]);
  int* __restrict__ C2_crd = (int*)(C->indices[1][1]);
  double* __restrict__ C_vals = (double*)(C->vals);
  int C_vals_size = C->vals_size;
  int* __restrict__ A2_pos = (int*)(A->indices[1][0]);
  int* __restrict__ A2_crd = (int*)(A->indices[1][1]);

  C1_pos = (int32_t*)malloc(sizeof(int32_t) * 2);
  C1_pos[0] = 0;
  int32_t C1_crd_size = 1048576;
  C1_crd = (int32_t*)malloc(sizeof(int32_t) * C1_crd_size);
  int32_t C2_crd_size = 1048576;
  C2_crd = (int32_t*)malloc(sizeof(int32_t) * C2_crd_size);

  int32_t pC1 = 0;
  int32_t pC2 = 0;
  for (int32_t i = 0; i < N; i++) {
    for (int32_t pA2 = A2_pos[i]; pA2 < A2_pos[(i + 1)]; pA2++) {
      int32_t j = A2_crd[pA2];
      if (C2_crd_size <= pC2) {
        int32_t C2_crd_new_size = TACO_MAX(C2_crd_size * 2, (pC2 + 1));
        C2_crd = (int32_t*)realloc(C2_crd, sizeof(int32_t) * C2_crd_new_size);
        C2_crd_size = C2_crd_new_size;
      }
      C2_crd[pC2] = j;
      pC2++;
      if (C1_crd_size <= pC1) {
        C1_crd = (int32_t*)realloc(C1_crd, sizeof(int32_t) * C1_crd_size * 2);
        C1_crd_size *= 2;
      }
      C1_crd[pC1] = i;
      pC1++;
      if (C1_pos_size <= pC1) {
        C1_pos = (int32_t*)realloc(C1_pos, sizeof(int32_t) * C1_pos_size * 2);
        C1_pos_size *= 2;
      }
    }
  }
  C1_pos[1] = pC1;

  C_vals = (double*)malloc(sizeof(double) * pC2);
  C_vals_size = pC2;

  C->indices[0][0] = (uint8_t*)(C1_pos);
  C->indices[0][1] = (uint8_t*)(C1_crd);
  C->indices[1][1] = (uint8_t*)(C2_crd);
  C->vals = (uint8_t*)C_vals;
  C->vals_size = C_vals_size;
  return 0;
}

int compute(taco_tensor_t *A2, taco_tensor_t *A0, taco_tensor_t *A1) {
  int* __restrict__ A21_pos = (int*)(A2->indices[0][0]);
  double* __restrict__ A2_vals = (double*)(A2->vals);
  int* __restrict__ A02_pos = (int*)(A0->indices[1][0]);
  int* __restrict__ A02_crd = (int*)(A0->indices[1][1]);
  double* __restrict__ A0_vals = (double*)(A0->vals);
  double* __restrict__ A1_vals = (double*)(A1->vals);

  int32_t pA22 = 0;
  for (int32_t i3A0 = 0; i3A0 < 4; i3A0++) {
    for (int32_t pA02 = A02_pos[i3A0]; pA02 < A02_pos[(i3A0 + 1)]; pA02++) {
      int32_t i4A0 = A02_crd[pA02];
      int32_t pA12 = i3A0 * 4 + i4A0;
      A2_vals[pA22] = A0_vals[pA02] * A1_vals[pA12];
      pA22++;
      pA21++;
      if (A21_pos_size <= pA21) {
        A21_pos = (int32_t*)realloc(A21_pos, sizeof(int32_t) * A21_pos_size * 2);
        A21_pos_size *= 2;
      }
    }
  }

  A2->indices[0][0] = (uint8_t*)(A21_pos);
  A2->vals = (uint8_t*)A2_vals;
  return 0;
}
#include "taco_csr_coo.h"
int _shim_assemble(void** parameterPack) {
  return assemble((taco_tensor_t*)(parameterPack[0]), (taco_tensor_t*)(parameterPack[1]), (taco_tensor_t*)(parameterPack[2]));
}
int _shim_compute(void** parameterPack) {
  return compute((taco_tensor_t*)(parameterPack[0]), (taco_tensor_t*)(parameterPack[1]), (taco_tensor_t*)(parameterPack[2]));
}
