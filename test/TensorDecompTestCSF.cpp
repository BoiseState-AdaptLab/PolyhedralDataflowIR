#include <string>
using std::string;
using std::to_string;
#include <gtest/gtest.h>
using namespace testing;

#include <cp_als_coo.h>

#ifdef SPLATT_ENABLED
#include <splatt.h>
#endif

#include <util/MatrixIO.hpp>
using util::MatlabIO;
using util::TensorIO;

#include "TensorDecompTest.hpp"

typedef float real;

namespace test {
    class TensorDecompTestCSF : public TensorDecompTest {

    protected:
        TensorDecompTestCSF(const string& name = "TensorDecompTestCSF") : TensorDecompTest(name) {
        }

        virtual ~TensorDecompTestCSF() {}

        virtual void Inspect() {
            // Run COO->DIA Inspector!
            _nfibers = coo_csf_insp(_vals, _nnz, _cols, _rows, &_dval, &_doff);
            cerr << "F = " << _nfibers << endl;
        }

        virtual void Execute() {
            //unsigned seed = 1568224077;
            //cp_als_coo(const float* X, const unsigned I, const unsigned J, const unsigned K, const unsigned M, const unsigned R, const unsigned* ind0, const unsigned* ind1, const unsigned* ind2, float* A, float* B, float* C, float* lmbda)
            cp_als_coo(_vals, _niter, _dims[0], _dims[1], _dims[2], _nnz, _rank, &_indices[0], &_indices[_nnz], &_indices[_nnz * 2], _factors[0], _factors[1], _factors[2], _lambda);
        }

        unsigned _nfibers;
        int* _doff;
        double* _dval;
    };

    TEST_F(TensorDecompTestCSF, CPD) {
        SetUp("./data/tensor/matmul_5-5-5.tns", 10, 11);
        Run();
        Verify();
        Assert();
    }
}
