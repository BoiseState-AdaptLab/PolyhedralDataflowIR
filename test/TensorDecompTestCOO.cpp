#include "TensorDecompTest.hpp"
//#include <cp_als_coo.h>
//#include <cp_als_3d_coo.h>
#include <cp_als_nd_coo.h>

namespace test {
    class TensorDecompTestCOO : public TensorDecompTest {

    protected:
        TensorDecompTestCOO(const string& name = "TensorDecompTestCOO") : TensorDecompTest(name) {
            _inspFlag = false;          // No inspector needed on this one.
        }

        virtual ~TensorDecompTestCOO() {}

        virtual void Execute() {
            //unsigned seed = 1568224077;
            //cp_als_3d_coo(_vals, _dims[0], _dims[1], _dims[2], _nnz, _rank, _niter, &_indices[0], &_indices[_nnz], &_indices[_nnz*2], _factors[0], _factors[1], _factors[2], _lambda);
            cp_als_nd_coo(_vals, _nnz, _order, _rank, _niter, _dims, _indices, _factors, _lambda);
        }
    };

    TEST_F(TensorDecompTestCOO, CPD) {
        //SetUp("./data/tensor/matmul_5-5-5.tns", 10, 50);
        SetUp("../VarDevEddie/themes/tensors/nips.tns", 10, 7);
        Run();
        Verify();
        Assert();
    }
}
