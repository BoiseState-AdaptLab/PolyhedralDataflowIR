#include "TensorDecompTest.hpp"
#include <coo_hicoo_insp.h>
#include <cp_als_nd_hicoo.h>

namespace test {
    class TensorDecompTestHiCOO : public TensorDecompTest {

    protected:
        TensorDecompTestHiCOO(const string& name = "TensorDecompTestHiCOO") : TensorDecompTest(name) {
            _bptr = nullptr;
            _bindices = nullptr;
            _eindices = nullptr;
            _bval = nullptr;
            _nb = 0;
            _bs = 256; //64; //128;
        }

        virtual ~TensorDecompTestHiCOO() {}

        virtual void Inspect() {
            // Run COO->HiCOO Inspector!
            _nb = coo_hicoo_insp(_vals, _bs, _nnz, _order, _dims, _indices, &_bval, &_bindices, &_bptr, &_eindices);
            //TensorEqual();
            cerr << "NB = " << _nb << endl;
        }

        virtual void Execute() {
            cp_als_nd_hicoo(_bval, _bs, _nnz, _order, _nb, _rank, _niter, _dims, _bindices, _bptr, _eindices, _factors, _lambda);
        }

        virtual void TensorEqua3D() {
            unsigned m,i,j,k,b,bi,bj,bk;

            map<tuple<unsigned, unsigned, unsigned>, float> coo_map;
            for (m = 0; m < _nnz; m++) {
                i = _indices[m];
                j = _indices[_nnz+m];
                k = _indices[_nnz+_nnz+m];
                tuple<unsigned, unsigned, unsigned> triple(i,j,k);
                coo_map[triple] = _vals[m];
            }

            map<tuple<unsigned, unsigned, unsigned>, float> hicoo_map;
            for(b = 0; b < _nb; b++) {
                bi=_bindices[(b)*_order];
                bj=_bindices[(b)*_order+1];
                bk=_bindices[(b)*_order+2];
                for(m = _bptr[b]; m < _bptr[b+1]; m++) {
                    i=_bs*bi+_eindices[(m)*_order];
                    j=_bs*bj+_eindices[(m)*_order+1];
                    k=_bs*bk+_eindices[(m)*_order+2];
                    tuple<unsigned, unsigned, unsigned> triple(i,j,k);
                    hicoo_map[triple] = _bval[m]; //_vals[m];
                }
            }

            ASSERT_EQ(coo_map, hicoo_map);
        }

        virtual void TensorEqual4D() {
            unsigned m,i,j,k,l,b,bi,bj,bk,bl;

            map<tuple<unsigned, unsigned, unsigned, unsigned>, float> coo_map;
            for (m = 0; m < _nnz; m++) {
                i = _indices[m];
                j = _indices[_nnz+m];
                k = _indices[_nnz+_nnz+m];
                l = _indices[_nnz+_nnz+_nnz+m];
                tuple<unsigned, unsigned, unsigned, unsigned> coords(i,j,k,l);
                coo_map[coords] = _vals[m];
            }

            map<tuple<unsigned, unsigned, unsigned, unsigned>, float> hicoo_map;
            for(b = 0; b < _nb; b++) {
                bi=_bindices[(b)*_order];
                bj=_bindices[(b)*_order+1];
                bk=_bindices[(b)*_order+2];
                bl=_bindices[(b)*_order+3];
                for(m = _bptr[b]; m < _bptr[b+1]; m++) {
                    i=_bs*bi+_eindices[(m)*_order];
                    j=_bs*bj+_eindices[(m)*_order+1];
                    k=_bs*bk+_eindices[(m)*_order+2];
                    l=_bs*bl+_eindices[(m)*_order+3];

                    tuple<unsigned, unsigned, unsigned, unsigned> coords(i,j,k,l);
                    hicoo_map[coords] = _bval[m];
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
        //SetUp("../VarDevEddie/themes/tensors/matmul_3-3-3.tns", 10, 50);
        //SetUp("../VarDevEddie/themes/tensors/hicoo.tns", 10, 7);
        //SetUp("../VarDevEddie/themes/tensors/nell-1.tns", 10, 7);
        //SetUp("../VarDevEddie/themes/tensors/nell-2.tns", 10, 50);
        //SetUp("../VarDevEddie/themes/tensors/nips.tns", 10, 7);
        SetUp("../VarDevEddie/themes/tensors/enron.tns", 10, 7);
        Run();
        Verify();
        Assert();
    }
}
