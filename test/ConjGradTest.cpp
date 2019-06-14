#include <string>
using std::string;
using std::to_string;
#include <gtest/gtest.h>
using namespace testing;

#ifdef SPLATT_ENABLED
#include <splatt.h>
#endif

#include <util/MatrixIO.hpp>
using util::MatlrixIO;

#include "BenchmarkTest.hpp"

namespace test {
    class ConjGradTest : public BenchmarkTest {

    protected:
        ConjGradTest() {}

        virtual ~ConjGradTest() {}

        virtual void SetUp(initializer_list<string> args) {
            string filename = *args.begin();


            // Read tensor file
            MatrixIO mtx(filename);
            mtx.read();
            _nnz = mtx.nnz();
            _nrow = mtx.nrows();
            _nrow = mtx.ncols();
        }

        virtual void TearDown() {
            free(_indices);
            free(_vals);
        }

        unsigned _nnz, _nrow, _ncol;
        unsigned* _indices;

        real* _vals;
    };

    TEST_F(ConjGradTest, CG) {
        SetUp("./data/matrix/m1.mtx");

        int stop = 1;
    }
}
