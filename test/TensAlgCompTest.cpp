#include <string>
using std::string;
using std::to_string;
#include <gtest/gtest.h>
using namespace testing;

#include <random>
using std::default_random_engine;
using std::uniform_real_distribution;

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

            const Format csr({Dense, Sparse}, {0,1});
            const Format csc({Dense, Sparse}, {1,0});
            const Format dcsr({Sparse, Sparse}, {0,1});
            const Format dcsc({Sparse, Sparse}, {1,0});
            const Format dns({Dense,Dense}, {0,1});
            const Format bcsr({Dense,Sparse,Dense,Dense});

            const ModeFormat::Property ordered = ModeFormat::NOT_ORDERED;
            const ModeFormat::Property unique = ModeFormat::NOT_UNIQUE;
            const Format coo({Compressed({ordered,unique}), Singleton({ordered,unique})}, {0,1});

            // Load a sparse matrix from file (stored in the Matrix Market format) and
            // store it as a compressed sparse row matrix. Matrices correspond to order-2
            // tensors in taco.
            //Tensor<double> A = read(filename, csr);

            const int N = 4;

            Tensor<double> A({N, N}, csr);
            RandMatrix(A);

            Tensor<double> B({N, N}, dns);
            InitMatrix(B, 1.0);

            Tensor<double> C({N, N}, coo);
            InitMatrix(C);

            IndexVar i, j;
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

        void RandMatrix(Tensor<double>& mtx, double val = 0.0) {
            default_random_engine gen(0);
            uniform_real_distribution<double> unif(0.0, 1.0);

            for (int i = 0; i < mtx.getDimension(0); ++i) {
                for (int j = 0; j < mtx.getDimension(1); ++j) {
                    mtx.insert({i,j}, unif(gen));
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
