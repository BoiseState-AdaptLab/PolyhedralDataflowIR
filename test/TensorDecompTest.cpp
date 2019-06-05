#include <string>
using std::string;
#include <gtest/gtest.h>
using namespace testing;

#ifdef SPLATT_ENABLED
#include <splatt.h>
#endif

#include <util/MatrixIO.hpp>
using util::MatlabIO;
using util::TensorIO;

namespace {
    class TensorDecompTest : public ::testing::Test {

    protected:
        TensorDecompTest() {}

        virtual ~TensorDecompTest() {}

        void SetUp(const string& filename, const int rank = 10) {
            _rank = rank;
            _tns = TensorIO(filename, rank);
            _tns.read();



//            _opts = splatt_default_opts();      /* allocate default options */
//            /* load the tensor from a file */
//            int ret = splatt_csf_load(filename.c_str(), &_dim, &_tns, _opts);
        }

        virtual void TearDown() {
            /* cleanup */
//            splatt_free_csf(_tns, _opts);
//            splatt_free_kruskal(&_fac);
//            splatt_free_opts(_opts);
        }

        unsigned _rank;
        TensorIO _tns;

//        double* _opts;
//        splatt_idx_t _dim;
//        splatt_csf* _tns;
//        splatt_kruskal _fac;
    };

    TEST_F(TensorDecompTest, SplattCPD) {
        SetUp("../../tensors/matmul_5-5-5.tns", 10);
        /* do the factorization! */
//        int ret = splatt_cpd_als(_tns, _rank, _opts, &_fac);
//
//        /* do some processing */
//        for(splatt_idx_t m = 0; m < _dim; ++m) {
//            /* access factored.lambda and factored.factors[m] */
//        }
    }

    TEST_F(TensorDecompTest, CPD_ALS) {
        SetUp("../tensors/matmul_5-5-5.tns", 10);
        int stop = 1;
    }
}
