#include <string>
using std::string;
using std::to_string;
#include <gtest/gtest.h>
using namespace testing;

#include "TensorDecompTest.hpp"
#include <coo_csf_insp.h>
#include <mttkrp_csf.h>

typedef float real;

namespace test {
    class TensorDecompTestCSF : public TensorDecompTest {

    protected:
        TensorDecompTestCSF(const string& name = "TensorDecompTestCSF") : TensorDecompTest(name) {
        }

        virtual ~TensorDecompTestCSF() {}

        virtual void Inspect() {
            // Run COO->CSF Inspector!
            coo_csf_insp(_dims, _indices, _nnz, _order, &_fptr, &_findx);
            cerr << "F = " << _fptr[0][1] << endl;
        }

        virtual void Execute() {
            //unsigned seed = 1568224077;
            //cp_als_coo(_vals, _niter, _dims[0], _dims[1], _dims[2], _nnz, _rank, &_indices[0], &_indices[_nnz], &_indices[_nnz * 2], _factors[0], _factors[1], _factors[2], _lambda);
            mttkrp_csf(_factors[1], _factors[2], _vals, _fptr[0][1], _rank, _findx[0], _findx[1], _findx[2], _fptr[1], _fptr[2], _factors[0]);
        }

        unsigned** _fptr;
        unsigned** _findx;
    };

    TEST_F(TensorDecompTestCSF, CPD) {
        //SetUp("./data/tensor/matmul_5-5-5.tns", 10, 11);
        SetUp("../VarDevEddie/themes/tensors/nips.tns", 10, 50);
        Run();
        Verify();
        Assert();
    }
}
