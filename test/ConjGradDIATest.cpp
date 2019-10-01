#include "InspExecTest.hpp"
#include "ConjGradTest.hpp"
// Include generated code:
#include "coo_dia_insp.h"
#include "conjgrad_dia.h"

namespace test {
class ConjGradDIATest : public ConjGradTest {
protected:
    ConjGradDIATest() : ConjGradTest("ConjGradDIATest") {
        _doff = nullptr;
        _dval = nullptr;
        _ndia = 0;
    }

    virtual ~ConjGradDIATest() {
        if (_doff != nullptr) {
            free(_doff);
        }
        if (_dval != nullptr) {
            free(_dval);
        }
    }

    virtual void Inspect() {
        // Run COO->DIA Inspector!
        _ndia = coo_dia_insp(_vals, _nnz, _cols, _rows, &_dval, &_doff);
        cerr << "D = " << _ndia << endl;

//        #define offset2(i,j,M) ((j)+(i)*(M))
//        for (unsigned i = 0; i < _nrow; i++) {
//            for (unsigned d = 0; d < _ndia; d++) {
//                unsigned j = i + _doff[d];
//                if (j < _ncol) {
//                    double v = _dval[offset2((d), (i), _nrow)];
//                    cerr << "(" << i << "," << j << "," << v << "), ";
//                }
//            }
//        }
//        cerr << endl;
    }

    virtual void Execute() {
        _error = conjgrad_dia(_dval, _b, _ndia, _nrow, _maxiter, _doff, _x);
        _niter = _maxiter;
    }

    unsigned _ndia;
    int* _doff;
    double* _dval;
};

TEST_F(ConjGradDIATest, CG) {
    //ConjGradTest::SetUp({"./data/matrix/cant.mtx"});
    //ConjGradTest::SetUp({"./data/matrix/taco.mtx"});
    ConjGradTest::SetUp({"../VarDevEddie/themes/Solver/matrices/mc2depi/mc2depi.mtx"});
    ConjGradTest::Run();
    ConjGradTest::Verify();
    ConjGradTest::Assert();
}
}
