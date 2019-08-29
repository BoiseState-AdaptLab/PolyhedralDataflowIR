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
// locations[tid][cacheNumber] should provide a Real*
    int numberOfCaches;
    int* sizeOfCaches;
    Real ***locations;
    double innerTime;
} MiniFluxDivData;

//<Macros>

// ************************************************************************ //
// Define value set macros here (VAL_* where * is the operation).
// ************************************************************************ //
#define GET_OTHER_X(b,c,z,y,x) (b)+\
        ((x)&1)
#define GET_OTHER_Y(b,c,z,y,x) (b)+\
        ((y)&1)*(numCell)+(x)
#define GET_OTHER_Z(b,c,z,y,x) (b)+\
        ((z)&1)*(numCell)*(numCell)+(y)*(numCell)+(x)

#define GET_VEL_X(b,c,z,y,x) (b)+\
        (z)*(numCell)*(numCell+1)+(y)*(numCell+1)+(x)

#define GET_VEL_Y(b,c,z,y,x) (b)+\
        (z)*(numCell+1)*(numCell)+(y)*(numCell)+(x)

#define GET_VEL_Z(b,c,z,y,x) (b)+\
        (z)*(numCell)*(numCell)+(y)*(numCell)+(x)

#define VAL_FLUX1X(c,z,y,x) *(GET_OTHER_X(cache[0],(c),(z),(y),(x)))
#define VAL_FLUX1Y(c,z,y,x) *(GET_OTHER_Y(cache[1],(c),(z),(y),(x)))
#define VAL_FLUX1Z(c,z,y,x) *(GET_OTHER_Z(cache[2],(c),(z),(y),(x)))
#define VAL_FLUX1Xv(c,z,y,x) *(GET_VEL_X(cache[3],(c),(z),(y),(x)))
#define VAL_FLUX1Yv(c,z,y,x) *(GET_VEL_Y(cache[4],(c),(z),(y),(x)))
#define VAL_FLUX1Zv(c,z,y,x) *(GET_VEL_Z(cache[5],(c),(z),(y),(x)))

#define VAL_FLUX2X(c,z,y,x) *(GET_OTHER_X(cache[0],(c),(z),(y),(x)))
#define VAL_FLUX2Y(c,z,y,x) *(GET_OTHER_Y(cache[1],(c),(z),(y),(x)))
#define VAL_FLUX2Z(c,z,y,x) *(GET_OTHER_Z(cache[2],(c),(z),(y),(x)))

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
#define STM_FLUX1Xv(c,z,y,x) {\
    VAL_FLUX1Xv(c,z,y,x) = factor1*((PHI_IN(c,z,y,(x)-2)) +\
                          7*(PHI_IN(c,z,y,(x)-1) + (PHI_IN(c,z,y,x))) +\
                          (PHI_IN(c,z,y,(x)+1)));\
}

// This is the Flux1 in the Y direction
#define STM_FLUX1Y(c,z,y,x) {\
    VAL_FLUX1Y(c,z,y,x) = factor1*((PHI_IN(c,z,(y)-2,x)) +\
                          7*(PHI_IN(c,z,(y)-1,x) + (PHI_IN(c,z,y,x))) +\
                          (PHI_IN(c,z,(y)+1,x)));\
}
#define STM_FLUX1Yv(c,z,y,x) {\
    VAL_FLUX1Yv(c,z,y,x) = factor1*((PHI_IN(c,z,(y)-2,x)) +\
                          7*(PHI_IN(c,z,(y)-1,x) + (PHI_IN(c,z,y,x))) +\
                          (PHI_IN(c,z,(y)+1,x)));\
}

// This is the Flux1 in the Z direction
#define STM_FLUX1Zv(c,z,y,x) {\
    VAL_FLUX1Zv(c,z,y,x) = factor1*((PHI_IN(c,(z)-2,y,x)) +\
                          7*(PHI_IN(c,(z)-1,y,x) + (PHI_IN(c,z,y,x))) +\
                          (PHI_IN(c,(z)+1,y,x)));\
}
#define STM_FLUX1Z(c,z,y,x) {\
    VAL_FLUX1Z(c,z,y,x) = factor1*((PHI_IN(c,(z)-2,y,x)) +\
                          7*(PHI_IN(c,(z)-1,y,x) + (PHI_IN(c,z,y,x))) +\
                          (PHI_IN(c,(z)+1,y,x)));\
}

// This is the Flux2 in the X direction
#define STM_FLUX2X(c,z,y,x) {\
    VAL_FLUX2X(c,z,y,x) = VAL_FLUX1X(c,z,y,x) * factor2*(VAL_FLUX1Xv(2,z,y,x));\
}
#define STM_FLUX2Xv(c,z,y,x) {\
    VAL_FLUX2X(c,z,y,x) = VAL_FLUX1Xv(c,z,y,x)* factor2*(VAL_FLUX1Xv(2,z,y,x));\
}

// This is the Flux2 in the Y direction
#define STM_FLUX2Y(c,z,y,x) {\
    VAL_FLUX2Y(c,z,y,x) = VAL_FLUX1Y(c,z,y,x) * factor2*(VAL_FLUX1Yv(3,z,y,x));\
}
#define STM_FLUX2Yv(c,z,y,x) {\
    VAL_FLUX2Y(c,z,y,x) = VAL_FLUX1Yv(c,z,y,x)* factor2*(VAL_FLUX1Yv(3,z,y,x));\
}

