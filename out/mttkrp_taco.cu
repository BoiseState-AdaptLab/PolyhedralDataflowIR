// Generated by the Tensor Algebra Compiler (tensor-compiler.org)

__global__
void computeDeviceKernel0(taco_tensor_t *A, taco_tensor_t *B, taco_tensor_t *C, taco_tensor_t *D){
  int A2_dimension = (int)(A->dimensions[1]);
  double* __restrict__ A_vals = (double*)(A->vals);
  int* __restrict__ B1_pos = (int*)(B->indices[0][0]);
  int* __restrict__ B1_crd = (int*)(B->indices[0][1]);
  int* __restrict__ B2_pos = (int*)(B->indices[1][0]);
  int* __restrict__ B2_crd = (int*)(B->indices[1][1]);
  int* __restrict__ B3_pos = (int*)(B->indices[2][0]);
  int* __restrict__ B3_crd = (int*)(B->indices[2][1]);
  double* __restrict__ B_vals = (double*)(B->vals);
  int C2_dimension = (int)(C->dimensions[1]);
  double* __restrict__ C_vals = (double*)(C->vals);
  int D2_dimension = (int)(D->dimensions[1]);
  double* __restrict__ D_vals = (double*)(D->vals);

  int32_t pB1 = blockIdx.x * blockDim.x + threadIdx.x + B1_pos[0];
  if (pB1 >= B1_pos[1]) {
    return;
  }

  int32_t iB = B1_crd[pB1];
  for (int32_t pB2 = B2_pos[pB1]; pB2 < B2_pos[(pB1 + 1)]; pB2++) {
    int32_t jB = B2_crd[pB2];
    for (int32_t pB3 = B3_pos[pB2]; pB3 < B3_pos[(pB2 + 1)]; pB3++) {
      int32_t kB = B3_crd[pB3];
      double tk = B_vals[pB3];
      for (int32_t rC = 0; rC < C2_dimension; rC++) {
        int32_t pC2 = jB * C2_dimension + rC;
        int32_t pD2 = kB * D2_dimension + rC;
        int32_t pA2 = iB * A2_dimension + rC;
        A_vals[pA2] = A_vals[pA2] + tk * C_vals[pC2] * D_vals[pD2];
      }
    }
  }
}

int compute(taco_tensor_t *A, taco_tensor_t *B, taco_tensor_t *C, taco_tensor_t *D) {
  int A1_dimension = (int)(A->dimensions[0]);
  int A2_dimension = (int)(A->dimensions[1]);
  double* __restrict__ A_vals = (double*)(A->vals);
  int* __restrict__ B1_pos = (int*)(B->indices[0][0]);

  #pragma omp parallel for
  for (int32_t pA = 0; pA < (A1_dimension * A2_dimension); pA++) {
    A_vals[pA] = 0.0;
  }
  computeDeviceKernel0<<<(B1_pos[1] - B1_pos[0] + 255) / 256, 256>>>(A, B, C, D);
  cudaDeviceSynchronize();
  return 0;
}

