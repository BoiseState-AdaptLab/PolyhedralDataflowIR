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

inline void _gpuAssert(cudaError_t code, const char *file, int line) {
    if (code != cudaSuccess)  {
        fprintf(stderr, "gpuAssert: %s %s %d\n", cudaGetErrorString(code), file, line);
        exit(code);
    }
}

void _gpuCheck(const char* msg, const char* file, int line) {
    cudaError_t _err = cudaGetLastError();
    if (_err != cudaSuccess) {
        fprintf(stderr, "FATAL: %s (%s at %s:%d)\n", msg, cudaGetErrorString(_err), file, line);
    }
}

cuda_t *cuda_new() {
    cuda_t *cuda = (cuda_t*) calloc(1, sizeof(cuda_t));
    cuda->ndevices = 0;
    cuda->devid = 0;
    cuda->dataptr = NULL;
    cuda->datasize = 0;
    cuda->start = NULL;
    cuda->stop = NULL;
    cuda->managed = 0;
    return cuda;
}

void cuda_del(cuda_t *cuda) {
    if (cuda->start != NULL) {
        gpuAssert(cudaEventDestroy(cuda->start));
    }
    if (cuda->stop != NULL) {
        gpuAssert(cudaEventDestroy(cuda->stop));
    }
    if (cuda->dataptr != NULL) {
        gpuAssert(cudaFree(cuda->dataptr));
    }
    free(cuda);
}

int cuda_device_count(cuda_t *cuda) {
    if (cuda->ndevices < 1) {
        gpuAssert(cudaGetDeviceCount(&cuda->ndevices));
    }
    return cuda->ndevices;
}

int cuda_device_id(cuda_t *cuda) {
    return cuda->devid;
}

cudaDeviceProp cuda_device_props(cuda_t *cuda) {
    gpuAssert(cudaGetDeviceProperties(&cuda->props, cuda->devid));
    return cuda->props;
}

void cuda_print(cuda_t *cuda, FILE *file) {
    cuda_device_props(cuda);
    cudaDeviceProp *prop = &cuda->props;

    fprintf(file, "Device Number: %d\n", cuda->devid);
    fprintf(file, "  Device name: %s\n", prop->name);
    fprintf(file, "  Memory Clock Rate (KHz): %d\n", prop->memoryClockRate);
    fprintf(file, "  Memory Bus Width (bits): %d\n", prop->memoryBusWidth);
    fprintf(file, "  Peak Memory Bandwidth (GB/s): %f\n",
            2.0 * prop->memoryClockRate * (prop->memoryBusWidth / 8) / 1.0e6);
    fprintf(file, "  Compute Version: %d.%d\n", prop->major, prop->minor);
    fprintf(file, "  Compute Mode: ");

    switch (prop->computeMode) {
        case cudaComputeModeExclusive:
            fprintf(file, "Exclusive");
            break;
        case cudaComputeModeProhibited:
            fprintf(file, "Prohibited");
            break;
        default:
            fprintf(file, "Default");
            break;
    }

    fprintf(file, "\n");
    fprintf(file, "  SM count: %d\n", prop->multiProcessorCount);
    fprintf(file, "  Shared mem/block: %zd\n", prop->sharedMemPerBlock);
    fprintf(file, "  Threads per warp: %d\n", prop->warpSize);
    fprintf(file, "  Max threads per block: %d\n", prop->maxThreadsPerBlock);

    fprintf(file, "  Max block size: (");
    for (int j = 0; j < 3; j++) {
        fprintf(file, "%d,", prop->maxThreadsDim[j]);
    }

    fprintf(file, ")\n  Max grid size: (");
    for (int j = 0; j < 3; j++) {
        fprintf(file, "%d,", prop->maxGridSize[j]);
    }

    fprintf(file, ")\n");
}

int cuda_set_device(cuda_t *cuda) {
    return cudaSetDevice(cuda->devid);
}

int cuda_sync() {
    return cudaDeviceSynchronize();
}

float cuda_profile_start(cuda_t *cuda) {
    gpuAssert(cudaEventCreate(&cuda->start));
    gpuAssert(cudaEventCreate(&cuda->stop));
    gpuAssert(cudaEventRecord(cuda->start));
    return 0.0f;
}

float cuda_profile_stop(cuda_t *cuda) {
    float elapsed;
    cudaDeviceSynchronize();
    gpuAssert(cudaEventRecord(cuda->stop));
    gpuAssert(cudaEventSynchronize(cuda->stop));
    gpuAssert(cudaEventElapsedTime(&elapsed, cuda->start, cuda->stop));
    return elapsed * 1E-3f;
}

void* cuda_malloc(size_t size) {
    void *ptr;
    gpuAssert(cudaMalloc(&ptr, size));
    gpuCheck("cudaMalloc");
    return ptr;
}

void* cuda_calloc(size_t num, size_t size) {
    void *ptr = cuda_malloc(num * size);
    gpuAssert(cudaMemset(ptr, 0, num * size));
    gpuCheck("cudaMemset");
    return ptr;
}

void* cuda_alloc(cuda_t *cuda, size_t size) {
    cuda->dataptr = cuda_malloc(size);
    cuda->datasize = size;
    return cuda->dataptr;
}

void* cuda_alloc_man(cuda_t *cuda, size_t size) {
    gpuAssert(cudaMallocManaged(&cuda->dataptr, size));
    gpuCheck("cudaMallocManaged");
    cuda->datasize = size;
    cuda->managed = 1;
    return cuda->dataptr;
}

void cuda_free(void* ptr) {
    gpuAssert(cudaFree(ptr));
}

void cuda_copy_host(const void* src, void* dest, size_t size) {
    fprintf(stderr, "src=%p, dest=%p,size=%u\n", src, dest, size);
    gpuAssert(cudaMemcpy(dest, src, size, cudaMemcpyDeviceToHost));
    gpuCheck("cudaMemcpyDeviceToHost");
}

void cuda_copy_device(const void* src, void* dest, size_t size) {
    gpuAssert(cudaMemcpy(dest, src, size, cudaMemcpyHostToDevice));
    gpuCheck("cudaMemcpyHostToDevice");
}

#endif //_CUDA_FUNCS_H_
