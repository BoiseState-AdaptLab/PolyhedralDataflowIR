#ifndef _TENSORDECOMPTEST_HPP_
#define _TENSORDECOMPTEST_HPP_

#include <tuple>
using std::tuple;

#include <util/MatrixIO.hpp>
using util::MatlabIO;
using util::TensorIO;

#include "InspExecTest.hpp"

#include <linalg.h>
#ifdef SPLATT_ENABLED
#include <splatt.h>
#endif

typedef float real;

namespace test {
    class TensorDecompTest : public InspExecTest {

    protected:
        TensorDecompTest(const string& name = "TensorDecompTest") : InspExecTest(name) {
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

            ASSERT_TRUE(_order == 3 || _order == 4);

            unsigned nbytes = _order * _nnz * sizeof(unsigned);
            _indices = (unsigned*) malloc(nbytes);
            memcpy(_indices, tns.indices(), nbytes);

            nbytes = _order * sizeof(unsigned);
            _dims = (unsigned*) malloc(nbytes);
            memcpy(_dims, tns.dims(), nbytes);

            nbytes = _nnz * sizeof(real);
            _vals = (real*) malloc(nbytes);
            double* tvals = tns.vals();
            for (unsigned m = 0; m < _nnz; m++) {
                _vals[m] = tvals[m];
            }
            //memcpy(_vals, tns.vals(), nbytes);

//            fprintf(stderr, "[");
//            for (unsigned m = 0; m < _nnz; m++) {
//                fprintf(stderr, "%g ", _vals[m]);
//            }
//            fprintf(stderr, "]\n");

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
            //cerr << "Leaving TensorDecompTest::SetUp\n";
        }

        virtual void Inspect() {}

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

        virtual void TensorEqual() {
            if (_order == 3) {
                TensorEqual3D();
            } else if (_order == 4) {
                TensorEqual4D();
            }
        }

        virtual void TensorEqual3D() {}

        virtual void TensorEqual4D() {}

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
}

#endif      //_TENSORDECOMPTEST_HPP_
