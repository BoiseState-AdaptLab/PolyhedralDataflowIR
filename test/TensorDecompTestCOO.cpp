#include <string>
using std::string;
using std::to_string;
#include <gtest/gtest.h>
using namespace testing;

//#include <cp_als_coo.h>
#include <cp_als_3d_coo.h>

#ifdef SPLATT_ENABLED
#include <splatt.h>
#endif

#include <util/MatrixIO.hpp>
using util::MatlabIO;
using util::TensorIO;

#include "TensorDecompTest.hpp"

typedef float real;

namespace test {
    class TensorDecompTestCOO : public TensorDecompTest {

    protected:
        TensorDecompTestCOO(const string& name = "TensorDecompTestCOO") : TensorDecompTest(name) {
            _inspFlag = false;          // No inspector needed on this one.
        }

        virtual ~TensorDecompTestCOO() {}

        virtual void Execute() {
            //unsigned seed = 1568224077;
            cp_als_3d_coo(_vals, _dims[0], _dims[1], _dims[2], _nnz, _rank, _niter, &_indices[0], &_indices[_nnz], &_indices[_nnz*2], _factors[0], _factors[1], _factors[2], _lambda);
        }
    };

    TEST_F(TensorDecompTestCOO, CPD) {
        SetUp("./data/tensor/matmul_5-5-5.tns", 10, 11);
        Run();
        Verify();
        Assert();
    }
}
