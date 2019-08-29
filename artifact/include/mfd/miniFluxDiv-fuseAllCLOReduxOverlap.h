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
    double innerTime;
    Real ***locations;
} MiniFluxDivData;

//<Macros>

// ************************************************************************ //
// Define value set macros here (VAL_* where * is the operation).
// ************************************************************************ //
#define GET_OTHER_X(b,c,z,y,x) (b)+((x)&1)
#define GET_OTHER_Y(b,c,z,y,x) (b)+((y)&1)*(TILE_SIZE)+(x&TILE_MASK)
//((y)&1)*(numCell)+(x)
#define GET_OTHER_Z(b,c,z,y,x) (b)+((z)&1)*(TILE_SIZE)*(TILE_SIZE)+(y&TILE_MASK)*(TILE_SIZE)+(x&TILE_MASK)
//((z)&1)*(numCell)*(numCell)+(y)*(numCell)+(x)

#define GET_VEL_X(b,c,z,y,x) (b)+\
        ((z)&TILE_MASK)*(TILE_SIZE)*(TILE_SIZE2)+((y)&TILE_MASK)*(TILE_SIZE2)+((x)&TILE_MASK2)
//((z)*(numCell)*(numCell+1))+((y)*(numCell+1))+(x)

#define GET_VEL_Y(b,c,z,y,x) (b)+\
        ((z)&TILE_MASK)*(TILE_SIZE)*(TILE_SIZE2)+((y)&TILE_MASK2)*(TILE_SIZE)+((x)&TILE_MASK)
//((z)*(numCell)*(numCell+1))+((y)*(numCell))+(x)


#define GET_VEL_Z(b,c,z,y,x) (b)+\
        ((z)&TILE_MASK2)*(TILE_SIZE)*(TILE_SIZE)+((y)&(TILE_MASK))*(TILE_SIZE)+((x)&TILE_MASK)
//((z)*(numCell)*(numCell))+((y)*(numCell))+(x)


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
    /*fprintf(stderr, "STM_FLUX1X(%d,%d,%d,%d)\n", c,z,y,x);*/\
}
#define STM_FLUX1Xv(c,z,y,x) {\
    VAL_FLUX1Xv(c,z,y,x) = factor1*((PHI_IN(c,z,y,(x)-2)) +\
                          7*(PHI_IN(c,z,y,(x)-1) + (PHI_IN(c,z,y,x))) +\
                          (PHI_IN(c,z,y,(x)+1)));\
    /*fprintf(stderr, "STM_FLUX1Xv(%d,%d,%d,%d)\n", c,z,y,x);*/\
}

// This is the Flux1 in the Y direction
#define STM_FLUX1Y(c,z,y,x) {\
    VAL_FLUX1Y(c,z,y,x) = factor1*((PHI_IN(c,z,(y)-2,x)) +\
                          7*(PHI_IN(c,z,(y)-1,x) + (PHI_IN(c,z,y,x))) +\
                          (PHI_IN(c,z,(y)+1,x)));\
    /*fprintf(stderr, "STM_FLUX1Y(%d,%d,%d,%d)\n", c,z,y,x);*/\
}
#define STM_FLUX1Yv(c,z,y,x) {\
    VAL_FLUX1Yv(c,z,y,x) = factor1*((PHI_IN(c,z,(y)-2,x)) +\
                          7*(PHI_IN(c,z,(y)-1,x) + (PHI_IN(c,z,y,x))) +\
                          (PHI_IN(c,z,(y)+1,x)));\
    /*fprintf(stderr, "STM_FLUX1Yv(%d,%d,%d,%d)\n", c,z,y,x);*/\
}

// This is the Flux1 in the Z direction
#define STM_FLUX1Z(c,z,y,x) {\
    VAL_FLUX1Z(c,z,y,x) = factor1*((PHI_IN(c,(z)-2,y,x)) +\
                          7*(PHI_IN(c,(z)-1,y,x) + (PHI_IN(c,z,y,x))) +\
                          (PHI_IN(c,(z)+1,y,x)));\
    /*fprintf(stderr, "STM_FLUX1Z(%d,%d,%d,%d)\n", c,z,y,x);*/\
}
#define STM_FLUX1Zv(c,z,y,x) {\
    VAL_FLUX1Zv(c,z,y,x) = factor1*((PHI_IN(c,(z)-2,y,x)) +\
                          7*(PHI_IN(c,(z)-1,y,x) + (PHI_IN(c,z,y,x))) +\
                          (PHI_IN(c,(z)+1,y,x)));\
    /*fprintf(stderr, "STM_FLUX1Zv(%d,%d,%d,%d)\n", c,z,y,x);*/\
}

