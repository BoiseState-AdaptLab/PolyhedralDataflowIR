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

int assemble(taco_tensor_t *A2, taco_tensor_t *A0, taco_tensor_t *A1) {
  int* __restrict__ A21_pos = (int*)(A2->indices[0][0]);
  int* __restrict__ A21_crd = (int*)(A2->indices[0][1]);
  int* __restrict__ A22_crd = (int*)(A2->indices[1][1]);
  double* __restrict__ A2_vals = (double*)(A2->vals);
  int A2_vals_size = A2->vals_size;
  int* __restrict__ A02_pos = (int*)(A0->indices[1][0]);
  int* __restrict__ A02_crd = (int*)(A0->indices[1][1]);

  A21_pos = (int32_t*)malloc(sizeof(int32_t) * 2);
  A21_pos[0] = 0;
  int32_t A21_crd_size = 1048576;
  A21_crd = (int32_t*)malloc(sizeof(int32_t) * A21_crd_size);
  int32_t A22_crd_size = 1048576;
  A22_crd = (int32_t*)malloc(sizeof(int32_t) * A22_crd_size);

  int32_t pA21 = 0;
  int32_t pA22 = 0;
  for (int32_t i3A0 = 0; i3A0 < 4; i3A0++) {
    for (int32_t pA02 = A02_pos[i3A0]; pA02 < A02_pos[(i3A0 + 1)]; pA02++) {
      int32_t i4A0 = A02_crd[pA02];
      if (A22_crd_size <= pA22) {
        int32_t A22_crd_new_size = TACO_MAX(A22_crd_size * 2, (pA22 + 1));
        A22_crd = (int32_t*)realloc(A22_crd, sizeof(int32_t) * A22_crd_new_size);
        A22_crd_size = A22_crd_new_size;
      }
      A22_crd[pA22] = i4A0;
      pA22++;
      if (A21_crd_size <= pA21) {
        A21_crd = (int32_t*)realloc(A21_crd, sizeof(int32_t) * A21_crd_size * 2);
        A21_crd_size *= 2;
      }
      A21_crd[pA21] = i3A0;
      pA21++;
      if (A21_pos_size <= pA21) {
        A21_pos = (int32_t*)realloc(A21_pos, sizeof(int32_t) * A21_pos_size * 2);
        A21_pos_size *= 2;
      }
    }
  }
  A21_pos[1] = pA21;

  A2_vals = (double*)malloc(sizeof(double) * pA22);
  A2_vals_size = pA22;

  A2->indices[0][0] = (uint8_t*)(A21_pos);
  A2->indices[0][1] = (uint8_t*)(A21_crd);
  A2->indices[1][1] = (uint8_t*)(A22_crd);
  A2->vals = (uint8_t*)A2_vals;
  A2->vals_size = A2_vals_size;
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
#include "/tmp/taco_tmp_WDmZkN/qyix7mt8kaz9.h"
int _shim_assemble(void** parameterPack) {
  return assemble((taco_tensor_t*)(parameterPack[0]), (taco_tensor_t*)(parameterPack[1]), (taco_tensor_t*)(parameterPack[2]));
}
int _shim_compute(void** parameterPack) {
  return compute((taco_tensor_t*)(parameterPack[0]), (taco_tensor_t*)(parameterPack[1]), (taco_tensor_t*)(parameterPack[2]));
}
