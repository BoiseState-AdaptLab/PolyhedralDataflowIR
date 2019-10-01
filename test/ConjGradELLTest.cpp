#include "InspExecTest.hpp"
#include "ConjGradTest.hpp"
// Include generated code:
#include "coo_ell_insp.h"
#include "conjgrad_ell.h"

namespace test {
class ConjGradELLTest : public ConjGradTest {
protected:
    ConjGradELLTest() : ConjGradTest("ConjGradELLTest") {
        _lcol = nullptr;
        _lval = nullptr;
        _nell = 0;
    }

    virtual ~ConjGradELLTest() {
        if (_lcol != nullptr) {
            free(_lcol);
        }
        if (_lval != nullptr) {
            free(_lval);
        }
    }

    virtual void Inspect() {
        // Run COO->CSR Inspector!
        _nell = coo_ell_insp(_nnz, _rows, _cols, _vals, &_lcol, &_lval);
        cerr << "K = " << _nell << endl;
//        #define offset2(i,j,M) ((j)+(i)*(M))
//        for (unsigned k = 0; k < _nell; k++) {
//            for (unsigned i = 0; i < _nrow; i++) {
//                unsigned j = _lcol[offset2((k),(i),_nrow)];
//                double v = _lval[offset2((k),(i),_nrow)];
//                cerr << "(" << k << "," << i << "," << j << "," << v << "), ";
//            }
//        }
//        cerr << endl;
    }

    virtual void Execute() {
        _error = conjgrad_ell(_lval, _b, _nell, _nrow, _maxiter, _lcol, _x);
        _niter = _maxiter;
    }

    unsigned _nell;
    unsigned* _lcol;
    double* _lval;
};

TEST_F(ConjGradELLTest, CG) {
    //ConjGradTest::SetUp({"./data/matrix/cant.mtx"});
    //ConjGradTest::SetUp({"./data/matrix/taco.mtx"});
    ConjGradTest::SetUp({"../VarDevEddie/themes/Solver/matrices/mc2depi/mc2depi.mtx"});
    ConjGradTest::Run();
    ConjGradTest::Verify();
    ConjGradTest::Assert();
}
}
