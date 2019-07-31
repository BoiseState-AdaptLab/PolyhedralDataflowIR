#ifndef _CONJGRADTEST_HPP_
#define _CONJGRADTEST_HPP_

#include <omp.h>

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

#include <pdfg/Digraph.hpp>
using pdfg::Digraph;
#include <util/MatrixIO.hpp>
using util::MatrixIO;
#include "InspExecTest.hpp"

namespace test {
    class ConjGradTest : public InspExecTest { //BenchmarkTest {
    public:
        static Digraph COOGraph() {
            Digraph cgg("CG_COO");
            cgg.graph_attr("rankdir", "LR");
            cgg.node("r", "", {"shape", "none"});
            cgg.node("i0", "i");
            cgg.node("c", "copy", {"shape","rect"});
            cgg.node("t", "t");
            cgg.node("m", "mset", {"shape","rect"});
            cgg.node("i1", "i");
            cgg.node("n", "n");
            cgg.node("j", "j");
            cgg.node("s", "spmv", {"shape","rect"});
            cgg.node("a", "adiv", {"shape","rect"});
            cgg.node("d", "ddot", {"shape","rect"});
            cgg.node("r0", "rdot0", {"shape","rect"});
            cgg.node("i2", "i");
            cgg.node("x", "xadd", {"shape","rect"});
            cgg.node("r1", "rsub", {"shape","rect"});
            cgg.node("r2", "rdot", {"shape","rect"});
            cgg.node("b", "bdiv", {"shape","rect"});
            cgg.node("i3", "i");
            cgg.node("b1", "bmul", {"shape","rect"});
            cgg.node("d1", "dadd", {"shape","rect"});

            cgg.edge("r", "i0", 0);
            cgg.edge("i0", "c", 0);
            cgg.edge("r", "t", 1);
            cgg.edge("t", "m", 0);
            cgg.edge("t", "n", 1);
            cgg.edge("n", "i1", 0);
            cgg.edge("i1", "j", 0);
            cgg.edge("j", "s", 0);
            cgg.edge("t", "a", 2);
            cgg.edge("i1", "d", 1);
            cgg.edge("i1", "r0", 2);
            cgg.edge("t", "i2", 3);
            cgg.edge("i2", "x", 0);
            cgg.edge("i2", "r1", 1);
            cgg.edge("i2", "r2", 2);
            cgg.edge("t", "b", 4);
            cgg.edge("t", "i3", 5);
            cgg.edge("i3", "b1", 0);
            cgg.edge("i3", "d1", 1);

            return cgg;
        }

        static Digraph CSRGraph() {
            Digraph cgg("CG_CSR");
            cgg.attr("rankdir", "LR");
            cgg.node("r", "", {"shape", "none"});
            cgg.node("i0", "i");
            cgg.node("c", "copy", {"shape","rect"});
            cgg.node("t", "t");
            cgg.node("m", "mset", {"shape","rect"});
            cgg.node("i1", "i");
            cgg.node("n", "n");
            cgg.node("j", "j");
            cgg.node("s", "spmv", {"shape","rect"});
            cgg.node("a", "adiv", {"shape","rect"});
            cgg.node("d", "ddot", {"shape","rect"});
            cgg.node("r0", "rdot0", {"shape","rect"});
            cgg.node("i2", "i");
            cgg.node("x", "xadd", {"shape","rect"});
            cgg.node("r1", "rsub", {"shape","rect"});
            cgg.node("r2", "rdot", {"shape","rect"});
            cgg.node("b", "bdiv", {"shape","rect"});
            cgg.node("i3", "i");
            cgg.node("b1", "bmul", {"shape","rect"});
            cgg.node("d1", "dadd", {"shape","rect"});

            cgg.edge("r", "i0", 0);
            cgg.edge("i0", "c", 0);
            cgg.edge("r", "t", 1);
            cgg.edge("t", "m", 0);
            cgg.edge("t", "i1", 1);
            cgg.edge("i1", "n", 0);
            cgg.edge("n", "j", 0);
            cgg.edge("j", "s", 0);
            cgg.edge("t", "a", 2);
            cgg.edge("i1", "d", 1);
            cgg.edge("i1", "r0", 2);
            cgg.edge("t", "i2", 3);
            cgg.edge("i2", "x", 0);
            cgg.edge("i2", "r1", 1);
            cgg.edge("i2", "r2", 2);
            cgg.edge("t", "b", 4);
            cgg.edge("t", "i3", 5);
            cgg.edge("i3", "b1", 0);
            cgg.edge("i3", "d1", 1);

            return cgg;
        }

    protected:
        ConjGradTest(const string& name = "ConjGradTest") : InspExecTest(name) {
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

            _maxiter = 500;
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

            _x = (double*) calloc(_ncol, sizeof(double));
            _b = (double*) malloc(_nrow * sizeof(double));

            // Initialize b to something more interesting than 0
            for (unsigned i = 0; i < _nrow; i++) {
                _b[i] = 1.0; //(double) (i + 1);
                _bVec(i) = _b[i];
            }
        }

        virtual void Inspect() {}

        virtual void Evaluate() {
            _cg.compute(_Aspm);
            _xVec = _cg.solve(_bVec);
            _niter_ref = _cg.iterations();
            _err_ref = _cg.error();
            _x_ref = _xVec.data();
        }

        virtual void Assert() {
            ASSERT_EQ(_niter, _niter_ref);
            ASSERT_LT(Compare(_x, _x_ref, _ncol), 0);
            ASSERT_LT(Compare(&_error, &_err_ref, 1), 0);
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
        //ConjugateGradient<SparseMatrix<double>> _cg;
        ConjugateGradient<SparseMatrix<double>, Lower|Upper> _cg;
    };
}

#endif // _CONJGRADTEST_HPP_
