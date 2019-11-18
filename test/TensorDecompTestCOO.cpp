#include "TensorDecompTest.hpp"
#include <cp_als_nd_coo.h>

namespace test {
    class TensorDecompTestCOO : public TensorDecompTest {

    protected:
        TensorDecompTestCOO(const string& name = "TensorDecompTestCOO") : TensorDecompTest(name) {
            _inspFlag = false;          // No inspector needed on this one.
        }

        virtual ~TensorDecompTestCOO() {}

        virtual void Execute() {
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
