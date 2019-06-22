#ifndef _CONJGRADCSRTEST_HPP_
#define _CONJGRADCSRTEST_HPP_

#include "InspExecTest.hpp"
#include "ConjGradTest.hpp"
// Include generated code:
//#include "cg_csr_insp.h"

namespace test {
class ConjGradCSRTest : public InspExecTest, public ConjGradTest {

protected:
    ConjGradCSRTest() : InspExecTest("ConjGradCSRTest") {
    }

    virtual void Inspect() {
        // Run COO->CSR Inspector!
    }
};

TEST_F(ConjGradCSRTest, CG) {
    SetUp({"./data/matrix/cant.mtx"});
    Run();
    Verify();
    Assert();
    int stop = 1;
}
}

#endif // _CONJGRADCSRTEST_HPP_
