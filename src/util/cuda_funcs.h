//
// Created by edavis on 12/16/16.
//

#ifndef _CUDA_FUNCS_H_
#define _CUDA_FUNCS_H_

#include <stdlib.h>
#include <stdio.h>
#include <cuda_runtime.h>

/**
 * gpuAssert
 *
 * CUDA error handler.
 *
 * @param code cudaError_t error code struct.
 * @param file Name of file in which error occurred.
 * @param line Line number on which error occurred.
 */
#define gpuAssert(ans) { _gpuAssert((ans), __FILE__, __LINE__); }
inline void _gpuAssert(cudaError_t code, const char* file, int line);

#define gpuCheck(msg) { _gpuCheck((msg), __FILE__, __LINE__); }
inline void _gpuCheck(const char* msg, const char* file, int line);

typedef struct {
    int ndevices;
    int devid;
    struct cudaDeviceProp props;
    cudaEvent_t start;
    cudaEvent_t stop;
    void *dataptr;
    size_t datasize;
    int managed;
} cuda_t;

cuda_t* cuda_new();
void cuda_del(cuda_t* cuda);
int cuda_device_count(cuda_t* cuda);
int cuda_device_id(cuda_t* cuda);
struct cudaDeviceProp cuda_device_props(cuda_t *cuda);
void cuda_print(cuda_t *cuda, FILE *file);
int cuda_set_device(cuda_t *cuda);
int cuda_sync();
float cuda_profile_start(cuda_t *cuda);
float cuda_profile_stop(cuda_t *cuda);
void* cuda_alloc(cuda_t *cuda, size_t size);
void* cuda_alloc_man(cuda_t *cuda, void** ptr, size_t size);
void* cuda_malloc(void** ptr, size_t size);
void cuda_free(void* ptr);
void cuda_copy_host(const void* src, void* dest, size_t size);
void cuda_copy_device(const void* src, void* dest, size_t size);

#endif //_CUDA_FUNCS_H_