// This is the Flux2 in the X direction
#define STM_FLUX2X(c,z,y,x) {\
    VAL_FLUX2X(c,z,y,x) = VAL_FLUX1X(c,z,y,x) * factor2*(VAL_FLUX1Xv(2,z,y,x));\
    /*fprintf(stderr, "STM_FLUX2X(%d,%d,%d,%d)\n", c,z,y,x);*/\
}
#define STM_FLUX2Xv(c,z,y,x) {\
    VAL_FLUX2X(c,z,y,x) = VAL_FLUX1Xv(c,z,y,x)* factor2*(VAL_FLUX1Xv(2,z,y,x));\
    /*fprintf(stderr, "STM_FLUX2Xv(%d,%d,%d,%d)\n", c,z,y,x);*/\
}

// This is the Flux2 in the Y direction
#define STM_FLUX2Y(c,z,y,x) {\
    VAL_FLUX2Y(c,z,y,x) = VAL_FLUX1Y(c,z,y,x) * factor2*(VAL_FLUX1Yv(3,z,y,x));\
    /*fprintf(stderr, "STM_FLUX2Y(%d,%d,%d,%d)\n", c,z,y,x);*/\
}
#define STM_FLUX2Yv(c,z,y,x) {\
    VAL_FLUX2Y(c,z,y,x) = VAL_FLUX1Yv(c,z,y,x)* factor2*(VAL_FLUX1Yv(3,z,y,x));\
    /*fprintf(stderr, "STM_FLUX2Yv(%d,%d,%d,%d)\n", c,z,y,x);*/\
}

// This is the Flux2 in the Z direction
#define STM_FLUX2Z(c,z,y,x) {\
    VAL_FLUX2Z(c,z,y,x) = VAL_FLUX1Z(c,z,y,x)* factor2*(VAL_FLUX1Zv(4,z,y,x));\
    /*fprintf(stderr, "STM_FLUX2Z(%d,%d,%d,%d)\n", c,z,y,x);*/\
}
#define STM_FLUX2Zv(c,z,y,x) {\
    VAL_FLUX2Z(c,z,y,x) = VAL_FLUX1Zv(c,z,y,x) * factor2*(VAL_FLUX1Zv(4,z,y,x));\
    /*fprintf(stderr, "STM_FLUX2Zv(%d,%d,%d,%d)\n", c,z,y,x);*/\
}

// This is the Diffusion in the X direction
#define STM_DIFFX(c,z,y,x) {\
    VAL_DIFFX(c,z,y,x) += ((VAL_FLUX2X(c,z,y,(x)+1)) - (VAL_FLUX2X(c,z,y,x)));\
    /*fprintf(stderr, "STM_DIFFX(%d,%d,%d,%d)\n", c,z,y,x);*/\
}

// This is the Diffusion in the Y direction
#define STM_DIFFY(c,z,y,x) {\
    VAL_DIFFY(c,z,y,x) += ((VAL_FLUX2Y(c,z,(y)+1,x)) - (VAL_FLUX2Y(c,z,y,x)));\
    /*fprintf(stderr, "STM_DIFFY(%d,%d,%d,%d)\n", c,z,y,x);*/\
}

// This is the Diffusion in the Z direction
#define STM_DIFFZ(c,z,y,x) {\
    VAL_DIFFZ(c,z,y,x) += ((VAL_FLUX2Z(c,(z)+1,y,x)) - (VAL_FLUX2Z(c,z,y,x)));\
    /*fprintf(stderr, "STM_DIFFZ(%d,%d,%d,%d)\n", c,z,y,x);*/\
}

// ************************************************************************ //
// Temporary data allocation function (setup).
// ************************************************************************ //

