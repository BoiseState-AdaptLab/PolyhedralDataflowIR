#include <string>
using std::string;
using std::to_string;
#include <gtest/gtest.h>
using namespace testing;

#include <taco.h>
using namespace taco;

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
            //Tensor<double> A = read(filename, csr);
            Tensor<double> A({4, 4}, csr);

            Tensor<double> B({A.getDimension(0), A.getDimension(1)}, csr);
            InitMatrix(B, 1.0);

            Tensor<double> C({A.getDimension(0), A.getDimension(1)}, csr);
            InitMatrix(C);

            IndexVar i, j; //, k, l;
            // Hadamard product:
            C(i,j) = A(i,j) * B(i,j);

            C.compile();

            // We can now call the functions taco generated to assemble the indices of the
            // output matrix and then actually compute the MTTKRP.
            C.assemble();
            C.compute();

            // Write the output of the computation to file (stored in the FROSTT format).
            write("C.tns", C);
        }

        void InitMatrix(Tensor<double>& mtx, double val = 0.0) {
            for (int i = 0; i < mtx.getDimension(0); ++i) {
                for (int j = 0; j < mtx.getDimension(1); ++j) {
                    mtx.insert({i,j}, 1.0);
                }
            }
            //mtx.pack();
        }

        virtual void TearDown() {
        }

    };

    TEST_F(TensAlgCompTest, Convert) {
        SetUp("./data/matrix/impact.mtx");

    }
}
