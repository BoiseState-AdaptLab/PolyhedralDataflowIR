#include <string>
using std::string;
using std::to_string;
#include <gtest/gtest.h>
using namespace testing;

#include <Eigen/Sparse>
#include <Eigen/SparseExtra>
#include <Eigen/IterativeLinearSolvers>
using Eigen::VectorXd;
using Eigen::SparseMatrix;
using Eigen::ConjugateGradient;
using Eigen::Lower;
using Eigen::Upper;

typedef Eigen::Triplet<double> Triple;

#include <util/MatrixIO.hpp>
using util::MatrixIO;
#include "BenchmarkTest.hpp"

namespace test {
    class ConjGradTest : public BenchmarkTest {

    protected:
        ConjGradTest() {}

        virtual ~ConjGradTest() {}

        virtual void SetUp(initializer_list<string> args) {
            // Read matrix file
            string filename = *args.begin();
            MatrixIO mtx(filename);
            mtx.read();

            _nnz = mtx.nnz();
            _nrow = mtx.nrows();
            _ncol = mtx.ncols();

            _maxiter = 100;
            _niter = _niter_ref = 0;

            // Copy arrays...
            unsigned nbytes = _nnz * sizeof(unsigned);
            _rows = (unsigned*) malloc(nbytes);
            memcpy(_rows, mtx.rows(), nbytes);
            _cols = (unsigned*) malloc(nbytes);
            memcpy(_cols, mtx.cols(), nbytes);

            nbytes = _nnz * sizeof(double);
            _vals = (double*) malloc(nbytes);
            memcpy(_vals, mtx.vals(), nbytes);

            _x = (double*) calloc(_ncol, sizeof(double));
            _b = (double*) calloc(_nrow, sizeof(double));

            // Initialize Eigen objects:
            _cg.setMaxIterations(_maxiter);

            vector<Triple> triples;
            triples.reserve(_nnz);
            for (unsigned n = 0; n < _nnz; n++) {
                triples.push_back(Triple(_rows[n], _cols[n], _vals[n]));
            }

            _Aspm.resize(_nrow, _ncol);
            _Aspm.setFromTriplets(triples.begin(), triples.end());

            _xVec.resize(_ncol);
            _bVec.resize(_nrow);
        }

        virtual void Evaluate() {
            _cg.compute(_Aspm);
            _xVec = _cg.solve(_bVec);
            _niter_ref = _cg.iterations();
            _err_ref = _cg.error();
            // TODO: This is from the Eigen docs, is it needed?
            // Update b, and solve again
            //x = cg.solve(b);
        }

        virtual void Execute() {
            // TODO: Implement me too...
            GTEST_COUT << "ConjGradTest::Execute TBD..." <<  endl;
        }

        virtual void Assert() {
            ASSERT_LT(Compare(_x, _xVec.data(), _ncol), 0);
            ASSERT_LT(Compare(_b, _bVec.data(), _nrow), 0);
            ASSERT_EQ(_niter, _niter_ref);
        }

        virtual void TearDown() {
            free(_rows);
            free(_cols);
            free(_vals);
            free(_x);
            free(_b);
        }

        unsigned _nnz, _nrow, _ncol, _maxiter;
        unsigned _niter, _niter_ref;
        unsigned* _rows;
        unsigned* _cols;

        double _error, _err_ref;
        double* _vals;
        double* _x;
        double* _b;

        // Eigen Objects:
        VectorXd _xVec, _bVec;
        SparseMatrix<double> _Aspm;
        ConjugateGradient<SparseMatrix<double>, Lower|Upper> _cg;
    };

    TEST_F(ConjGradTest, CG) {
        SetUp({"./data/matrix/cant.mtx"});
        Run();
        Verify();
        Assert();
        int stop = 1;
    }
}
