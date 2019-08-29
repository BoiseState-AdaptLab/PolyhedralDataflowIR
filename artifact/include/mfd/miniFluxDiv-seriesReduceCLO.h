#include "MiniFluxUtil.h"

#define TILE_SIZE 8
#define TILE_MASK 7
#define TILE_SIZE2 16
#define TILE_MASK2 15

//<Data Structure>
typedef struct _MiniFluxDivData {
    int numCell;
    int numBox;
    int numComp;
    int nThreads;
    int nGhost;
    int fullNumCell;
    int fullNumCell2;
    int fullNumCell3;
    double innerTime;
    Real **g_caches;
} MiniFluxDivData;

//<Macros>

// ************************************************************************ //
// Define value set macros here (VAL_* where * is the operation).
// ************************************************************************ //

#define VAL_FLUX1X(c,z,y,x) *(GET_FACE_VAL_X(g_cache,(c),(z),(y),(x)))
#define VAL_FLUX1Y(c,z,y,x) *(GET_FACE_VAL_Y(g_cache,(c),(z),(y),(x)))
#define VAL_FLUX1Z(c,z,y,x) *(GET_FACE_VAL_Z(g_cache,(c),(z),(y),(x)))

#define VAL_FLUX2X(c,z,y,x) *(GET_FACE_VAL_X(g_cache,(c),(z),(y),(x)))
#define VAL_FLUX2Y(c,z,y,x) *(GET_FACE_VAL_Y(g_cache,(c),(z),(y),(x)))
#define VAL_FLUX2Z(c,z,y,x) *(GET_FACE_VAL_Z(g_cache,(c),(z),(y),(x)))

#define VAL_DIFFX(c,z,y,x) PHI_OUT(c,z,y,x)
#define VAL_DIFFY(c,z,y,x) PHI_OUT(c,z,y,x)
#define VAL_DIFFZ(c,z,y,x) PHI_OUT(c,z,y,x)

// ************************************************************************ //
// Define statement macros here (STM_* where * is the operation).
// ************************************************************************ //

// This is the Flux1 in the X direction
#define STM_FLUX1X(c,z,y,x) {\
    VAL_FLUX1X(c,z,y,x) = factor1*((PHI_IN(c,z,y,(x)-2)) +\
                          7*(PHI_IN(c,z,y,(x)-1) + (PHI_IN(c,z,y,x))) +\
                          (PHI_IN(c,z,y,(x)+1)));\
}

// This is the Flux1 in the Y direction
#define STM_FLUX1Y(c,z,y,x) {\
    VAL_FLUX1Y(c,z,y,x) = factor1*((PHI_IN(c,z,(y)-2,x)) +\
                          7*(PHI_IN(c,z,(y)-1,x) + (PHI_IN(c,z,y,x))) +\
                          (PHI_IN(c,z,(y)+1,x)));\
}

// This is the Flux1 in the Z direction
#define STM_FLUX1Z(c,z,y,x) {\
    VAL_FLUX1Z(c,z,y,x) = factor1*((PHI_IN(c,(z)-2,y,x)) +\
                          7*(PHI_IN(c,(z)-1,y,x) + (PHI_IN(c,z,y,x))) +\
                          (PHI_IN(c,(z)+1,y,x)));\
}

// This is the Flux2 in the X direction
#define STM_FLUX2X(c,z,y,x) {\
    VAL_FLUX2X(c,z,y,x) = VAL_FLUX1X(c,z,y,x) * factor2*(VAL_FLUX1X(2,z,y,x));\
}

// This is the Flux2 in the Y direction
#define STM_FLUX2Y(c,z,y,x) {\
    VAL_FLUX2Y(c,z,y,x) = VAL_FLUX1Y(c,z,y,x) * factor2*(VAL_FLUX1Y(3,z,y,x));\
}

// This is the Flux2 in the Z direction
#define STM_FLUX2Z(c,z,y,x) {\
    VAL_FLUX2Z(c,z,y,x) = VAL_FLUX1Z(c,z,y,x) * factor2*(VAL_FLUX1Z(4,z,y,x));\
}

// This is the Diffusion in the X direction
#define STM_DIFFX(c,z,y,x) {\
    VAL_DIFFX(c,z,y,x) += ((VAL_FLUX2X(c,z,y,(x)+1)) - (VAL_FLUX2X(c,z,y,x)));\
}

// This is the Diffusion in the Y direction
#define STM_DIFFY(c,z,y,x) {\
    VAL_DIFFY(c,z,y,x) += ((VAL_FLUX2Y(c,z,(y)+1,x)) - (VAL_FLUX2Y(c,z,y,x)));\
}

// This is the Diffusion in the Z direction
#define STM_DIFFZ(c,z,y,x) {\
    VAL_DIFFZ(c,z,y,x) += ((VAL_FLUX2Z(c,(z)+1,y,x)) - (VAL_FLUX2Z(c,z,y,x)));\
}

// ************************************************************************ //
// Temporary data allocation function (setup).
// ************************************************************************ //

inline void miniFluxDiv_alloc(MiniFluxDivData* data) {
    if (data != NULL) {
        data->innerTime = 0.0;
        //<Insert your memory allocation code here...>
        int cacheSize = sizeof(Real) * data->numCell * data->numCell * (data->numCell + 1) * data->numComp;
        data->g_caches = (Real **) malloc(sizeof(Real*) * data->nThreads);

        #pragma omp parallel for default(shared)
        for (int i = 0; i < data->nThreads; i++) {
            data->g_caches[i] = (Real *) malloc(cacheSize);
        }
    }
}   // miniFluxDiv_alloc