// This is the Flux2 in the Z direction
#define STM_FLUX2Z(c,z,y,x) {\
    VAL_FLUX2Z(c,z,y,x) = VAL_FLUX1Z(c,z,y,x)* factor2*(VAL_FLUX1Zv(4,z,y,x));\
}
#define STM_FLUX2Zv(c,z,y,x) {\
    VAL_FLUX2Z(c,z,y,x) = VAL_FLUX1Zv(c,z,y,x) * factor2*(VAL_FLUX1Zv(4,z,y,x));\
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
        int cacheSize = sizeof(Real) * data->numCell * data->numCell * (data->numCell + 1) * data->numComp;
        //<Insert your memory allocation code here...>
        data->innerTime = 0.0;
        data->numberOfCaches = 6;
        data->sizeOfCaches = (int*)malloc(sizeof(int)*data->numberOfCaches);
        data->sizeOfCaches[0] = sizeof(Real)*2;
        data->sizeOfCaches[1] = sizeof(Real)*2*data->numCell;
        data->sizeOfCaches[2] = sizeof(Real)*2*data->numCell*data->numCell;
        data->sizeOfCaches[3] = sizeof(Real) * data->numCell * data->numCell * (data->numCell + 1);
        data->sizeOfCaches[4] = sizeof(Real) * data->numCell * data->numCell * (data->numCell + 1);
        data->sizeOfCaches[5] = sizeof(Real) * data->numCell * data->numCell * (data->numCell + 1);

        // allocation a set of locations per thread
        data->locations = (Real ***) malloc(sizeof(Real**) * data->nThreads);

        #pragma omp parallel for default(shared)
        for (int i = 0; i < data->nThreads; i++) {
            data->locations[i] = (Real **) malloc(sizeof(Real*)*data->numberOfCaches);
            for(int j = 0; j< data->numberOfCaches ; j++){
              data->locations[i][j] = (Real*) malloc(data->sizeOfCaches[j]);
              if(data->locations[i][j] == NULL){
                exit(0);
              }
            }
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

    #pragma omp parallel for default(shared)
    for (int idx = 0; idx < numBox; idx++) {
        //int ix, iy, iz, ic;

        Real *old_box = old_boxes[idx];
        Real *new_box = new_boxes[idx];


        int tid = omp_get_thread_num();
        Real **cache = data->locations[tid];

  for (int c2 = 0; c2 < numCell; c2 += 1)
    for (int c3 = 0; c3 < numCell; c3 += 1)
      for (int c4 = 0; c4 <= numCell; c4 += 1)
        STM_FLUX1Xv(2, c2, c3, c4);
  for (int c2 = 0; c2 < numCell; c2 += 1)
    for (int c3 = 0; c3 <= numCell; c3 += 1)
      for (int c4 = 0; c4 < numCell; c4 += 1)
        STM_FLUX1Yv(3, c2, c3, c4);
  for (int c2 = 0; c2 <= numCell; c2 += 1)
    for (int c3 = 0; c3 < numCell; c3 += 1)
      for (int c4 = 0; c4 < numCell; c4 += 1)
        STM_FLUX1Zv(4, c2, c3, c4);
  for (int c1 = 0; c1 <= 4; c1 += 1)
    for (int c2 = 0; c2 < numCell; c2 += 1)
      for (int c3 = 0; c3 < numCell; c3 += 1)
        for (int c4 = 0; c4 <= numCell; c4 += 1) {
          if (c1 >= 3) {
            STM_FLUX1X(c1, c2, c3, c4);
            STM_FLUX2X(c1, c2, c3, c4);
          } else if (c1 <= 1) {
            STM_FLUX1X(c1, c2, c3, c4);
            STM_FLUX2X(c1, c2, c3, c4);
          } else
            STM_FLUX2Xv(2, c2, c3, c4);
          if (c4 >= 1)
            STM_DIFFX(c1, c2, c3, c4 - 1);
        }
  for (int c1 = 0; c1 <= 4; c1 += 1)
    for (int c2 = 0; c2 < numCell; c2 += 1)
      for (int c3 = 0; c3 <= numCell; c3 += 1)
        for (int c4 = 0; c4 < numCell; c4 += 1) {
          if (c1 <= 2) {
            STM_FLUX1Y(c1, c2, c3, c4);
            STM_FLUX2Y(c1, c2, c3, c4);
          } else if (c1 == 4) {
            STM_FLUX1Y(4, c2, c3, c4);
            STM_FLUX2Y(4, c2, c3, c4);
          } else
            STM_FLUX2Yv(3, c2, c3, c4);
          if (c3 >= 1)
            STM_DIFFY(c1, c2, c3 - 1, c4);
        }
  for (int c1 = 0; c1 <= 4; c1 += 1)
    for (int c2 = 0; c2 <= numCell; c2 += 1)
      for (int c3 = 0; c3 < numCell; c3 += 1)
        for (int c4 = 0; c4 < numCell; c4 += 1) {
          if (c1 <= 3) {
            STM_FLUX1Z(c1, c2, c3, c4);
            STM_FLUX2Z(c1, c2, c3, c4);
          } else
            STM_FLUX2Zv(4, c2, c3, c4);
          if (c2 >= 1)
            STM_DIFFZ(c1, c2 - 1, c3, c4);
        }
    }
}	// miniFluxdiv_kernel

// ************************************************************************ //
// Temporary data cleanup function (teardown).
// ************************************************************************ //

inline void miniFluxDiv_free(MiniFluxDivData* data) {
    //<Insert your memory de-allocation code here...>
    /*if (data != NULL && data->locations != NULL) {
        #pragma omp parallel for default(shared)
        for (int i = 0; i < data->nThreads; i++) {
        }

    }*/
}    // miniFluxDiv_free
