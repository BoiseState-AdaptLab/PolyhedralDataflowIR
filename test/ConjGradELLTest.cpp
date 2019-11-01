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

        ASSERT_TRUE(_lcol != NULL);
        ASSERT_TRUE(_lval != NULL);
//        #define offset2(i,j,M) ((j)+(i)*(M))
//        for (unsigned k = 0; k < _nell; k++) {
//            for (unsigned i = 0; i < _nrow; i++) {
//                unsigned j = _lcol[offset2((k),(i),_nrow)];
//                double v = _lval[offset2((k),(i),_nrow)];
//                cerr << "(" << k << "," << i << "," << j << "," << v << "), ";
//            }
//        }
//        cerr << endl;
        //MatrixEqual();
    }

    virtual void Execute() {
        _error = conjgrad_ell(_lval, _b, _nell, _nrow, _maxiter, _lcol, _x);
        _niter = _maxiter;
    }

    virtual void MatrixEqual() {
        unsigned k, n, i, j;

        map<pair<unsigned, unsigned>, double> coo_map;
        for (n = 0; n < _nnz; n++) {
            i = _rows[n];
            j = _cols[n];
            pair<unsigned, unsigned> crd = make_pair(i,j);
            coo_map[crd] = _vals[n];
        }

        map<pair<unsigned, unsigned>, double> ell_map;
        for (i = 0; i < _nrow; i++) {
            for (k = 0; k < _nell; k++) {
                n = k*_nrow+i;
                if (_lval[n] != 0.0) {
                    j = _lcol[n];
                    pair<unsigned, unsigned> crd = make_pair(i, j);
                    ell_map[crd] = _lval[n];
                }
            }
        }

        ASSERT_EQ(coo_map, ell_map);
    }

    unsigned _nell;
    unsigned* _lcol;
    double* _lval;
};

TEST_F(ConjGradELLTest, CG) {
    //ConjGradTest::SetUp({"./data/matrix/cant.mtx"});
    //ConjGradTest::SetUp({"./data/matrix/taco.mtx"});
    //ConjGradTest::SetUp({"../VarDevEddie/themes/Solver/matrices/mc2depi/mc2depi.mtx"});
    ConjGradTest::SetUp({"../VarDevEddie/themes/Solver/matrices/mac_econ_fwd500/mac_econ_fwd500.mtx"});
    ConjGradTest::Run();
    ConjGradTest::Verify();
    ConjGradTest::Assert();
}
}