// ************************************************************************ //
// Benchmark kernel function where statement macros are called (run).
// ************************************************************************ //

inline void miniFluxDiv_kernel(Real **old_boxes, Real **new_boxes, MiniFluxDivData* data) {
    //<Insert your kernel code here...>
    int numBox = data->numBox;
    int numCell = data->numCell;
    int numComp = data->numComp;
    int nGhost = data->nGhost;
    int full_numCell = data->fullNumCell;
    int full_numCell2 = data->fullNumCell2;
    int full_numCell3 = data->fullNumCell3;
    Real **g_caches = data->g_caches;

    #pragma omp parallel for default(shared)
    for (int idx = 0; idx < numBox; idx++) {
        //int ix, iy, iz, ic;

        Real *old_box = old_boxes[idx];
        Real *new_box = new_boxes[idx];


        int tid = omp_get_thread_num();
        Real *g_cache = g_caches[tid];

  for (int c1 = 0; c1 <= 4; c1 += 1)
    for (int c2 = 0; c2 < numCell; c2 += 1)
      for (int c3 = 0; c3 < numCell; c3 += 1)
        for (int c4 = 0; c4 <= numCell; c4 += 1)
          STM_FLUX1X(c1, c2, c3, c4);
  for (int c0 = 1; c0 <= 3; c0 += 1) {
    if (c0 == 3) {
      for (int c2 = 0; c2 < numCell; c2 += 1)
        for (int c3 = 0; c3 < numCell; c3 += 1)
          for (int c4 = 0; c4 <= numCell; c4 += 1)
            STM_FLUX2X(2, c2, c3, c4);
    } else if (c0 == 2) {
      for (int c1 = 3; c1 <= 4; c1 += 1)
        for (int c2 = 0; c2 < numCell; c2 += 1)
          for (int c3 = 0; c3 < numCell; c3 += 1)
            for (int c4 = 0; c4 <= numCell; c4 += 1)
              STM_FLUX2X(c1, c2, c3, c4);
    } else
      for (int c1 = 0; c1 <= 1; c1 += 1)
        for (int c2 = 0; c2 < numCell; c2 += 1)
          for (int c3 = 0; c3 < numCell; c3 += 1)
            for (int c4 = 0; c4 <= numCell; c4 += 1)
              STM_FLUX2X(c1, c2, c3, c4);
  }
  for (int c1 = 0; c1 <= 4; c1 += 1)
    for (int c2 = 0; c2 < numCell; c2 += 1)
      for (int c3 = 0; c3 < numCell; c3 += 1)
        for (int c4 = 0; c4 < numCell; c4 += 1)
          STM_DIFFX(c1, c2, c3, c4);
  for (int c1 = 0; c1 <= 4; c1 += 1)
    for (int c2 = 0; c2 < numCell; c2 += 1)
      for (int c3 = 0; c3 <= numCell; c3 += 1)
        for (int c4 = 0; c4 < numCell; c4 += 1)
          STM_FLUX1Y(c1, c2, c3, c4);
  for (int c0 = 6; c0 <= 8; c0 += 1) {
    if (c0 >= 7) {
      for (int c2 = 0; c2 < numCell; c2 += 1)
        for (int c3 = 0; c3 <= numCell; c3 += 1)
          for (int c4 = 0; c4 < numCell; c4 += 1)
            STM_FLUX2Y(-c0 + 11, c2, c3, c4);
    } else
      for (int c1 = 0; c1 <= 2; c1 += 1)
        for (int c2 = 0; c2 < numCell; c2 += 1)
          for (int c3 = 0; c3 <= numCell; c3 += 1)
            for (int c4 = 0; c4 < numCell; c4 += 1)
              STM_FLUX2Y(c1, c2, c3, c4);
  }
  for (int c1 = 0; c1 <= 4; c1 += 1)
    for (int c2 = 0; c2 < numCell; c2 += 1)
      for (int c3 = 0; c3 < numCell; c3 += 1)
        for (int c4 = 0; c4 < numCell; c4 += 1)
          STM_DIFFY(c1, c2, c3, c4);
  for (int c1 = 0; c1 <= 4; c1 += 1)
    for (int c2 = 0; c2 <= numCell; c2 += 1)
      for (int c3 = 0; c3 < numCell; c3 += 1)
        for (int c4 = 0; c4 < numCell; c4 += 1)
          STM_FLUX1Z(c1, c2, c3, c4);
  for (int c1 = 0; c1 <= 4; c1 += 1)
    for (int c2 = 0; c2 <= numCell; c2 += 1)
      for (int c3 = 0; c3 < numCell; c3 += 1)
        for (int c4 = 0; c4 < numCell; c4 += 1)
          STM_FLUX2Z(c1, c2, c3, c4);
  for (int c1 = 0; c1 <= 4; c1 += 1)
    for (int c2 = 0; c2 < numCell; c2 += 1)
      for (int c3 = 0; c3 < numCell; c3 += 1)
        for (int c4 = 0; c4 < numCell; c4 += 1)
          STM_DIFFZ(c1, c2, c3, c4);
    }
}	// miniFluxdiv_kernel

// ************************************************************************ //
// Temporary data cleanup function (teardown).
// ************************************************************************ //

inline void miniFluxDiv_free(MiniFluxDivData* data) {
    if (data != NULL) {
        //<Insert your memory de-allocation code here...>
        if (data->g_caches != NULL) {
            #pragma omp parallel for default(shared)
            for (int i = 0; i < data->nThreads; i++) {
                free(data->g_caches[i]);
            }

            free(data->g_caches);
        }
    }
}    // miniFluxDiv_free
