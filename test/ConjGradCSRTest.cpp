#include "InspExecTest.hpp"
#include "ConjGradTest.hpp"
// Include generated code:
#include "coo_csr_insp.h"
#include "conjgrad_csr.h"
//#include "conjgrad_csr_red.h"

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

        ASSERT_EQ(_rowptr[0], 0);
        ASSERT_EQ(_rowptr[_nrow], _nnz);
        //cerr << "rp = [";
        for (unsigned i = 0; i < _nrow; i++) {
            //cerr << _rowptr[i] << ",";
            ASSERT_LE(_rowptr[i], _rowptr[i+1]);
            for (unsigned n = _rowptr[i]; n < _rowptr[i+1]; n++) {
                ASSERT_EQ(i, _rows[n]);
            }
        }
        //cerr << "]\n";
        //MatrixEqual();
    }

    virtual void Execute() {
        _error = conjgrad_csr(_vals, _b, _nrow, _maxiter, _cols, _rowptr, _x);
        _niter = _maxiter;
    }

    virtual void MatrixEqual() {
        map<pair<unsigned, unsigned>, double> coo_map;
        for (unsigned n = 0; n < _nnz; n++) {
            unsigned i = _rows[n];
            unsigned j = _cols[n];
            pair<unsigned, unsigned> crd = make_pair(i,j);
            coo_map[crd] = _vals[n];
        }

        map<pair<unsigned, unsigned>, double> csr_map;
        for (unsigned i = 0; i < _nrow; i++) {
            for (unsigned n = _rowptr[i]; n < _rowptr[i+1]; n++) {
                unsigned j = _cols[n];
                pair<unsigned, unsigned> crd = make_pair(i,j);
                csr_map[crd] = _vals[n];
            }
        }

        ASSERT_EQ(coo_map, csr_map);
    }

    unsigned* _rowptr;
};

TEST_F(ConjGradCSRTest, CG) {
    //ConjGradTest::SetUp({"./data/matrix/impact.mtx"});
    //ConjGradTest::SetUp({"./data/matrix/taco.mtx"});
    ConjGradTest::SetUp({"../VarDevEddie/themes/Solver/matrices/mc2depi/mc2depi.mtx"});
    //ConjGradTest::SetUp({"../VarDevEddie/themes/Solver/matrices/mac_econ_fwd500/mac_econ_fwd500.mtx"});
    ConjGradTest::Run();
    ConjGradTest::Verify();
    ConjGradTest::Assert();
}
}
