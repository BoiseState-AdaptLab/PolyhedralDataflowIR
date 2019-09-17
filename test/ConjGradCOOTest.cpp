#include "InspExecTest.hpp"
#include "ConjGradTest.hpp"
// Include generated code:
#include "conjgrad_coo.h"

namespace test {
class ConjGradCOOTest : public ConjGradTest {

protected:
    ConjGradCOOTest() : ConjGradTest("ConjGradCOOTest") {
        _inspFlag = false;          // No inspector needed on this one.
    }

    virtual void Execute() {
        _error = conjgrad_coo(_vals, _b, _nnz, _nrow, _maxiter, _cols, _rows, _x);
        _niter = _maxiter;
    }
};

TEST_F(ConjGradCOOTest, CG) {
    SetUp({"./data/matrix/cant.mtx"});
    //SetUp({"./data/matrix/cg.mtx"});
    //SetUp({"./data/matrix/chesapeake.mtx"});
    Run();
    Verify();
    Assert();
};

}
