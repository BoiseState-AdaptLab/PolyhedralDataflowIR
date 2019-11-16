#include "InspExecTest.hpp"
#include "ConjGradTest.hpp"
// Include generated code:
#include "coo_csb_insp.h"
#include "conjgrad_csb.h"

namespace test {
class ConjGradCSBTest : public ConjGradTest {
protected:
    ConjGradCSBTest() : ConjGradTest("ConjGradCSBTest") {
        _brow = _bcol = _bptr = nullptr;
        _erow = _ecol = nullptr;
        _nb = 0;
        _bs = 128;
    }

    virtual ~ConjGradCSBTest() {
        if (_bptr != nullptr) {
            free(_bptr);
        }
        if (_brow != nullptr) {
            free(_brow);
        }
        if (_bcol != nullptr) {
            free(_bcol);
        }
        if (_erow != nullptr) {
            free(_erow);
        }
        if (_ecol != nullptr) {
            free(_ecol);
        }
    }

    virtual void Inspect() {
        // Run COO->CSR Inspector!
        _nb = coo_csb_insp(_vals, _bs, _nnz, _cols, _rows, &_bval, &_bcol, &_bptr, &_brow, &_ecol, &_erow);
        cerr << "NB=" << _nb << ",BS=" << _bs << endl;
        //MatrixEqual();
    }

    virtual void Execute() {
        //_error = conjgrad_csb(_vals, _b, _nrow, _nb, _maxiter, _cols, _brow, _bcol, _erow, _ecol, _x);
        _error = conjgrad_csb(_bval, _b, _bs, _nrow, _nb, _maxiter, _bptr, _brow, _bcol, _erow, _ecol, _x);
        _niter = _maxiter;
    }

    virtual void MatrixEqual() {
        unsigned b, n, i, j;

        map<pair<unsigned, unsigned>, double> coo_map;
        for (n = 0; n < _nnz; n++) {
            i = _rows[n];
            j = _cols[n];
            pair<unsigned, unsigned> crd = make_pair(i,j);
            coo_map[crd] = _vals[n];
        }

        map<pair<unsigned, unsigned>, double> csb_map;
        for (b = 0; b < _nb; b++) {
            for (n = _bptr[b]; n < _bptr[b+1]; n++) {
                i = _bs * _brow[b] + _erow[n];
                j = _bs * _bcol[b] + _ecol[n];
                pair<unsigned, unsigned> crd = make_pair(i, j);
                csb_map[crd] = _bval[n];
            }
        }

        ASSERT_EQ(coo_map, csb_map);
    }

    unsigned* _bptr;
    unsigned* _brow;
    unsigned* _bcol;
    unsigned char* _erow;
    unsigned char* _ecol;
    unsigned _nb;
    unsigned _bs;
    double* _bval;
};

TEST_F(ConjGradCSBTest, CG) {
    ConjGradTest::SetUp({"../VarDevEddie/themes/Solver/matrices/mc2depi/mc2depi.mtx"});
    ConjGradTest::Run();
    ConjGradTest::Verify();
    ConjGradTest::Assert();
}
}
