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

// Include generated code:
#include "conjgrad.h"

namespace test {
    class ConjGradTest : public BenchmarkTest {

    protected:
        ConjGradTest() {
            _name = "ConjGradTest";
        }

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
            _tolerance = 1e-10;
            _error = 1.0;

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
            _cg.setTolerance(_tolerance);

            vector<Triple> triples;
            triples.reserve(_nnz);
            for (unsigned n = 0; n < _nnz; n++) {
                triples.push_back(Triple(_rows[n], _cols[n], _vals[n]));
            }

            _Aspm.resize(_nrow, _ncol);
            _Aspm.setFromTriplets(triples.begin(), triples.end());

            _xVec.resize(_ncol);
            _bVec.resize(_nrow);

            // Initialize b to something more interesting than 0
            for (unsigned i = 0; i < _nrow; i++) {
                _b[i] = (double) (i + 1);
                _bVec(i) = _b[i];
            }
        }

        virtual void Evaluate() {
            _cg.compute(_Aspm);
            _xVec = _cg.solve(_bVec);
            _niter_ref = _cg.iterations();
            _err_ref = _cg.error();
            _x_ref = _xVec.data();
            _b_ref = _bVec.data();
            // TODO: This is from the Eigen docs, is it needed?
            // Update b, and solve again
            //x = cg.solve(b);
        }

        virtual void Execute() {
            for (unsigned t = 0; t < _maxiter && _error > _tolerance; t++) {
                _error = conj_grad(_nrow, _b, _nnz, _rows, _cols, _vals, _x, _x); //, float * d);
            }
        }

        virtual void Assert() {
            vector<double> ref(_ncol);
            copy(_x_ref, _x_ref + _ncol, ref.begin());
            Write("data/matrix/x_ref.csv", ref);
            ref.resize(_nrow);
            copy(_b_ref, _b_ref + _nrow, ref.begin());
            Write("data/matrix/b_ref.csv", ref);

            ASSERT_LT(Compare(_x, _x_ref, _ncol), 0);
            ASSERT_LT(Compare(_b, _b_ref, _nrow), 0);
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

        double _error, _err_ref, _tolerance;
        double* _vals;
        double* _x;
        double* _b;
        const double* _x_ref;
        const double* _b_ref;

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
