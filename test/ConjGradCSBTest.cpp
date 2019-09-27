#include "InspExecTest.hpp"
#include "ConjGradTest.hpp"
// Include generated code:
#include "coo_csb_insp.h"
#include "conjgrad_csb.h"

namespace test {
class ConjGradCSBTest : public ConjGradTest {
protected:
    ConjGradCSBTest() : ConjGradTest("ConjGradCSBTest") {
        _brow = _bcol = _bptr = _erow = _ecol = nullptr;
        _nb = 0;
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
        _nb = coo_csb_insp(_rows, _nnz, &_crow, &_crp);
//        ASSERT_EQ(_rowptr[0], 0);
//        ASSERT_EQ(_rowptr[_nrow], _nnz);
//        for (unsigned i = 0; i < _nrow; i++) {
//            ASSERT_LE(_rowptr[i], _rowptr[i+1]);
//        }
    }

    virtual void Execute() {
        _error = conjgrad_csb(_vals, _b, _nrow, _nb, _maxiter, _cols, _brow, _bcol, _erow, _ecol, _x);
        _niter = _maxiter;
    }

    unsigned* _bptr;
    unsigned* _brow;
    unsigned* _bcol;
    unsigned* _erow;
    unsigned* _ecol;
    unsigned _nb;
};

TEST_F(ConjGradCSBTest, CG) {
    ConjGradTest::SetUp({"./data/matrix/cant.mtx"});
    ConjGradTest::Run();
    ConjGradTest::Verify();
    ConjGradTest::Assert();
}
}
