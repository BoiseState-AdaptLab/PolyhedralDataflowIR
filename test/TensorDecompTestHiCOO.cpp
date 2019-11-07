#include "TensorDecompTest.hpp"
#include <coo_hicoo_insp.h>
//#include <mttkrp_csf.h>
#include <cp_als_3d_hicoo.h>

namespace test {
    class TensorDecompTestHiCOO : public TensorDecompTest {

    protected:
        TensorDecompTestHiCOO(const string& name = "TensorDecompTestHiCOO") : TensorDecompTest(name) {
            _bptr = nullptr;
            _bindices = nullptr;
            _eindices = nullptr;
            _bval = nullptr;
            _nb = 0;
            _bs = 128;
        }

        virtual ~TensorDecompTestHiCOO() {}

        virtual void Inspect() {
            // Run COO->HiCOO Inspector!
            //coo_csf_insp(_dims, _indices, _nnz, _order, &_fptr, &_findx);
            //_nb = coo_hicoo_insp(_vals, _bs, _nnz, _order, _dims, _indices, &_bindices, &_bptr, &_eindices, &_bval);
            _nb = coo_hicoo_insp(_vals, _bs, _nnz, _order, _dims, _indices, &_bval, &_bindices, &_bptr, &_eindices);
            cerr << "NB = " << _nb << endl;
        }

        virtual void Execute() {
            cp_als_3d_hicoo(_vals, _bs, _dims[0], _dims[1], _dims[2], _nnz, _nb, _rank, _niter, _bindices, _bptr, _eindices, _factors[0], _factors[1], _factors[2], _lambda);
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

            // TODO: Implement HiCOO traversal!
            map<tuple<unsigned, unsigned, unsigned>, double> hicoo_map;
            for(p = _fptr[0][0]; p < _fptr[0][1]; p++) {
                i = _findx[0][p];
                for(q = _fptr[1][p]; q < _fptr[1][p+1]; q++) {
                    j = _findx[1][q];
                    for(m = _fptr[2][q]; m < _fptr[2][q+1]; m++) {
                        k = _findx[2][m];
                        tuple<unsigned, unsigned, unsigned> triple(i,j,k);
                        hicoo_map[triple] = _vals[m];
                    }
                }
            }

            ASSERT_EQ(coo_map, hicoo_map);
        }

        unsigned* _bptr;
        unsigned short* _bindices;
        unsigned char* _eindices;
        unsigned _nb;
        unsigned _bs;
        real* _bval;
    };

    TEST_F(TensorDecompTestHiCOO, CPD) {
        //SetUp("./data/tensor/matmul_5-5-5.tns", 10, 11);
        SetUp("../VarDevEddie/themes/tensors/nips.tns", 10, 50);
        Run();
        Verify();
        Assert();
    }
}
