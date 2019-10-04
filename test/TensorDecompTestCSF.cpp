#include <string>
using std::string;
using std::to_string;
#include <gtest/gtest.h>
using namespace testing;

#include <coo_csf_insp.h>
#include <mttkrp_csf.h>

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
            // Run COO->CSF Inspector!
            coo_csf_insp(_dims, _indices, _nnz, _order, &_fptr, &_find);
            cerr << "F = " << _fptr[0][1] << endl;
        }

        virtual void Execute() {
            //unsigned seed = 1568224077;
            //cp_als_coo(_vals, _niter, _dims[0], _dims[1], _dims[2], _nnz, _rank, &_indices[0], &_indices[_nnz], &_indices[_nnz * 2], _factors[0], _factors[1], _factors[2], _lambda);
            mttkrp_csf(const float* B, const float* C, const float* X, const unsigned F, const unsigned R, const unsigned* ind0, const unsigned* ind1, const unsigned* ind2, const unsigned* pos0, const unsigned* pos1, float* A);
        }

        unsigned** _fptr;
        unsigned** _find;
    };

    TEST_F(TensorDecompTestCSF, CPD) {
        SetUp("./data/tensor/matmul_5-5-5.tns", 10, 11);
        Run();
        Verify();
        Assert();
    }
}