inline void miniFluxDiv_alloc(MiniFluxDivData* data) {
    //fprintf(stderr, "Enter miniFluxDiv_alloc\n");

    if (data != NULL) {
        //int nCells = data->numCell;
        int nCells = TILE_SIZE;
        data->innerTime = 0.0;
        //<Insert your memory allocation code here...>
        data->numberOfCaches = 6;
        data->sizeOfCaches = (int*)malloc(sizeof(int)*data->numberOfCaches);
        data->sizeOfCaches[0] = sizeof(Real)*2;
        data->sizeOfCaches[1] = sizeof(Real) * 2 * nCells;
        data->sizeOfCaches[2] = sizeof(Real) * 2 * nCells * nCells;
        data->sizeOfCaches[3] = sizeof(Real) * nCells * nCells * (nCells*2);
        data->sizeOfCaches[4] = sizeof(Real) * nCells * nCells * (nCells*2);
        data->sizeOfCaches[5] = sizeof(Real) * nCells * nCells * (nCells*2);

        //fprintf(stderr, "data->numberOfCaches = %d\n", data->numberOfCaches);
        //fprintf(stderr, "data->sizeOfCaches = %p\n", data->sizeOfCaches);

        // allocation a set of locations per thread
        data->locations = (Real ***) malloc(sizeof(Real**) * data->nThreads);

        //fprintf(stderr, "data->locations = %p\n", data->locations);

#pragma omp parallel for default(shared)
        for (int i = 0; i < data->nThreads; i++) {
            data->locations[i] = (Real **) malloc(sizeof(Real*)*data->numberOfCaches);
            //fprintf(stderr, "data->locations[%d] = %p\n", i, data->locations[i]);
            for(int j = 0; j< data->numberOfCaches ; j++){
                //fprintf(stderr, "data->sizeOfCaches[%d] = %d\n", j, data->sizeOfCaches[j]);
                data->locations[i][j] = (Real*) malloc(data->sizeOfCaches[j]);
                //fprintf(stderr, "data->locations[%d][%d] = %p\n", i, j, data->locations[i][j]);
                if(data->locations[i][j] == NULL){
                    exit(0);
                }
            }
        }
    }

    //fprintf(stderr, "Exit miniFluxDiv_alloc\n");
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
    int nThreads = data->nThreads;
    int full_numCell = data->fullNumCell;
    int full_numCell2 = data->fullNumCell2;
    int full_numCell3 = data->fullNumCell3;

    //omp_set_nested(1);
    //omp_set_dynamic(1);

    #pragma omp parallel for default(shared)
    for (int idx = 0; idx < numBox; idx++) {
        Real *old_box = old_boxes[idx];
        Real *new_box = new_boxes[idx];

        int tid = omp_get_thread_num();
        Real **cache = data->locations[tid];
for (int c0 = 0; c0 <= numCell / 8; c0 += 1)
    for (int c1 = 0; c1 <= numCell / 8; c1 += 1)
      for (int c2 = 0; c2 <= numCell / 8; c2 += 1) {
        for (int c5 = max(1, 8 * c0); c5 <= min(numCell, 8 * c0 + 7); c5 += 1)
          for (int c6 = max(1, 8 * c1); c6 <= min(numCell, 8 * c1 + 7); c6 += 1)
            for (int c7 = max(0, 8 * c2 - 1); c7 <= min(numCell, 8 * c2 + 7); c7 += 1)
              STM_FLUX1Xv(2, c5 - 1, c6 - 1, c7);
        for (int c5 = max(1, 8 * c0); c5 <= min(numCell, 8 * c0 + 7); c5 += 1)
          for (int c6 = max(0, 8 * c1 - 1); c6 <= min(numCell, 8 * c1 + 7); c6 += 1)
            for (int c7 = max(1, 8 * c2); c7 <= min(numCell, 8 * c2 + 7); c7 += 1)
              STM_FLUX1Yv(3, c5 - 1, c6, c7 - 1);
        for (int c5 = max(0, 8 * c0 - 1); c5 <= min(numCell, 8 * c0 + 7); c5 += 1)
          for (int c6 = max(1, 8 * c1); c6 <= min(numCell, 8 * c1 + 7); c6 += 1)
            for (int c7 = max(1, 8 * c2); c7 <= min(numCell, 8 * c2 + 7); c7 += 1)
              STM_FLUX1Zv(4, c5, c6 - 1, c7 - 1);
        for (int c4 = 0; c4 <= 1; c4 += 1) {
          if (c0 == 0) {
            for (int c6 = max(1, 8 * c1); c6 <= min(numCell, 8 * c1 + 7); c6 += 1)
              for (int c7 = max(1, 8 * c2); c7 <= min(numCell, 8 * c2 + 7); c7 += 1) {
                STM_FLUX1Z(c4, 0, c6 - 1, c7 - 1);
                STM_FLUX2Z(c4, 0, c6 - 1, c7 - 1);
              }
          } else
            for (int c6 = max(1, 8 * c1); c6 <= min(numCell, 8 * c1 + 7); c6 += 1)
              for (int c7 = max(1, 8 * c2); c7 <= min(numCell, 8 * c2 + 7); c7 += 1) {
                STM_FLUX1Z(c4, 8 * c0 - 1, c6 - 1, c7 - 1);
                STM_FLUX2Z(c4, 8 * c0 - 1, c6 - 1, c7 - 1);
              }
          for (int c5 = max(1, 8 * c0); c5 <= min(numCell, 8 * c0 + 7); c5 += 1) {
            if (c1 == 0) {
              for (int c7 = max(1, 8 * c2); c7 <= min(numCell, 8 * c2 + 7); c7 += 1) {
                STM_FLUX1Y(c4, c5 - 1, 0, c7 - 1);
                STM_FLUX2Y(c4, c5 - 1, 0, c7 - 1);
              }
            } else
              for (int c7 = max(1, 8 * c2); c7 <= min(numCell, 8 * c2 + 7); c7 += 1) {
                STM_FLUX1Y(c4, c5 - 1, 8 * c1 - 1, c7 - 1);
                STM_FLUX2Y(c4, c5 - 1, 8 * c1 - 1, c7 - 1);
              }
            for (int c6 = max(1, 8 * c1); c6 <= min(numCell, 8 * c1 + 7); c6 += 1) {
              if (c2 == 0) {
                STM_FLUX1X(c4, c5 - 1, c6 - 1, 0);
                STM_FLUX2X(c4, c5 - 1, c6 - 1, 0);
              } else {
                STM_FLUX1X(c4, c5 - 1, c6 - 1, 8 * c2 - 1);
                STM_FLUX2X(c4, c5 - 1, c6 - 1, 8 * c2 - 1);
              }
              for (int c7 = max(1, 8 * c2); c7 <= min(numCell, 8 * c2 + 7); c7 += 1) {
                STM_FLUX1Y(c4, c5 - 1, c6, c7 - 1);
                STM_FLUX1X(c4, c5 - 1, c6 - 1, c7);
                STM_FLUX1Z(c4, c5, c6 - 1, c7 - 1);
                STM_FLUX2Z(c4, c5, c6 - 1, c7 - 1);
                STM_FLUX2Y(c4, c5 - 1, c6, c7 - 1);
                STM_FLUX2X(c4, c5 - 1, c6 - 1, c7);
                STM_DIFFZ(c4, c5 - 1, c6 - 1, c7 - 1);
                STM_DIFFX(c4, c5 - 1, c6 - 1, c7 - 1);
                STM_DIFFY(c4, c5 - 1, c6 - 1, c7 - 1);
              }
            }
          }
        }
        if (c0 == 0) {
          for (int c6 = max(1, 8 * c1); c6 <= min(numCell, 8 * c1 + 7); c6 += 1)
            for (int c7 = max(1, 8 * c2); c7 <= min(numCell, 8 * c2 + 7); c7 += 1) {
              STM_FLUX1Z(2, 0, c6 - 1, c7 - 1);
              STM_FLUX2Z(2, 0, c6 - 1, c7 - 1);
            }
        } else
          for (int c6 = max(1, 8 * c1); c6 <= min(numCell, 8 * c1 + 7); c6 += 1)
            for (int c7 = max(1, 8 * c2); c7 <= min(numCell, 8 * c2 + 7); c7 += 1) {
              STM_FLUX1Z(2, 8 * c0 - 1, c6 - 1, c7 - 1);
              STM_FLUX2Z(2, 8 * c0 - 1, c6 - 1, c7 - 1);
            }
        for (int c5 = max(1, 8 * c0); c5 <= min(numCell, 8 * c0 + 7); c5 += 1) {
          if (c1 == 0) {
            for (int c7 = max(1, 8 * c2); c7 <= min(numCell, 8 * c2 + 7); c7 += 1) {
              STM_FLUX1Y(2, c5 - 1, 0, c7 - 1);
              STM_FLUX2Y(2, c5 - 1, 0, c7 - 1);
            }
          } else
            for (int c7 = max(1, 8 * c2); c7 <= min(numCell, 8 * c2 + 7); c7 += 1) {
              STM_FLUX1Y(2, c5 - 1, 8 * c1 - 1, c7 - 1);
              STM_FLUX2Y(2, c5 - 1, 8 * c1 - 1, c7 - 1);
            }
          for (int c6 = max(1, 8 * c1); c6 <= min(numCell, 8 * c1 + 7); c6 += 1) {
            if (c2 == 0) {
              STM_FLUX2Xv(2, c5 - 1, c6 - 1, 0);
            } else
              STM_FLUX2Xv(2, c5 - 1, c6 - 1, 8 * c2 - 1);
            for (int c7 = max(1, 8 * c2); c7 <= min(numCell, 8 * c2 + 7); c7 += 1) {
              STM_FLUX1Y(2, c5 - 1, c6, c7 - 1);
              STM_FLUX1Z(2, c5, c6 - 1, c7 - 1);
              STM_FLUX2Z(2, c5, c6 - 1, c7 - 1);
              STM_FLUX2Y(2, c5 - 1, c6, c7 - 1);
              STM_FLUX2Xv(2, c5 - 1, c6 - 1, c7);
              STM_DIFFZ(2, c5 - 1, c6 - 1, c7 - 1);
              STM_DIFFX(2, c5 - 1, c6 - 1, c7 - 1);
              STM_DIFFY(2, c5 - 1, c6 - 1, c7 - 1);
            }
          }
        }
        if (c0 == 0) {
          for (int c6 = max(1, 8 * c1); c6 <= min(numCell, 8 * c1 + 7); c6 += 1)
            for (int c7 = max(1, 8 * c2); c7 <= min(numCell, 8 * c2 + 7); c7 += 1) {
              STM_FLUX1Z(3, 0, c6 - 1, c7 - 1);
              STM_FLUX2Z(3, 0, c6 - 1, c7 - 1);
            }
        } else
          for (int c6 = max(1, 8 * c1); c6 <= min(numCell, 8 * c1 + 7); c6 += 1)
            for (int c7 = max(1, 8 * c2); c7 <= min(numCell, 8 * c2 + 7); c7 += 1) {
              STM_FLUX1Z(3, 8 * c0 - 1, c6 - 1, c7 - 1);
              STM_FLUX2Z(3, 8 * c0 - 1, c6 - 1, c7 - 1);
            }
        for (int c5 = max(1, 8 * c0); c5 <= min(numCell, 8 * c0 + 7); c5 += 1) {
          if (c1 == 0) {
            for (int c7 = max(1, 8 * c2); c7 <= min(numCell, 8 * c2 + 7); c7 += 1)
              STM_FLUX2Yv(3, c5 - 1, 0, c7 - 1);
          } else
            for (int c7 = max(1, 8 * c2); c7 <= min(numCell, 8 * c2 + 7); c7 += 1)
              STM_FLUX2Yv(3, c5 - 1, 8 * c1 - 1, c7 - 1);
          for (int c6 = max(1, 8 * c1); c6 <= min(numCell, 8 * c1 + 7); c6 += 1) {
            if (c2 == 0) {
              STM_FLUX1X(3, c5 - 1, c6 - 1, 0);
              STM_FLUX2X(3, c5 - 1, c6 - 1, 0);
            } else {
              STM_FLUX1X(3, c5 - 1, c6 - 1, 8 * c2 - 1);
              STM_FLUX2X(3, c5 - 1, c6 - 1, 8 * c2 - 1);
            }
            for (int c7 = max(1, 8 * c2); c7 <= min(numCell, 8 * c2 + 7); c7 += 1) {
              STM_FLUX1X(3, c5 - 1, c6 - 1, c7);
              STM_FLUX1Z(3, c5, c6 - 1, c7 - 1);
              STM_FLUX2Z(3, c5, c6 - 1, c7 - 1);
              STM_FLUX2X(3, c5 - 1, c6 - 1, c7);
              STM_FLUX2Yv(3, c5 - 1, c6, c7 - 1);
              STM_DIFFZ(3, c5 - 1, c6 - 1, c7 - 1);
              STM_DIFFX(3, c5 - 1, c6 - 1, c7 - 1);
              STM_DIFFY(3, c5 - 1, c6 - 1, c7 - 1);
            }
          }
        }
        if (c0 == 0) {
          for (int c6 = max(1, 8 * c1); c6 <= min(numCell, 8 * c1 + 7); c6 += 1)
            for (int c7 = max(1, 8 * c2); c7 <= min(numCell, 8 * c2 + 7); c7 += 1)
              STM_FLUX2Zv(4, 0, c6 - 1, c7 - 1);
        } else
          for (int c6 = max(1, 8 * c1); c6 <= min(numCell, 8 * c1 + 7); c6 += 1)
            for (int c7 = max(1, 8 * c2); c7 <= min(numCell, 8 * c2 + 7); c7 += 1)
              STM_FLUX2Zv(4, 8 * c0 - 1, c6 - 1, c7 - 1);
        for (int c5 = max(1, 8 * c0); c5 <= min(numCell, 8 * c0 + 7); c5 += 1) {
          if (c1 == 0) {
            for (int c7 = max(1, 8 * c2); c7 <= min(numCell, 8 * c2 + 7); c7 += 1) {
              STM_FLUX1Y(4, c5 - 1, 0, c7 - 1);
              STM_FLUX2Y(4, c5 - 1, 0, c7 - 1);
            }
          } else
            for (int c7 = max(1, 8 * c2); c7 <= min(numCell, 8 * c2 + 7); c7 += 1) {
              STM_FLUX1Y(4, c5 - 1, 8 * c1 - 1, c7 - 1);
              STM_FLUX2Y(4, c5 - 1, 8 * c1 - 1, c7 - 1);
            }
          for (int c6 = max(1, 8 * c1); c6 <= min(numCell, 8 * c1 + 7); c6 += 1) {
            if (c2 == 0) {
              STM_FLUX1X(4, c5 - 1, c6 - 1, 0);
              STM_FLUX2X(4, c5 - 1, c6 - 1, 0);
            } else {
              STM_FLUX1X(4, c5 - 1, c6 - 1, 8 * c2 - 1);
              STM_FLUX2X(4, c5 - 1, c6 - 1, 8 * c2 - 1);
            }
            for (int c7 = max(1, 8 * c2); c7 <= min(numCell, 8 * c2 + 7); c7 += 1) {
              STM_FLUX1Y(4, c5 - 1, c6, c7 - 1);
              STM_FLUX1X(4, c5 - 1, c6 - 1, c7);
              STM_FLUX2Zv(4, c5, c6 - 1, c7 - 1);
              STM_FLUX2Y(4, c5 - 1, c6, c7 - 1);
              STM_FLUX2X(4, c5 - 1, c6 - 1, c7);
              STM_DIFFZ(4, c5 - 1, c6 - 1, c7 - 1);
              STM_DIFFX(4, c5 - 1, c6 - 1, c7 - 1);
              STM_DIFFY(4, c5 - 1, c6 - 1, c7 - 1);
            }
          }
        }
      }

    }

    //fprintf(stderr, "Exit miniFluxDiv_kernel\n");
}	// miniFluxdiv_kernel

