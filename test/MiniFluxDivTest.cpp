#include <cstring>
#include <string>
using std::string;
using std::to_string;

#include <gtest/gtest.h>
using namespace testing;

#include <BenchmarkTest.hpp>
#include <util/Lists.hpp>

#define dx 0.5
#define factor1 (1.0/12.0)
#define factor2 2.0

#define NBOXES 32
#define NCELLS 128
#define NTHREADS 1
#define NDIMS 3
#define NCOMP 5
#define NGHOST 2

#undef GET_VAL_PTR
#define GET_VAL_PTR(b,c,z,y,x) (b)+(c)*_fullNumCell3+((z)+NGHOST)*_fullNumCell2+\
                               ((y)+NGHOST)*_fullNumCell+((x)+NGHOST)

#define PHI_IN(c,z,y,x) *(GET_VAL_PTR(old_box,(c),(z),(y),(x)))
#define PHI_OUT(c,z,y,x) *(GET_VAL_PTR(new_box,(c),(z),(y),(x)))
#define PHI_PTR(c,z,y,x) (GET_VAL_PTR(new_box,(c),(z),(y),(x)))
#define PHI_REF(c,z,y,x) *(GET_VAL_PTR(ref_box,(c),(z),(y),(x)))

#define p_data(box,z,y,x) *(GET_VAL_PTR((box),0,z,y,x))
#define e_data(box,z,y,x) *(GET_VAL_PTR((box),1,z,y,x))
#define u_data(box,z,y,x) *(GET_VAL_PTR((box),2,z,y,x))
#define v_data(box,z,y,x) *(GET_VAL_PTR((box),3,z,y,x))
#define w_data(box,z,y,x) *(GET_VAL_PTR((box),4,z,y,x))

// Import generated code
#if NDIMS>2
#include "mfd_3d.h"
#else
#include "mfd_2d.h"
#endif

typedef double Real;

namespace test {
    class MiniFluxDivTest : public BenchmarkTest {
    protected:
        MiniFluxDivTest() {
            _name = "MiniFluxDivTest";
        }

        virtual void SetUp(initializer_list<string> args) {
            _numBox = 1; //NBOXES;
            _numCell = NCELLS;
            _nThreads = NTHREADS;

            vector<string> argv(args.begin(), args.end());
            unsigned argc = argv.size();

            for(int iarg = 0; iarg < argc-1; iarg++) {
                if(strcmp(argv[iarg].c_str(),"-c") == 0) {
                    _numCell = atoi(argv[iarg+1].c_str());
                } else if(strcmp(argv[iarg].c_str(), "-b") == 0) {
                    _numBox = atoi(argv[iarg+1].c_str());
                } else if(strcmp(argv[iarg].c_str(), "-p") == 0) {
                    _nThreads = atoi(argv[iarg+1].c_str());
                }
            }

            _fullNumCell = _numCell + 2 * NGHOST;
            _fullNumCell2 = _fullNumCell * _fullNumCell;
            _fullNumCell3 = _fullNumCell2 * _fullNumCell;

           // TODO: Init arrays...
            _old_boxes = (Real**) malloc(sizeof(Real*) * _numBox);
            _new_boxes = (Real**) malloc(sizeof(Real*) * _numBox);
            _ref_boxes = (Real**) malloc(sizeof(Real*) * _numBox);

            int idx, iz, iy, ix;
            unsigned boxSize = _fullNumCell3 * NCOMP;

            for(idx=0;idx<_numBox;idx++){
                _old_boxes[idx] = (Real*) malloc(sizeof(Real)*boxSize);
                _new_boxes[idx] = (Real*) malloc(sizeof(Real)*boxSize);
                _ref_boxes[idx] = (Real*) malloc(sizeof(Real)*boxSize);
            }

            for(idx=0;idx < _numBox;idx++){
                Real* old_box = _old_boxes[idx];
                Real* new_box = _new_boxes[idx];
                Real* ref_box = _ref_boxes[idx];

                for(iz=-NGHOST;iz<(_fullNumCell-NGHOST);iz++){
                    for(iy=-NGHOST;iy<(_fullNumCell-NGHOST);iy++){
                        for(ix=-NGHOST;ix<(_fullNumCell-NGHOST);ix++){
                            p_data(new_box,iz,iy,ix) = dx*(iz+iy+ix);
                            e_data(new_box,iz,iy,ix) = 1.+dx*(iz+iy+ix);
                            u_data(new_box,iz,iy,ix) = 2.+dx*(iz+iy+ix);
                            v_data(new_box,iz,iy,ix) = 3.+dx*(iz+iy+ix);
                            w_data(new_box,iz,iy,ix) = 4.+dx*(iz+iy+ix);

                            p_data(old_box,iz,iy,ix) = dx*(iz+iy+ix);
                            e_data(old_box,iz,iy,ix) = 1.+dx*(iz+iy+ix);
                            u_data(old_box,iz,iy,ix) = 2.+dx*(iz+iy+ix);
                            v_data(old_box,iz,iy,ix) = 3.+dx*(iz+iy+ix);
                            w_data(old_box,iz,iy,ix) = 4.+dx*(iz+iy+ix);

                            p_data(ref_box,iz,iy,ix) = dx*(iz+iy+ix);
                            e_data(ref_box,iz,iy,ix) = 1.+dx*(iz+iy+ix);
                            u_data(ref_box,iz,iy,ix) = 2.+dx*(iz+iy+ix);
                            v_data(ref_box,iz,iy,ix) = 3.+dx*(iz+iy+ix);
                            w_data(ref_box,iz,iy,ix) = 4.+dx*(iz+iy+ix);
                        }
                    }
                }
            }
            int stop = 1;
        }

