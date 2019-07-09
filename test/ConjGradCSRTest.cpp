#include "InspExecTest.hpp"
#include "ConjGradTest.hpp"
// Include generated code:
#include "coo_csr_insp.h"
#include "conjgrad_csr.h"

namespace test {
class ConjGradCSRTest : public ConjGradTest {
protected:
    ConjGradCSRTest() : ConjGradTest("ConjGradCSRTest") {
        _rowptr = nullptr;
    }

    virtual ~ConjGradCSRTest() {
        if (_rowptr != nullptr) {
            free(_rowptr);
        }
    }

    virtual void Inspect() {
        // Run COO->CSR Inspector!
        _rowptr = (unsigned*) calloc(_nrow + 1, sizeof(unsigned));
        coo_csr_insp(_nnz, _rows, _rowptr);
//        ASSERT_EQ(_rowptr[0], 0);
//        ASSERT_EQ(_rowptr[_nrow], _nnz);
//        for (unsigned i = 0; i < _nrow; i++) {
//            ASSERT_LE(_rowptr[i], _rowptr[i+1]);
//        }
    }

    virtual void Execute() {
        _error = conjgrad_csr(_vals, _b, _nrow, _maxiter, _cols, _rowptr, _x);
        _niter = _maxiter;
    }

    unsigned* _rowptr;
};

TEST_F(ConjGradCSRTest, CG) {
    ConjGradTest::SetUp({"./data/matrix/cant.mtx"});
    ConjGradTest::Run();
    ConjGradTest::Verify();
    ConjGradTest::Assert();
}
}
