//#ifndef _CONJGRADCSRTEST_HPP_
//#define _CONJGRADCSRTEST_HPP_

#include "InspExecTest.hpp"
#include "ConjGradTest.cpp"
// Include generated code:
#include "coo_csr_insp.h"
#include "conjgrad_csr.h"

namespace test {
class ConjGradCSRTest : public InspExecTest, public ConjGradTest {

protected:
    ConjGradCSRTest() : InspExecTest("ConjGradCSRTest") {
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
        unsigned nrow = coo_csr_insp(_nnz, _rows, _rowptr);
        int stop = 1;
    }

    virtual void Execute() {
        unsigned nbytes = _nrow * sizeof(double);
        double* r = (double*) malloc(nbytes);
        double* d = (double*) malloc(nbytes);

        // copy
        memcpy(r, _b, nbytes);
        memcpy(d, r, nbytes);

        // conjgrad
        unsigned t = 0;
        //for (; t < _maxiter && _error > _tolerance; t++) {
        for (; t < _maxiter; t++) {
            _error = conjgrad_csr(_vals, _nrow, _cols, _rowptr, d, r, _x);
        }

        free(r);
        free(d);

        _niter = t;
    }

    unsigned* _rowptr;
};

TEST_F(ConjGradCSRTest, CG) {
    ConjGradTest::SetUp({"./data/matrix/cant.mtx"});
    Inspect();
    ConjGradTest::Run();
    ConjGradTest::Verify();
    ConjGradTest::Assert();
    int stop = 1;
}
}

// _CONJGRADCSRTEST_HPP_
