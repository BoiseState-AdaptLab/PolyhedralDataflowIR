#include "InspExecTest.hpp"
#include "ConjGradTest.hpp"
// Include generated code:
#include "coo_dsr_insp.h"
#include "conjgrad_dsr.h"

namespace test {
class ConjGradDSRTest : public ConjGradTest {
protected:
    ConjGradDSRTest() : ConjGradTest("ConjGradDSRTest") {
        _crp = _crow = nullptr;
        _nzr = 0;
    }

    virtual ~ConjGradDSRTest() {
        if (_crp != nullptr) {
            free(_crp);
        }
        if (_crow != nullptr) {
            free(_crow);
        }
    }

    virtual void Inspect() {
        // Run COO->CSR Inspector!
        _nzr = coo_dsr_insp(_rows, _nnz, &_crow, &_crp);
        cerr << "R=" << _nzr << endl;
//        ASSERT_EQ(_rowptr[0], 0);
//        ASSERT_EQ(_rowptr[_nrow], _nnz);
//        for (unsigned i = 0; i < _nrow; i++) {
//            ASSERT_LE(_rowptr[i], _rowptr[i+1]);
//        }
        //MatrixEqual();
    }

    virtual void Execute() {
        _error = conjgrad_dsr(_vals, _b, _nrow, _nzr, _maxiter, _cols, _crow, _crp, _x);
        _niter = _maxiter;
    }

    virtual void MatrixEqual() {
        unsigned m, n, i, j;

        map<pair<unsigned, unsigned>, double> coo_map;
        for (n = 0; n < _nnz; n++) {
            i = _rows[n];
            j = _cols[n];
            pair<unsigned, unsigned> crd = make_pair(i,j);
            coo_map[crd] = _vals[n];
        }

        map<pair<unsigned, unsigned>, double> dsr_map;
        for (m = 0; m < _nzr; m++) {
            i = _crow[m];
            for (n = _crp[m]; n < _crp[m+1]; n++) {
                j = _cols[n];
                pair<unsigned, unsigned> crd = make_pair(i,j);
                dsr_map[crd] = _vals[n];
            }
        }

        ASSERT_EQ(coo_map, dsr_map);
    }

    unsigned* _crp;
    unsigned* _crow;
    unsigned _nzr;
};

TEST_F(ConjGradDSRTest, CG) {
    //ConjGradTest::SetUp({"./data/matrix/cant.mtx"});
    ConjGradTest::SetUp({"../VarDevEddie/themes/Solver/matrices/mc2depi/mc2depi.mtx"});
    //ConjGradTest::SetUp({"./data/matrix/taco.mtx"});
    ConjGradTest::Run();
    ConjGradTest::Verify();
    ConjGradTest::Assert();
}
}
