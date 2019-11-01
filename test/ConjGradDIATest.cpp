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

//        cerr << "doff = [";
//        for (unsigned n = 0; n < _ndia; n++) {
//            cerr << _doff[n] << ",";
//        }
//        cerr << "]\n";
//
//        double sum = 0.0;
//        for (unsigned i = 0; i < _nrow; i++) {
//            for (unsigned d = 0; d < _ndia; d++) {
//                unsigned j = i + _doff[d];
//                if (j < _ncol) {
//                    double v = _dval[d * _nrow + i];
//                    //cerr << "(" << i << "," << d << "," << j << "," << v << "), ";
//                    sum += v;
//                }
//            }
//        }
//        cerr << sum << endl;

        MatrixEqual();
    }

    virtual void Execute() {
        _error = conjgrad_dia(_dval, _b, _ndia, _nrow, _maxiter, _doff, _x);
        _niter = _maxiter;
    }

    virtual void MatrixEqual() {
        unsigned d, n, i, j;

        map<pair<unsigned, unsigned>, double> coo_map;
        for (n = 0; n < _nnz; n++) {
            i = _rows[n];
            j = _cols[n];
            pair<unsigned, unsigned> crd = make_pair(i,j);
            coo_map[crd] = _vals[n];
        }

        map<pair<unsigned, unsigned>, double> dia_map;
        for (d = 0; d < _ndia; d++) {
            for (i = 0; i < _nrow; i++) {
                j = i + _doff[d];
                n = _nrow*d+i;
                if (j < _ncol && _dval[n] != 0.0) {
                    pair<unsigned, unsigned> crd = make_pair(i, j);
                    dia_map[crd] = _dval[n];
                }
            }
        }

        ASSERT_EQ(coo_map, dia_map);
    }

    unsigned _ndia;
    int* _doff;
    double* _dval;
};

TEST_F(ConjGradDIATest, CG) {
    //ConjGradTest::SetUp({"./data/matrix/taco.mtx"});
    ConjGradTest::SetUp({"../VarDevEddie/themes/Solver/matrices/mc2depi/mc2depi.mtx"});
    //ConjGradTest::SetUp({"../VarDevEddie/themes/Solver/matrices/cant/cant.mtx"});
    ConjGradTest::Run();
    ConjGradTest::Verify();
    ConjGradTest::Assert();
}
}