        virtual void Execute() {
            //#pragma omp parallel for default(shared) schedule(auto)
            for (unsigned b = 0; b < _numBox; b++) {
                mfd(_old_boxes[b], _numCell, _new_boxes[b]);
            }
        }

        // Execute reference code for verification
        virtual void Evaluate() {
            int idx;
            int ic,iz,iy,ix;
            int numCell= _numCell;
            int numBox= _numBox;
            int nGhost = NGHOST;
            int numComp = NCOMP;
            int full_numCell= numCell+2*nGhost;
            int full_numCell2= full_numCell*full_numCell;
            int full_numCell3= full_numCell*full_numCell*full_numCell;
            int flux_total_Size= numCell*numCell*(numCell+1);

            // loop bounds
            //int phi_comp_mult = ((numCell+2*nGhost)*(numCell+2*nGhost)*(numCell+2*nGhost));
            //const int flux_comp_mult = ((numCell)*(numCell)*(numCell+1));
            const int phi_pencil_size = (numCell+2*nGhost);

            #undef GET_VAL_PTR
            #define GET_VAL_PTR(b,c,z,y,x) (b)+ (c)*full_numCell3 + ((z)+nGhost) * full_numCell2 +\
                    ((y)+nGhost)*full_numCell+((x)+nGhost)

            #undef GET_FACE_VAL_PTR
            #define GET_FACE_VAL_PTR(d,b,c,z,y,x) (b)+\
                                     (c)*(numCell+((d)==2))*(numCell+((d)==1))*(numCell+((d)==0)) +\
                                     (z)*(numCell+((d)==1))*(numCell+((d)==0))+\
                                     (y)*(numCell+((d)==0))+(x)

            // process each of the boxes one at a time
            for(idx=0;idx < numBox;idx++){
                Real* old_box = _old_boxes[idx];
                Real* new_box = _ref_boxes[idx];
                int f_xu,f_yu,f_zu;
                int flux_pencil_x;
                int flux_pencil_y;
                int iDir,ic,iz,iy,ix;
                int phiOffset1,phiOffset2,fluxOffset1;

                // the flux cache
                Real* fluxCache = (Real*)malloc(sizeof(Real)*numCell*numCell*(numCell+1)*numComp);
                // Allocate the space for the velocity cache
                // This is only a single component

                Real* velCache = (Real*)malloc(sizeof(Real)*numCell*numCell*(numCell+1));

                // compute the fluxes on the faces in each direction
                for(iDir=0;iDir<NDIMS;iDir++){
                    // x-direction
                    //std::string axis = "";
                    if(iDir == 0){
                        //axis = "X";
                        f_zu = numCell;
                        f_yu = numCell;
                        f_xu = numCell+1;
                        flux_pencil_x = numCell+1;
                        flux_pencil_y = numCell;
                        phiOffset1 = 1;
                        phiOffset2 = 2;
                        fluxOffset1 = 1;
                    }else if(iDir == 1) {
                        //axis = "Y";
                        f_zu = numCell;
                        f_yu = numCell+1;
                        f_xu = numCell;
                        flux_pencil_x = numCell;
                        flux_pencil_y = numCell+1;
                        phiOffset1 = phi_pencil_size;
                        phiOffset2 = phi_pencil_size*2;
                        fluxOffset1 = numCell;
                    }else if(iDir == 2) {
                        //axis = "Z";
                        f_zu = numCell+1;
                        f_yu = numCell;
                        f_xu = numCell;
                        flux_pencil_x = numCell;
                        flux_pencil_y = numCell;
                        phiOffset1 = phi_pencil_size*phi_pencil_size;
                        phiOffset2 = phi_pencil_size*phi_pencil_size*2;
                        fluxOffset1 = numCell*numCell;
                    }
                    // the upper bounds are determined by direction info above
                    // Iterate over faces and calculate g()
                    for(iz=0;iz<f_zu;iz++){
                        for(iy=0;iy<f_yu;iy++){
                            for(ic=0;ic<numComp;ic++){
                                Real* phip = GET_VAL_PTR(old_box,ic,iz,iy,0);
                                Real* fluxp = GET_FACE_VAL_PTR(iDir,fluxCache,ic,iz,iy,0);
                                for (ix=0;ix<f_xu;ix++) {
                                    *fluxp = factor1*((*(phip - phiOffset2)) +7*((*(phip - phiOffset1)) + (*(phip))) +(*(phip + phiOffset1)));
                                    ++phip;
                                    ++fluxp;
                                }
                            }
                        }
                    }

                    // cache the velocity component for the next half of the calculation
                    memcpy(velCache,(fluxCache+(iDir+2)*((numCell+1)*numCell*numCell)),sizeof(Real)*numCell*numCell*(numCell+1));

                    for(iz=0;iz<f_zu;iz++){
                        for(iy=0;iy<f_yu;iy++){
                            for(ic=0;ic<numComp;ic++){
                                //pointer arithmetic
                                Real* velp = velCache + iz*flux_pencil_y*flux_pencil_x+ iy*flux_pencil_x;
                                Real* fluxp = GET_FACE_VAL_PTR(iDir,fluxCache,ic,iz,iy,0);
                                // inner loop
                                for(ix=0;ix<f_xu;ix++){
                                    //std::string action = "FLUX2" + axis;
                                    //fprintf(stderr, "%s: %d,%d,%d,%d (%d,%d)\n", action.c_str(),ic,iz,iy,ix,0,0);

                                    *fluxp *= factor2*(*velp);
                                    ++fluxp;
                                    ++velp;
                                }
                            }
                        }
                    }
                    // compute the second half of the flux calculation
                    // accumulate the differences into the new data box
                    for(iz=0;iz<numCell;iz++){
                        for(iy=0;iy<numCell;iy++){
                            for(ic=0;ic<numComp;ic++){
                                // pointer arithmetic
                                Real* phip = GET_VAL_PTR(new_box,ic,iz,iy,0);
                                Real* fluxp = GET_FACE_VAL_PTR(iDir,fluxCache,ic,iz,iy,0);
                                for(ix=0;ix<numCell;ix++){
                                    //std::string action = "DIFF" + axis;
                                    //fprintf(stderr, "%s: %d,%d,%d,%d (%d,%d)\n", action.c_str(),ic,iz,iy,ix,0,0);

                                    *phip += (*(fluxp + fluxOffset1)) - (*fluxp);
                                    ++phip;
                                    ++fluxp;
                                }
                            }
                        }
                    }
                } // direction loop

                free(fluxCache);
                free(velCache);
            } // box loop
        }

        virtual void Assert() {
            //for(int idx=0;idx < _numBox;idx++) {
                ASSERT_LT(Compare(_new_boxes[0], _ref_boxes[0], _fullNumCell3 * NCOMP), 0);
            //}
        }

        virtual void TearDown() {
            for(int idx=0;idx<_numBox;idx++){
                free(_old_boxes[idx]);
                free(_new_boxes[idx]);
                free(_ref_boxes[idx]);
            }

            free(_old_boxes);
            free(_new_boxes);
            free(_ref_boxes);
        }

        int _numCell;
        int _numBox;

        // The size of the 3D data is (numCell+2*nGhost)^3
        int _fullNumCell;
        int _fullNumCell2;
        int _fullNumCell3;

        Real** _old_boxes;
        Real** _new_boxes;
        Real** _ref_boxes;
    };

    TEST_F(MiniFluxDivTest, MFD) {
        SetUp({""});
        NumRuns(3);
        //NumThreads(1);
        Run();
        Verify();
        Assert();
    }
}
