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

#include "BenchmarkTest.hpp"

typedef float real;

namespace test {
    class TensorDecompTest : public BenchmarkTest {

    protected:
        TensorDecompTest(const string& name = "TensorDecompTest") : BenchmarkTest(name) {
        }

        virtual ~TensorDecompTest() {}

        void SetUp(const string& filename, const int rank = 10, const int niter = 50) {
            _rank = rank;
            _niter = niter;

            // Read tensor file
            TensorIO tns(filename, rank);
            tns.read();
            _nnz = tns.nnz();
            _order = tns.order();

            unsigned nbytes = _order * _nnz * sizeof(unsigned);
            _indices = (unsigned*) malloc(nbytes);
            memcpy(_indices, tns.indices(), nbytes);

            nbytes = _order * sizeof(unsigned);
            _dims = (unsigned*) malloc(nbytes);
            memcpy(_dims, tns.dims(), nbytes);

            nbytes = _nnz * sizeof(real);
            _vals = (real*) malloc(nbytes);
            memcpy(_vals, tns.vals(), nbytes);

            // Get file prefix
            string prefix = filename;
            size_t pos = prefix.rfind('.');
            if (pos != string::npos) {
                prefix = prefix.substr(0, pos);
            }
            prefix += "_r" + to_string(_rank) + "_";

            // Lambda is an Rx1 vector.
            string lambda_file = prefix + "lambda.mat";
            MatlabIO lam(lambda_file, rank, 1);
            lam.read();

            // Need to do a copy, 'mat' object will go out of scope.
            nbytes = rank * sizeof(real);
            _lambda_ref = (real*) malloc(nbytes);
            memcpy(_lambda_ref, lam.vals(), nbytes);
            _lambda = (real*) malloc(nbytes);

            // Read factor matrices
            _factors = (real**) calloc(_order, sizeof(real*));
            _factors_ref = (real**) calloc(_order, sizeof(real*));
            for (unsigned d = 0; d < _order; d++) {
                unsigned dim = _dims[d];
                string factor_file = prefix + "mode" + to_string(d+1) + ".mat";
                MatlabIO fac(factor_file, dim, rank);
                fac.read();

                nbytes = dim * rank * sizeof(real);
                _factors[d] = (real*) malloc(nbytes);
                _factors_ref[d] = (real*) malloc(nbytes);
                memcpy(_factors_ref[d], fac.vals(), nbytes);
            }
        }

        virtual void Execute() {
            //unsigned seed = 1568224077;
            //cp_als_coo(const float* X, const unsigned I, const unsigned J, const unsigned K, const unsigned M, const unsigned R, const unsigned* ind0, const unsigned* ind1, const unsigned* ind2, float* A, float* B, float* C, float* lmbda)
            cp_als_coo(_vals, _niter, _dims[0], _dims[1], _dims[2], _nnz, _rank, &_indices[0], &_indices[_nnz], &_indices[_nnz * 2], _factors[0], _factors[1], _factors[2], _lambda);
        }

        virtual void Evaluate() {
            // Nothing to do here, maybe later SPLATT or SKT...
        }

        virtual void Assert() {
            ASSERT_LT(Compare<real>(_lambda, _lambda_ref, _rank), 0);
            for (unsigned d = 0; d < _order; d++) {
                ASSERT_LT(Compare<real>(_factors[d], _factors_ref[d], _dims[d] * _rank), 0);
            }
        }

        virtual void TearDown() {
            free(_indices);
            free(_dims);
            free(_vals);
            free(_lambda_ref);
            free(_lambda);

            for (unsigned d = 0; d < _order; d++) {
                free(_factors_ref[d]);
                free(_factors[d]);
            }
            free(_factors_ref);
            free(_factors);

            /* cleanup */
//            splatt_free_csf(_tns, _opts);
//            splatt_free_kruskal(&_fac);
//            splatt_free_opts(_opts);
        }

        unsigned _niter;
        unsigned _nnz;
        unsigned _rank;
        unsigned _order;
        unsigned* _indices;
        unsigned* _dims;

        real* _vals;
        real* _lambda;
        real** _factors;
        real* _lambda_ref;
        real** _factors_ref;

//        double* _opts;
//        splatt_idx_t _dim;
//        splatt_csf* _tns;
//        splatt_kruskal _fac;
    };

    TEST_F(TensorDecompTest, CPD) {
        SetUp("./data/tensor/matmul_5-5-5.tns", 10, 1);
        Run();
        Verify();
        Assert();
    }
}
