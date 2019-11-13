#include "TensorDecompTest.hpp"
#include <coo_csf_insp.h>
//#include <mttkrp_csf.h>
//#include <cp_als_3d_csf.h>
//#include <cp_als_4d_csf.h>
#include <cp_als_nd_csf.h>

namespace test {
    class TensorDecompTestCSF : public TensorDecompTest {

    protected:
        TensorDecompTestCSF(const string& name = "TensorDecompTestCSF") : TensorDecompTest(name) {
        }

        virtual ~TensorDecompTestCSF() {}

        virtual void Inspect() {
            // Run COO->CSF Inspector!
            coo_csf_insp(_dims, _indices, _nnz, _order, &_fptr, &_findex);
            cerr << "F = " << _fptr[0][1] << endl;
            //TensorEqual();
            //TensorEqualND();
        }

        virtual void Execute() {
//            if (_order == 3) {
//                cp_als_3d_csf(_vals, _fptr[0][1], _dims[0], _dims[1], _dims[2], _nnz, _rank, _niter, _findx[0], _findx[1], _findx[2], _fptr[1], _fptr[2], _factors[0], _factors[1], _factors[2], _lambda);
//            } else if (_order == 4) {
//                //cp_als_4d_csf(_vals, _fptr[0][1], _dims[0], _dims[1], _dims[2], _dims[3], _nnz, _rank, _niter, _findx[0], _findx[1], _findx[2], _findx[3], _fptr[1], _fptr[2], _fptr[3], _factors[0], _factors[1], _factors[2], _factors[3], _lambda);
//            }
            cp_als_nd_csf(_vals, _nnz, _order, _rank, _niter, _dims, _fptr, _findex, _factors, _lambda);
        }

        virtual void TensorEqual3D() {
            unsigned m,n,i,j,k,p,q,s;

            map<tuple<unsigned, unsigned, unsigned>, float> coo_map;
            for (m = 0; m < _nnz; m++) {
                i = _indices[m];
                j = _indices[_nnz+m];
                k = _indices[_nnz+_nnz+m];
                tuple<unsigned, unsigned, unsigned> triple(i,j,k);
                coo_map[triple] = _vals[m];
            }

            map<tuple<unsigned, unsigned, unsigned>, float> csf_map;
            for(p = _fptr[0][0]; p < _fptr[0][1]; p++) {
                i = _findex[0][p];
                for(q = _fptr[1][p]; q < _fptr[1][p+1]; q++) {
                    j = _findex[1][q];
                    for(m = _fptr[2][q]; m < _fptr[2][q+1]; m++) {
                        k = _findex[2][m];
                        tuple<unsigned, unsigned, unsigned> triple(i,j,k);
                        csf_map[triple] = _vals[m];
                    }
                }
            }

            ASSERT_EQ(coo_map, csf_map);
        }

        virtual void TensorEqual4D() {
            unsigned m,i,j,k,l,p,q,r;

            map<tuple<unsigned, unsigned, unsigned, unsigned>, float> coo_map;
            for (m = 0; m < _nnz; m++) {
                i = _indices[m];
                j = _indices[_nnz+m];
                k = _indices[_nnz+_nnz+m];
                l = _indices[_nnz+_nnz+_nnz+m];
                tuple<unsigned, unsigned, unsigned, unsigned> coords(i,j,k,l);
                coo_map[coords] = _vals[m];
            }

            map<tuple<unsigned, unsigned, unsigned, unsigned>, float> csf_map;
            for(p = _fptr[0][0]; p < _fptr[0][1]; p++) {
                i = _findex[0][p];
                for(q = _fptr[1][p]; q < _fptr[1][p+1]; q++) {
                    j = _findex[1][q];
                    for(r = _fptr[2][q]; r < _fptr[2][q+1]; r++) {
                        k = _findex[2][r];
                        for(m = _fptr[3][r]; m < _fptr[3][r+1]; m++) {
                            l = _findex[3][m];
                            tuple<unsigned,unsigned,unsigned,unsigned> coords(i,j,k,l);
                            csf_map[coords] = _vals[m];
                        }
                    }
                }
            }

            ASSERT_EQ(coo_map, csf_map);
        }

        virtual void TensorEqualND() {
            unsigned f,m,n,p,s,i,j,k,l,q,r;

            map<vector<unsigned>, float> coo_map;
            map<vector<unsigned>, float> csf_map;
            vector<unsigned> crd(_order, 0);

            for(p = _fptr[0][0]; p < _fptr[0][1]; p++) {
                i = _findex[0][p];
                for(q = _fptr[1][p]; q < _fptr[1][p+1]; q++) {
                    j = _findex[1][q];
                    for(r = _fptr[2][q]; r < _fptr[2][q+1]; r++) {
                        k = _findex[2][r];
                        for(m = _fptr[3][r]; m < _fptr[3][r+1]; m++) {
                            l = _findex[3][m];
                            fprintf(stderr, "(p,i,q,j,r,k,m,l)=(%u,%u,%u,%u,%u,%u,%u,%u)\n", p, i, q, j, r, k, m, l);
                            int stop = 1;
                        }
                    }
                }
            }

            //s = 0;
            unsigned F = _fptr[0][1];
            for (f = 0; f < F; f++) {
                i = _findex[0][f];
                crd[n] = i;

                for (n = 1; n < _order; n++) {
                    p = f;
                    for(q = _fptr[n][p]; q < _fptr[n][p+1]; q++) {
                        i = _findex[n][q];
                        crd[n] = i;
                        //s = p;
                        if (n == _order - 1) {
                            vector<unsigned> cpy = crd;
                            csf_map[cpy] = _vals[p];
                            //s = 0;
                        }
                    }
                }
            }

            for (m = 0; m < _nnz; m++) {
                p = m;
                for (n = 0; n < _order; n++) {
                    crd[n] = _indices[p];
                    p += _nnz;
                }
                vector<unsigned> cpy = crd;
                coo_map[cpy] = _vals[m];
            }

            ASSERT_EQ(coo_map, csf_map);
        }

        unsigned** _fptr;
        unsigned** _findex;
    };

    TEST_F(TensorDecompTestCSF, CPD) {
        //SetUp("./data/tensor/matmul_5-5-5.tns", 10, 11);
        SetUp("../VarDevEddie/themes/tensors/nips.tns", 10, 10);
        //SetUp("../VarDevEddie/themes/tensors/nell-2.tns", 10, 50);
        Run();
        Verify();
        Assert();
    }
}
