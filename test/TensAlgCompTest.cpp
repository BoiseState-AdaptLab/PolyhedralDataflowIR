#include <string>
using std::string;
using std::to_string;
#include <gtest/gtest.h>
using namespace testing;

#include <taco.h>
using namespace taco;

#include <util/MatrixIO.hpp>
using util::MatlabIO;
using util::TensorIO;

#include "BenchmarkTest.hpp"

namespace test {
    class TensAlgCompTest : public ::testing::Test {

    protected:
        TensAlgCompTest() {}

        virtual ~TensAlgCompTest() {}

        void SetUp(const string& filename, const int rank = 10) {
            //_rank = rank;

//            Use matrix addition,
//            C(i,j) = A(i,j) + B(i,j)
//            Or Hadamard product,
//            C(i,j) = A(i,j) * B(i,j)
//
//            Where A is input format, C is output, and B is zeros if sum, or ones if product.

            Format csr({Dense,Sparse});
            Format  dv({Dense});

            // Load a sparse matrix from file (stored in the Matrix Market format) and
            // store it as a compressed sparse row matrix. Matrices correspond to order-2
            // tensors in taco.
            Tensor<double> A = read(filename, csr);

            Tensor<double> B({A.getDimension(0), A.getDimension(1)}, csr);
            for (int i = 0; i < B.getDimension(0); ++i) {
                for (int j = 0; j < B.getDimension(1); ++j) {
                    B.insert({i,j}, 1.0);
                }
            }
            B.pack();


        }

        void Init()

        virtual void TearDown() {
        }


//        double* _opts;
//        splatt_idx_t _dim;
//        splatt_csf* _tns;
//        splatt_kruskal _fac;
    };

    TEST_F(TensAlgCompTest, SplattCPD) {
        SetUp("../../matrices/impact.mtx");
        /* do the factorization! */
//        int ret = splatt_cpd_als(_tns, _rank, _opts, &_fac);
//
//        /* do some processing */
//        for(splatt_idx_t m = 0; m < _dim; ++m) {
//            /* access factored.lambda and factored.factors[m] */
//        }
    }

    TEST_F(TensAlgCompTest, CPD_ALS) {
        SetUp("./data/tensor/matmul_5-5-5.tns", 10);

        int stop = 1;
    }
}
