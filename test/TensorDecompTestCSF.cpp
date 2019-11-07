#include "TensorDecompTest.hpp"
#include <coo_csf_insp.h>
//#include <mttkrp_csf.h>
#include <cp_als_3d_csf.h>

namespace test {
    class TensorDecompTestCSF : public TensorDecompTest {

    protected:
        TensorDecompTestCSF(const string& name = "TensorDecompTestCSF") : TensorDecompTest(name) {
        }

        virtual ~TensorDecompTestCSF() {}

        virtual void Inspect() {
            // Run COO->CSF Inspector!
            coo_csf_insp(_dims, _indices, _nnz, _order, &_fptr, &_findx);
            cerr << "F = " << _fptr[0][1] << endl;
            //TensorEqual();
        }

        virtual void Execute() {
            //unsigned seed = 1568224077;
            cp_als_3d_csf(_vals, _fptr[0][1], _dims[0], _dims[1], _dims[2], _nnz, _rank, _niter, _findx[0], _findx[1], _findx[2], _fptr[1], _fptr[2], _factors[0], _factors[1], _factors[2], _lambda);
            //mttkrp_csf(_factors[1], _factors[2], _vals, _fptr[0][1], _rank, _findx[0], _findx[1], _findx[2], _fptr[1], _fptr[2], _factors[0]);
        }

        virtual void TensorEqual() {
            unsigned m,i,j,k,p,q;

            map<tuple<unsigned, unsigned, unsigned>, double> coo_map;
            for (m = 0; m < _nnz; m++) {
                i = _indices[m];
                j = _indices[_nnz+m];
                k = _indices[_nnz+_nnz+m];
                tuple<unsigned, unsigned, unsigned> triple(i,j,k);
                coo_map[triple] = _vals[m];
            }

            map<tuple<unsigned, unsigned, unsigned>, double> csf_map;
            for(p = _fptr[0][0]; p < _fptr[0][1]; p++) {
                i = _findx[0][p];
                for(q = _fptr[1][p]; q < _fptr[1][p+1]; q++) {
                    j = _findx[1][q];
                    for(m = _fptr[2][q]; m < _fptr[2][q+1]; m++) {
                        k = _findx[2][m];
                        tuple<unsigned, unsigned, unsigned> triple(i,j,k);
                        csf_map[triple] = _vals[m];
                    }
                }
            }

            ASSERT_EQ(coo_map, csf_map);
        }

        unsigned** _fptr;
        unsigned** _findx;
    };

    TEST_F(TensorDecompTestCSF, CPD) {
        //SetUp("./data/tensor/matmul_5-5-5.tns", 10, 11);
        SetUp("../VarDevEddie/themes/tensors/nips.tns", 10, 50);
        Run();
        Verify();
        Assert();
    }
}