// ************************************************************************ //
// Temporary data cleanup function (teardown).
// ************************************************************************ //

inline void miniFluxDiv_free(MiniFluxDivData* data) {
    //fprintf(stderr, "Enter miniFluxDiv_free\n");

    //<Insert your memory de-allocation code here...>
    if (data != NULL && data->locations != NULL) {
        //fprintf(stderr, "data->numberOfCaches = %d\n", data->numberOfCaches);
        //fprintf(stderr, "data->sizeOfCaches = %p\n", data->sizeOfCaches);
        //fprintf(stderr, "data->locations = %p\n", data->locations);

        #pragma omp parallel for default(shared)
        for (int i = 0; i < data->nThreads; i++) {
            //fprintf(stderr, "data->locations[%d] = %p\n", i, data->locations[i]);
            for(int j = 0; j< data->numberOfCaches ; j++){
                //fprintf(stderr, "data->sizeOfCaches[%d] = %d\n", j, data->sizeOfCaches[j]);
                if(data->locations[i][j] != NULL){
                    //fprintf(stderr, "data->locations[%d][%d] = %p\n", i, j, data->locations[i][j]);
                    free(data->locations[i][j]);
                }
            }

            free(data->locations[i]);
        }

        free(data->locations);
        free(data->sizeOfCaches);
    }

    //fprintf(stderr, "Exit miniFluxDiv_free\n");
}    // miniFluxDiv_free
