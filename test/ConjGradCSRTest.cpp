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
    }

    virtual void Execute() {
        _error = conjgrad_csr(_vals, _b, _nrow, _maxiter, _cols, _rowptr, _x);
    }

    virtual void Evaluate() {
// TODO: Call ConjGradCOOTest::Execute.
    }

    unsigned* _rowptr;
};

TEST_F(ConjGradCSRTest, CG) {
    ConjGradTest::SetUp({"./data/matrix/cant.mtx"});
    ConjGradTest::Run();
    ConjGradTest::Verify();
    ConjGradTest::Assert();
    int stop = 1;
}
}
