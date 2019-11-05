#include <cmath>
#include <gtest/gtest.h>
#include <iostream>
#include <map>
#include <sstream>
#include <pdfg/Codegen.hpp>
#include <pdfg/Digraph.hpp>
#include <pdfg/GraphIL.hpp>
#include <poly/PolyLib.hpp>
//#include <pdfg/FlowGraph.hpp>
//#include <isl/IntSetLib.hpp>
//#include <solve/Z3Lib.hpp>
#include <ConjGradTest.hpp>
using test::ConjGradTest;

using namespace pdfg;
using namespace poly;
using namespace std;

TEST(eDSLTest, Dense) {
    Iter i("i"), j("j");
    Const N("N"), M("M");
    Space dns("Idns", 0 <= i < N ^ 0 <= j < M);
    string result = Codegen().gen(dns);
    string expected = "for(t1 = 0; t1 <= N-1; t1++) {\n  for(t2 = 0; t2 <= M-1; t2++) {\n    s0(t1,t2);\n  }\n}\n";
    //cerr << result << endl;
    ASSERT_EQ(result, expected);
}

TEST(eDSLTest, COO) {
    Iter i("i"), n("n"), j("j");
    Func row("row"), col("col");
    Const NNZ("NNZ");
    Space coo("Icoo", 0 <= n < NNZ ^ i==row(n) ^ j==col(n));
    string result = Codegen().gen(coo);
    string expected = "for(t1 = 0; t1 <= NNZ-1; t1++) {\n"
                      "  t2=row(t1);\n"
                      "  t3=col(t1);\n"
                      "  s0(t1,t2,t3);\n"
                      "}\n";
    //cerr << result << endl;
    ASSERT_EQ(result, expected);
}

TEST(eDSLTest, CSR) {
    Iter i("i"), j("j"), n("n");
    Const N("N"), NNZ("NNZ");
    Func rp("rp"), col("col");
    Space csr("Icsr", 0 <= i < N ^ rp(i) <= n < rp(i+1) ^ j==col(n));
    string result = Codegen().gen(csr);
    string expected = "for(t1 = 0; t1 <= N-1; t1++) {\n  for(t2 = rp(t1); t2 <= rp1(t1)-1; t2++) {\n    t3=col(t1,t2);\n    s0(t1,t2,t3);\n  }\n}\n";
    //cerr << result << endl;
    ASSERT_EQ(result, expected);
}

TEST(eDSLTest, DSR) {
    Iter i("i"), n("n"), j("j"), m("m");
    Func crp("crp"), crow("crow"), col("col");
    Const N("N"), NZR("NZR");
    Space dsr("Idsr", 0 <= m < NZR ^ i==crow(m) ^ crp(m) <= n < crp(m+1) ^ j==col(n));
    string result = Codegen().gen(dsr);
    //cerr << result;
    string expected = "for(t1 = 0; t1 <= NZR-1; t1++) {\n  t2=crow(t1);\n  for(t3 = crp(t1); t3 <= crp1(t1)-1; t3++) {\n    t4=col(t1,t2,t3);\n    s0(t1,t2,t3,t4);\n  }\n}\n";
    ASSERT_EQ(result, expected);
}

TEST(eDSLTest, ELL) {
    Iter i("i"), j("j"), k("k");
    Const N("N"), K("K");
    Func rp("rp"), ecol("ecol");
    Space ell("Iell", 0 <= i < N ^ 0 <= k < K ^ j==ecol(i,k));
    string result = Codegen().gen(ell);
    string expected = "for(t1 = 0; t1 <= N-1; t1++) {\n  for(t2 = 0; t2 <= K-1; t2++) {\n    t3=ecol(t1,t2);\n    s0(t1,t2,t3);\n  }\n}\n";
    //cerr << result << endl;
    ASSERT_EQ(result, expected);
}

TEST(eDSLTest, Lset) {
    Iter l("l"), s("s"), i("i"), n("n"), j("j");
    Func lp("lp"), lrow("lrow"), rp("rp"), col("col");
    Const L("L");
    Space lev("Ilev", 0 <= l < L ^ lp(l) <= s < lp(l+1) ^ i==lrow(s) ^ rp(i) <= n < rp(i+1) ^ j==col(n));
    string result = Codegen().gen(lev);
    string expected = "for(t1 = 0; t1 <= L-1; t1++) {\n  for(t2 = lp(t1); t2 <= lp1(t1)-1; t2++) {\n    t3=lrow(t1,t2);\n    for(t4 = rp(t1,t2,t3); t4 <= rp1(t1,t2,t3)-1; t4++) {\n      t5=col(t1,t2,t3,t4);\n      s0(t1,t2,t3,t4,t5);\n    }\n  }\n}\n";
    //cerr << result << endl;
    ASSERT_EQ(result, expected);
}

TEST(eDSLTest, CSC) {
    Iter i("i"), j("j"), n("n");
    Const M("M"), NNZ("NNZ");
    Func cp("cp"), row("row");
    Space csc("Icsc", 0 <= j < M ^ cp(j) <= n < cp(j+1) ^ i==row(n));
    string result = Codegen().gen(csc);
    string expected = "for(t1 = 0; t1 <= M-1; t1++) {\n  for(t2 = cp(t1); t2 <= cp1(t1)-1; t2++) {\n    t3=row(t1,t2);\n    s0(t1,t2,t3);\n  }\n}\n";
    //cerr << result << endl;
    ASSERT_EQ(result, expected);
}

TEST(eDSLTest, TCOO) {
    Iter i("i"), n("n"), j("j"),  k("k");
    Func ind0("ind0"), ind1("ind1"),ind2("ind2");
    Const NNZ("NNZ");
    Space coo("Icoo", 0 <= n < NNZ ^ i==ind0(n) ^ j==ind1(n) ^ k==ind2(n));
    string result = Codegen().gen(coo);
    string expected = "for(t1 = 0; t1 <= NNZ-1; t1++) {\n  t2=ind0(t1);\n  t3=ind1(t1);\n  t4=ind2(t1);\n  s0(t1,t2,t3,t4);\n}\n";
    //cerr << result << endl;
    ASSERT_EQ(result, expected);
}

TEST(eDSLTest, HiCOO) {
    Iter i("i"), j("j"), k("k"), n("n"), b("b");
    Const B("B", 8), NB("NB");
    //Func bp("bp"), bind("bind", 2), eind("eind", 2);
    Func bp("bp"), bind0("bind0"), bind1("bind1"), bind2("bind2"), eind0("eind0"), eind1("eind1"), eind2("eind2");
    Space hic("Ihic", 0 <= b < NB ^ bp(b) <= n < bp(b+1) ^ i==bind0(b)*B+eind0(n) ^
                      j==bind1(b)*B+eind1(n) ^ k==bind2(b)*B+eind2(n));
    string result = Codegen().gen(hic);
    string expected = "for(t1 = 0; t1 <= NB-1; t1++) {\n  for(t2 = bp(t1); t2 <= bp1(t1)-1; t2++) {\n    t3=eind0(t1,t2)+8*bind0(t1);\n    t4=8*bind1(t1)+eind1(t1,t2);\n    t5=8*bind2(t1)+eind2(t1,t2);\n    s0(t1,t2,t3,t4,t5);\n  }\n}\n";
    //cerr << result << endl;
    ASSERT_EQ(result, expected);
}

TEST(eDSLTest, CSF) {
    Iter i("i"), j("j"), k("k"), p("p"), m("m"), n("n");
    Func ind0("ind0"), ind1("ind1"), ind2("ind2");
    Func pos0("pos0"), pos1("pos1");
    Const NZF("NZF");
    Space csf("Icsf", 0 <= p < NZF ^ i==ind0(p) ^ pos0(p) <= m < pos0(p+1) ^ j==ind1(m) ^ pos1(m) <= n < pos1(m+1) ^ k==ind2(n));
    string result = Codegen().gen(csf);
    string expected = "for(t1 = 0; t1 <= NZF-1; t1++) {\n  t2=ind0(t1);\n  for(t3 = pos0(t1); t3 <= pos0_1(t1)-1; t3++) {\n    t4=ind1(t1,t2,t3);\n    for(t5 = pos1(t1,t2,t3); t5 <= pos1_1(t1,t2,t3)-1; t5++) {\n      t6=ind2(t1,t2,t3,t4,t5);\n      s0(t1,t2,t3,t4,t5,t6);\n    }\n  }\n}\n";
    ASSERT_EQ(result, expected);
}

TEST(eDSLTest, DSR_SpMV) {
    Iter i("i"), n("n"), j("j"), m("m");
    Func crp("crp"), crow("crow"), col("col");
    Const N("N"), M("M"), NZR("NZR"), NNZ("NNZ");
    //Space dsr("spmv", 0 <= m < NZR ^ i==crow(m) ^ crp(m) <= n < crp(m+1) ^ 0 <= n < NNZ ^ j==col(n));
    Space dsr("spmv", 0 <= m < NZR ^ i==crow(m) ^ crp(m) <= n < crp(m+1) ^ j==col(n));

    string name = "dsr_spmv";
    init(name);
    Space val("val", NNZ), x("x", M), y("y", N);
    Comp spmv = dsr + (y[i] += val[n] * x[j]);
    //cerr << codegen() << endl;

    string result = stringify<Comp>(spmv);
    string expected = "spmv(m,i,n,j) = { 0 <= m ^ m < NZR ^ 0 < NZR ^ i = crow(m) ^ crp(m) <= n ^ n < crp(m+1) ^ crp(m) < crp(m+1) ^ j = col(n) } : { y[i]+=val[n]*x[j] }";
    ASSERT_EQ(result, expected);

    result = codegen(name);
    //cerr << result << endl;
    expected = "#include <stdio.h>\n"
               "#include <stdlib.h>\n"
               "#include <stdint.h>\n"
               "#include <sys/time.h>\n\n"
               "#define min(x,y) (((x)<(y))?(x):(y))\n"
               "#define max(x,y) (((x)>(y))?(x):(y))\n"
               "#define floord(x,y) ((x)/(y))\n"
               "#define offset2(i,j,M) ((j)+(i)*(M))\n"
               "#define offset3(i,j,k,M,N) ((k)+((j)+(i)*(M))*(N))\n"
               "#define offset4(i,j,k,l,M,N,P) ((l)+((k)+((j)+(i)*(M))*(N))*(P))\n"
               "#define arrinit(ptr,val,size) for(unsigned __i__=0;__i__<(size);__i__++) (ptr)[__i__]=(val)\n\n"
               "void dsr_spmv(const unsigned NZR, const unsigned* crow, const unsigned* crp, const unsigned* col, const float* val, const float* x, float* y);\n"
               "inline void dsr_spmv(const unsigned NZR, const unsigned* crow, const unsigned* crp, const unsigned* col, const float* val, const float* x, float* y) {\n"
               "    unsigned t1,t2,t3,t4,t5;\n"
               "// spmv\n"
               "#define col(m,i,n) col[(n)]\n"
               "#define crow(m) crow[(m)]\n"
               "#define crp(m) crp[(m)]\n"
               "#define crp1(m) crp[(m+1)]\n\n"
               "#undef s0\n"
               "#define s0(m,i,n,j) y[(i)]+=val[(n)]*x[(j)]\n\n"
               "for(t1 = 0; t1 <= NZR-1; t1++) {\n"
               "  t2=crow(t1);\n"
               "  for(t3 = crp(t1); t3 <= crp1(t1)-1; t3++) {\n"
               "    t4=col(t1,t2,t3);\n"
               "    s0(t1,t2,t3,t4);\n"
               "  }\n"
               "}\n"
               "\n"
               "}    // dsr_spmv\n";
    ASSERT_EQ(result, expected);
}

TEST(eDSLTest, COO_MTTKRP) {
    Iter n('n'), i('i'), j('j'),  k('k'), r('r');
    Func ind0("ind0"), ind1("ind1"),ind2("ind2");
    Const M("M"), I("I"), J("J"), K("K"), R("R");
    Space coo("Icoo", 0 <= n < M ^ i==ind0(n) ^ j==ind1(n) ^ k==ind2(n) ^ 0 <= r < R);
    Space A("A",I,R), X("X",M), C("C",K,R), B("B",J,R);

    string name = "mttkrp_coo";
    pdfg::init(name);
    Comp mttkrp("krp", coo, (A(i,r) += X(n) * C(k,r) * B(j,r)));
    pdfg::print("out/" + name + ".json");
    string result = pdfg::codegen("out/" + name + ".h");

    //cerr << result << endl;
    string expected = "#define ind0(n) ind0[(n)]\n#define ind1(n) ind1[(n)]\n#define ind2(n) ind2[(n)]\n\n#undef s0\n#define s0(n,i,j,k,l) A[offset2((i),(j),(J))]+=B[(n)]*C[offset2((k),(j),(J))]*D[offset2((l),(j),(J))]\n\nunsigned t1,t2,t3,t4,t5;\n#pragma omp parallel for schedule(auto) private(t1,t2,t3,t4,t5)\nfor(t1 = 0; t1 <= NNZ-1; t1++) {\n  t2=ind0(t1);\n  t3=ind1(t1);\n  t4=ind2(t1);\n  for(t5 = 0; t5 <= L-1; t5++) {\n    s0(t1,t2,t3,t4,t5);\n  }\n}\n";
    ASSERT_EQ(result, expected);
}

TEST(eDSLTest, CSF_MTTKRP) {
    Iter i("i"), j("j"), k("k"), m("m"), n("n"), p("p"), q("q"), r("r");
    Const F("F"), M("M"), I("I"), J("J"), K("K"), R("R");
    //Func ind("ind", 2), pos("pos", 2);
    Func ind0("ind0"), ind1("ind1"), ind2("ind2"), pos0("pos0"), pos1("pos1");
    Space csf("Icsf", 0 <= p < F ^ i==ind0(p) ^ pos0(p) <= q < pos0(p+1) ^ j==ind1(q) ^
                      pos1(q) <= n < pos1(q+1) ^ k==ind2(n) ^ 0 <= r < R);
    Space A("A",I,R), X("X",M), B("B",J,R), C("C",K,R);

    string name = "mttkrp_csf";
    pdfg::init(name, "", "f", "u", {"A"}, "0");
    Comp mttkrp("krp", csf, (A(i,r) += X(n) * C(k,r) * B(j,r)));
    pdfg::parallelize();
    pdfg::print("out/" + name + ".json");
    string result = pdfg::codegen("out/" + name + ".h");

    //cerr << result << endl;
    string expected = "#define ind0(p) ind0[(p)]\n#define ind1(p,i,m) ind1[(m)]\n#define ind2(p,i,m,j,n) ind2[(n)]\n#define pos0(p) pos0[(p)]\n#define pos0_1(p) pos0[(p+1)]\n#define pos1(p,i,m) pos1[(m)]\n#define pos1_1(p,i,m) pos1[(m+1)]\n\n#undef s0\n#define s0(p,i,m,j,n,k,l) A[offset2((i),(j),(J))]+=B[(n)]*C[offset2((k),(j),(J))]*D[offset2((l),(j),(J))]\n\nunsigned t1,t2,t3,t4,t5,t6,t7;\n#pragma omp parallel for schedule(auto) private(t1,t2,t3,t4,t5,t6,t7)\nfor(t1 = 0; t1 <= NZF-1; t1++) {\n  t2=ind0(t1);\n  for(t3 = pos0(t1); t3 <= pos0_1(t1)-1; t3++) {\n    t4=ind1(t1,t2,t3);\n    for(t5 = pos1(t1,t2,t3); t5 <= pos1_1(t1,t2,t3)-1; t5++) {\n      t6=ind2(t1,t2,t3,t4,t5);\n      for(t7 = 0; t7 <= L-1; t7++) {\n        s0(t1,t2,t3,t4,t5,t6,t7);\n      }\n    }\n  }\n}\n";
    ASSERT_EQ(result, expected);
}

TEST(eDSLTest, COO_CSR_Insp) {
    Iter i("i"), n("n"), j("j");
    Func rp("rp"), row("row"), col("col");
    Const N("N"), NNZ("NNZ");

    // COO->CSR Inspector:
    Space insp1("I_N", 0 <= n < NNZ ^ i==row(n));
    Comp inspN = insp1 + (i >= N) + (N=i+1);
    string result = Codegen("").gen(inspN);
//    cerr << inspN << endl;
//    cerr << result << endl;
    string expected = "#define row(n) row[(n)]\n\n#undef s0\n#define s0(n,i) if ((i) >= N) N=(i)+1\n\nunsigned t1,t2;\nfor(t1 = 0; t1 <= NNZ-1; t1++) {\n  t2=row(t1);\n  s0(t1,t2);\n}\n";
    ASSERT_EQ(result, expected);

    Space insp2("I_rp", 0 <= n < NNZ ^ i==row(n) ^ n >= rp(i+1));
    Comp insp_rp = insp2 + (rp(i+1) = n+1);
    result = Codegen("").gen(insp_rp);
//    cerr << insp_rp << endl;
//    cerr << result << endl;
    expected = "#define row(n) row[(n)]\n"
               "#define rp(n,i) rp[(i+1)]\n\n"
               "#undef s0\n"
               "#define s0(n,i) rp((i)+1)=(n)+1\n\n"
               "unsigned t1,t2;\n"
               "for(t1 = 0; t1 <= NNZ-1; t1++) {\n"
               "  t2=row(t1);\n"
               "  if (t1 >= rp(t1,t2)) {\n"
               "    s0(t1,t2);\n"
               "  }\n"
               "}\n";

    ASSERT_EQ(result, expected);
}

TEST(eDSLTest, COO_CSR_Insp_Opt) {
    Iter i("i"), n("n"), j("j"), m("m");
    Func rp("rp"), row("row"), col("col");
    Const N("N"), NNZ("NNZ");

    // Here we add the constraint: row(i-1) <= row(i), 1 <= i < N (i.e., row is sorted)

    // COO->CSR Inspector:
    Space insp1("I_N"); //, 0 <= n < NNZ ^ i==row(n) ^ i >= N);
    //Math expr = (N=row(NNZ-1));
    Comp inspN = insp1 + (N=row(NNZ-1)+1);
    string result = Codegen("").gen(inspN);
    cerr << inspN << endl;
    cerr << result << endl;
    string expected = "#undef s0\n"
                      "#define s0() N=row(NNZ-1)+1\n\n"
                      "s0();\n";
    ASSERT_EQ(result, expected);

    Space insp2("I_rp", 0 <= n < NNZ ^ i==row(n) ^ n >= rp(i+1));
    Comp insp_rp = insp2 + (rp(i+1) = n+1);
    result = Codegen("").gen(insp_rp);
    cerr << insp_rp << endl;
    cerr << result << endl;
    expected = "#define row(n) row[(n)]\n"
               "#define rp(n,i) rp[(i+1)]\n\n"
               "#undef s0\n"
               "#define s0(n,i) rp((i)+1)=(n)+1\n\n"
               "unsigned t1,t2;\n"
               "for(t1 = 0; t1 <= NNZ-1; t1++) {\n"
               "  t2=row(t1);\n"
               "  if (t1 >= rp(t1,t2)) {\n"
               "    s0(t1,t2);\n"
               "  }\n"
               "}\n";

    ASSERT_EQ(result, expected);
}

TEST(eDSLTest, Jacobi2D) {
    Iter t('t'), i('i'), j('j');
    Const T('T'), N('N'),  M('M');
    Space jac("jac", 1 <= t <= T ^ 1 <= i <= N ^ 1 <= j <= N);
    Space A("A", T, N+2, M+2);
    init("jacobi2d");
    Comp stencil = jac + (A(t,i,j) = paren(A(t-1,i,j) + A(t-1,i,j-1) + A(t-1,i,j+1) + A(t-1,i+1,j) + A(t-1,i-1,j)) * 0.2);
    print("out/jacobi2d.json");
    string result = codegen("out/jacobi2d.o");
    //cerr << result << endl;
    ASSERT_TRUE(!result.empty());
}

TEST(eDSLTest, GaussSeidel) {
    Iter t('t'), i('i'), j('j');
    Const T('T'), N('N'),  M('M');
    Space gs("jac", 1 <= t <= T ^ 1 <= i <= N ^ 1 <= j <= N);
    Space A("A", T, N+2, M+2);
    string name = "gs_rb";
    pdfg::init(name);
    auto expr = (A(t,i,j) = paren(A(t-1,i,j) + A(t,i,j-1) + A(t-1,i,j+1) + A(t-1,i+1,j) + A(t,i-1,j)) * 0.2);
    Comp stencil_r("st_r", gs, ((i+j) % 2 == 0), expr);
    Comp stencil_b("st_b", gs, ((i+j) % 2 == 1), expr);
    pdfg::fuse({"st_r", "st_b"});
    pdfg::print("out/" + name + ".json");
    string result = pdfg::codegen("out/" + name + ".h");
    //cerr << result << endl;
    ASSERT_TRUE(!result.empty());
}

TEST(eDSLTest, COO_CSR_Insp_Fuse) {
    // COO->CSR Inspector:
    // Here we assume the constraint: row(i-1) <= row(i), 1 <= i < N (i.e., row is sorted)
    Iter i('i'), n('n'), j('j'), m('m');
    Func rp("rp"), row("row"), col("col");
    Const N("N"), NNZ("NNZ");
    Space insp1("I_N");
    Space insp2("Irp", 0 <= n < NNZ ^ i==row(n));
    Space insp3("Irp2", 0 <= i < N);

    pdfg::init("coo_csr_insp", "N", "d", "u", {"rp"});
    Comp inspN("inspN", insp1, (N=row(NNZ-1)+1));
    Comp insp_rp("insp_rp", insp2, (n >= rp(i+1)), (rp(i+1) = n+1));
    Comp insp_rp2("insp_rp2", insp3, (rp(i) > rp(i+1)), (rp(i+1) = rp(i)+0));

    //pdfg::fuse(insp_rp, insp_rp2);
    print("out/coo_csr_insp.json");
    string result = codegen("out/coo_csr_insp.h", "", "C++", "simd");
    ASSERT_TRUE(!result.empty());
}

TEST(eDSLTest, COO_ELL_Insp) {
    // COO->ELL Inspector:
    // Here we assume the constraint: row(i-1) <= row(i), 1 <= i < N (i.e., row is sorted)
    Iter i('i'), n('n'), j('j'), m('m');
    Func rp("rp"), row("row"), col("col"), val("val"), lcol("lcol"), lval("lval");
    Const N('N'), M('M'), K('K');
    Space k("k"), p("p");
    Space insp1("I_N");
    Space insp2("Irp", 0 <= n < M ^ i==row(n));
    Int zero(0), one(1);

    string name = "coo_ell_insp";
    pdfg::init(name, "K", "d", "u", {"lcol", "lval"}, "0");
    Comp inspN("inspN", insp1, (N=row(M-1)+1));
    Comp inspK("inspK", insp2, (i > p), (K=max(k+1,K)) ^ (k=zero+0));
    Comp insp_lc("insp_lc", insp2, (lcol(k,i)=col(n)+0));
    Comp insp_lv("insp_lv", insp2, (lval(k,i)=val(n)+0));
    Comp insp_k("insp_k", insp2, (k += one));
    Comp insp_p("insp_p", insp2, (p = i + 0));

    pdfg::fuse();
    print("out/" + name + ".json");
    string result = codegen("out/" + name + ".h", "", "C++");
    ASSERT_TRUE(!result.empty());
}

TEST(eDSLTest, COO_CSB_Insp) {
    Iter i('i'), j('j'), k('k'), n('n'), b('b'), m('m');
    Const B("B", 128), NB("NB"), N('N'), M('M'), p('p', 0);

    Func bp("bp"), brow("brow"), bcol("bcol"), erow("erow"), ecol("ecol");
    Func bid("bid", 2), bsize("bsize", 1), bmap("bmap", 2);
    Func row("row"), col("col");

    // Iteration spaces:
    Space insp1("insp1", 0 <= n < M ^ i==row(n) ^ j==col(n));
    Space insp2("insp2", 0 <= b < NB ^ 0 <= m < bsize(b) ^ n==bmap({b,m}));
    Space exec("exec", 0 <= b < NB ^ bp(b) <= n < bp(b+1) ^ i==brow(b)*B+erow(n) ^ j==bcol(b)*B+ecol(n));

    // Data spaces:
    Space val("val", M), bval("bval", M);

    string name = "coo_csb_insp";
    pdfg::init(name, "NB", "f", "u", {"bp", "brow", "bcol", "erow", "ecol"}, "0");

    Comp bs_put("bs_put", insp1, b=bid(i/B,j/B)+0);
    Comp br_put("br_put", insp1, (brow(b)=i/B));
    Comp bc_put("bc_put", insp1, (bcol(b)=j/B));
    Comp nb_cnt("nb_cnt", insp1, (b >= NB), (NB=b+1));

    Comp bp_put("bp_put", insp2, (p >= bp(b+1)), (bp(b+1)=p+1));
    Comp er_ut("er_put", insp2, (erow(p)=row(n)-B*brow(b)));
    Comp ec_put("ec_put", insp2, (ecol(p)=col(n)-B*bcol(b)));
    Comp bv_put("bv_put", insp2, (bval(p)=val(n)+0));
    Comp p_inc("p_inc", insp2, (p += 1));

    pdfg::fuse({"bs_put", "br_put", "bc_put", "nb_cnt"});
    pdfg::fuse({"bp_put", "er_put", "ec_put", "bv_put", "p_inc"});

    // TODO:
    //  1) Implement dynamic data spaces.

    pdfg::print("out/" + name + ".json");
    string result = pdfg::codegen("out/" + name + ".h", "", "C++", "auto");
    ASSERT_TRUE(!result.empty());
}

TEST(eDSLTest, COO_DSR_Insp) {
    // COO->DSR Inspector:
    // Here we assume the constraint: row(i-1) <= row(i), 1 <= i < N (i.e., row is sorted)
    Const N("N"), M("M"), R("R");
    Iter i('i'), n('n'), j('j'), m('m');
    Space row("row"), col("col");
    Space crp("crp"), crow("crow");
    Space insp1("I_R");
    Space insp2("Icrp", 1 <= n < M ^ i==row(n));

    string name = "coo_dsr_insp";
    pdfg::init(name, "R", "d", "u", {"crp", "crow"}, "0");
    Comp insp_init("insp_init", insp1, ((crow(0) = row(0) + 0) ^ (R = crp(0) = crp(1) = Int(0) + 0)));
    Comp insp_m("insp_m", insp2, (i != row(n-1)), (R += 1));
    Comp insp_crow("insp_crow", insp2, (crow(R) = i + 0));
    Comp insp_crp("insp_crp", insp2, (n >= crp(R+1)), (crp(R+1) = n+1));
    Comp insp_R("insp_R", insp1, (R+=1));

    pdfg::fuse({"insp_m", "insp_crow", "insp_crp"});
    pdfg::print("out/" + name + ".json");
    string result = pdfg::codegen("out/" + name + ".h", "", "C++");
    ASSERT_TRUE(!result.empty());
}

TEST(eDSLTest, COO_DIA_Insp) {
    // COO->DIA Inspector:
    // Here we assume the constraint: row(i-1) <= row(i), 1 <= i < N (i.e., row is sorted)
    Iter i('i'), n('n'), j('j'), d('d');
    Func rp("rp"), row("row"), col("col"), did("did", 2), doff("doff");
    Const N('N'), M('M'), D('D');
    Space k("k"), p("p");
    Space insp1("I_N");
    Space insp2("Icoo", 0 <= n < M ^ i==row(n) ^ j==col(n));
    Space val("val"), dval("dval");
    Int zero(0), one(1);

    string name = "coo_dia_insp";
    pdfg::init(name, "D", "d", "i", {"doff", "dval"}, "0");
    Comp inspN("inspN", insp1, (N = row(M-1)+1));
    Comp inspD1("inspD1", insp2, (d = did(i,j)+0));
    Comp inspD2("inspD2", insp2, (d >= D), (D = d+1));
    Comp inspD3("inspD3", insp2, (doff(d) = j - i));
    Comp inspD4("inspD4", insp2, (dval(d,i) = val(n)+0));

    pdfg::fuse();
    print("out/" + name + ".json");
    string result = codegen("out/" + name + ".h", "", "C++");
    ASSERT_TRUE(!result.empty());
}

TEST(eDSLTest, CSR_ELL_Insp) {
    Iter i('i'), j('j'), k('k'), n('n');
    Const N("N"), NNZ("NNZ"), K("K");
    Func rp("rp"), row("row"), col("col"), ecol("ecol");
    Space csr("Icsr", 0 <= i < N ^ rp(i) <= n < rp(i+1) ^ j==col(n));
    Space ell("Iell", 0 <= i < N ^ 0 <= k < K ^ j==ecol(i,k));

    // CSR->COO Inspector:
    string name = "csr_ell_insp";
    pdfg::init(name);
    Space insp("kmax", 0 <= i < N ^ rp(i) <= n < rp(i+1) ^ k==n-rp(i) ^ j==col(n));
    Comp kmax = insp + (K=max(k,K));
    Math asn = (ecol(i,k)=j+0);
    kmax += asn;
    cerr << kmax << endl;

    pdfg::print("out/" + name + ".json");
    string result = pdfg::codegen("out/" + name + ".h", "", "C++", "auto");
    string expected = "for(t1 = 0; t1 <= N-1; t1++) {\n  for(t2 = 0; t2 <= K-1; t2++) {\n    t3=ecol(t1,t2);\n    s0(t1,t2,t3);\n  }\n}\n";
    //cerr << result << endl;
    ASSERT_EQ(result, expected);
}

TEST(eDSLTest, CSR_BSR_Insp) {
    Iter i('i'), j('j'), n('n');
    Const N("N"), M("M"), NNZ("NNZ"), NB("NB");
    Const R("R", 8), C("C", 8);
    Iter b("b"), ii("ii"), jj("jj"), ri("ri"), cj("cj");
    Func rp("rp"), col("col"), bp("bp"), bcol("bcol"); //, min("min", 2);
    Space icsr("Icsr", 0 <= i < N ^ rp(i) <= n < rp(i+1) ^ j==col(n));
    Space val("val", NNZ), x("x", N), y("y", N), bval("bval"); //, NB, R, C);

    string name = "csr_bsr_insp";
    pdfg::init(name, "NB", "d", "u", {"bp", "bcol"});
//    Comp csr = icsr + (y[i] += val[n] * x[j]);
//    cerr << csr << endl;

    Space ibsr("Ibsr", 0 <= ii < N/R ^ bp(ii) <= b < bp(ii+1) ^ jj==bcol(b) ^ 0 <= ri < R ^ 0 <= cj < C ^ i==ii*R+ri ^ j==jj*C+cj);
    Comp bsr = ibsr + (y[i] += bval(b,ri,cj) * x[j]);
    cerr << bsr << endl;
    string result = pdfg::codegen("out/" + name + ".h", "0", "C++", "simd");
    cerr << result << endl;

    Comp count("count", icsr, (NB += 1));
    pdfg::tile({"count"}, {"i"}, {R.val()});
    //result = count.tile(i, R, ii);
    //Space insp("Iin", 0 <= ii < N/R ^ ii*R <= i < R*ii+R ^ rp(i) <= n < rp(i+1) ^ j==col(n));
    cerr << result << endl;
    //result = Codegen("").gen(result);
    cerr << result << endl;

    // make-dense 1st:
//    result = csr.make_dense(0 <= j < M);
//    cerr << result << endl;
//    result = csr.tile(i, j, R, C, ii, jj);
//    cerr << result << endl;

    pdfg::fuse();
    pdfg::print("out/" + name + ".json");
    result = pdfg::codegen("out/" + name + ".h", "", "C++", "simd");
    ASSERT_TRUE(!result.empty());
}

TEST(eDSLTest, MFD_2D) {
    Iter x('x'), y('y'), c('c');
    Const N('N'), C('C', 4);
    ConstrTuple fx = (0 <= c < C ^ 0 <= y < N ^ 0 <= x <= N);
    ConstrTuple fy = (0 <= c < C ^ 0 <= y <= N ^ 0 <= x < N);
    ConstrTuple df = (0 <= c < C ^ 0 <= y < N ^ 0 <= x < N);
    ConstrTuple box = (0 <= c < C ^ 0 <= y <= N+1 ^ 0 <= x <= N+1);

    Space bin("bin", box);
    Space bout("bout", box);
    Space cx1("cx1", fx), cx2("cx2", fx);
    Space cy1("cy1", fy), cy2("cy2", fy);

    string name = "mfd";
    init(name, "", "float", "", {"bout"}); //, to_string(0));

    Comp fx1("fx1", fx, (cx1(c,y,x) = (1./12.)*paren(bin(c,y,x-2) + 7.0 * paren(bin(c,y,x-1) + bin(c,y,x)) + bin(c,y,x+1))));
    Comp fx2("fx2", fx, (cx2(c,y,x) = cx1(c,y,x) * 2.0 * cx1(2,y,x)));
    Comp dx("dx", df, (bout(c,y,x) += cx2(c,y,x+1) - cx2(c,y,x)));

    Comp fy1("fy1", fy, (cy1(c,y,x) = (1./12.)*paren(bin(c,y-2,x) + 7.0 * paren(bin(c,y-1,x) + bin(c,y,x)) + bin(c,y+1,x))));
    Comp fy2("fy2", fy, (cy2(c,y,x) = cy1(c,y,x) * 2.0 * cy1(3,y,x)));
    Comp dy("dy", df, (bout(c,y,x) += cy2(c,y+1,x) - cy2(c,y,x)));

    print("out/" + name + ".json");
    string result = codegen("out/" + name + "_2d.h", "", "C++", "auto");
    //cerr << result << endl;
    ASSERT_TRUE(!result.empty());
}

TEST(eDSLTest, MFD_3D) {
    Iter x('x'), y('y'), z('z'), c('c');
    Const B('B'), N('N'), C('C', 5);
    ConstrTuple fx = (0 <= c < C ^ 0 <= z < N ^ 0 <= y < N ^ 0 <= x <= N);
    ConstrTuple fy = (0 <= c < C ^ 0 <= z < N ^ 0 <= y <= N ^ 0 <= x < N);
    ConstrTuple fz = (0 <= c < C ^ 0 <= z <= N ^ 0 <= y < N ^ 0 <= x < N);
    ConstrTuple df = (0 <= c < C ^ 0 <= z < N ^ 0 <= y < N ^ 0 <= x < N);
    ConstrTuple box = (0 <= c < C ^ -2 <= z < N+2 ^ -2 <= y < N+2 ^ -2 <= x < N+2);

    Space bin("bin", box);
    Space bout("bout", box);
    Space cx1("cx1", fx), cx2("cx2", fx);
    Space cy1("cy1", fy), cy2("cy2", fy);
    Space cz1("cz1", fz), cz2("cz2", fz);

    string name = "mfd";
    init(name, "", "d", "", {"bout"}); //, to_string(0));

    Math fac1 = paren(Real(1.0) / Real(12.0));
    Comp fx1("fx1", fx, (cx1(c,z,y,x) = fac1*paren(bin(c,z,y,x-2) + 7.0 * paren(bin(c,z,y,x-1) + bin(c,z,y,x)) + bin(c,z,y,x+1))));
    Comp fx2("fx2", fx, (cx2(c,z,y,x) = cx1(c,z,y,x) * 2.0 * cx1(2,z,y,x)));
    Comp dx("dx", df, (bout(c,z,y,x) += paren(cx2(c,z,y,x+1) - cx2(c,z,y,x))));

    Comp fy1("fy1", fy, (cy1(c,z,y,x) = fac1*paren(bin(c,z,y-2,x) + 7.0 * paren(bin(c,z,y-1,x) + bin(c,z,y,x)) + bin(c,z,y+1,x))));
    Comp fy2("fy2", fy, (cy2(c,z,y,x) = cy1(c,z,y,x) * 2.0 * cy1(3,z,y,x)));
    Comp dy("dy", df, (bout(c,z,y,x) += paren(cy2(c,z,y+1,x) - cy2(c,z,y,x))));

    Comp fz1("fz1", fz, (cz1(c,z,y,x) = fac1*paren(bin(c,z-2,y,x) + 7.0 * paren(bin(c,z-1,y,x) + bin(c,z,y,x)) + bin(c,z+1,y,x))));
    Comp fz2("fz2", fz, (cz2(c,z,y,x) = cz1(c,z,y,x) * 2.0 * cz1(4,z,y,x)));
    Comp dz("dz", df, (bout(c,z,y,x) += paren(cz2(c,z+1,y,x) - cz2(c,z,y,x))));

    fuse();
    //tile({"z", "y", "x"}, {8, 8, 8});
    print("out/" + name + ".json");
    string result = codegen("out/" + name + "_3d.h", "", "C++", "auto");
    //cerr << result << endl;
    ASSERT_TRUE(!result.empty());
}

TEST(eDSLTest, Blur) {
    Iter x('x'), y('y'), c('c');
    Const R('R'), C('C');
    ConstrTuple constrs = (0 <= c < Int(3) ^ 1 <= x <= R ^ 1 <= y <= C);
    Space rng("rng", constrs), img("img", constrs), blurx("blurx", constrs), blury("blury", constrs);

    string name = "blur";
    init(name, "", "float"); //, "", {"d", "r"}, to_string(0));
    Comp bx("bx", rng, (blurx(c,x,y) = (img(c,x-1,y) + img(c,x,y) + img(c,x+1,y)) * 0.33333333));
    Comp by("by", rng, (blury(c,x,y) = (blurx(c,x,y-1) + img(c,x,y) + img(c,x,y+1)) * 0.33333333));
    //fuse();

    print("out/" + name + ".json");
    string result = codegen("out/" + name + ".o", "", "C++", "auto");
    //cerr << result << endl;
    ASSERT_TRUE(!result.empty());
}

TEST(eDSLTest, ConjGrad) {
    Iter t('t'), i('i'), j('j'), n('n');
    Const N('N'), M('M'), K('K');   // N=#rows/cols, M=#nnz, K=#iterations
    Func rp("rp"), row("row"), col("col");

    // Iteration spaces:
    Space sca("sca");
    Space vec("vec", 0 <= i < N);
    Space csr("csr", 0 <= i < N ^ rp(i) <= n < rp(i+1) ^ j==col(n));
    //Space coo("coo", 0 <= n < M ^ i==row(n) ^ j==col(n));
    //Space spv("spv", 0 <= n < M ^ i==row(n));   // Sparse vector, to enable fusion of dot products with SpMV.
    //Space mtx = coo;
    Space mtx = csr;

    // Data spaces:
    Space A("A", M), x("x", N), b("b", N), r("r", N), s("s", N), d("d", N);
    Space alpha("alpha"), beta("beta"), ds("ds"), rs("rs"), rs0("rs0");

    string name = "conjgrad_csr";
    //string name = "conjgrad_coo";
    init(name, "rs", "d", "", {"d", "r"}, to_string(0));

    //Comp copy("copy", vec, ((r[i]=b[i]+0) ^ (d[i]=b[i]+0)));
    Comp spmv("spmv", mtx, (s[i] += A[n] * d[j]));
    Comp ddot("ddot", vec, (ds += d[i]*s[i]));
    //Comp ddot("ddot", spv, (ds += d[i]*s[i]));
    Comp rdot0("rdot0", vec, (rs0 += r[i]*r[i]));
    //Comp rdot0("rdot0", spv, (rs0 += r[i]*r[i]));
    Comp adiv("adiv", sca, (alpha = rs0/ds));
    Comp xadd("xadd", vec, (x[i] += alpha * d[i]));
    Comp rsub("rsub", vec, (r[i] -= alpha*s[i]));
    Comp rdot("rdot", vec, (rs += r[i]*r[i]));
    Comp bdiv("bdiv", sca, (beta = rs / rs0));
    Comp dadd("dadd", vec, (d[i] = r[i] + beta * d[i]));     // FMA: a+b*c

    // Perform fusions
    fuse("spmv", "ddot", "rdot0");
    fuse("xadd", "rsub", "rdot");
    fuse("bmul", "dadd");

    perfmodel();        // perfmodel annotates graph with performance attributes.
    print("out/" + name + ".json");
    string result = codegen("out/" + name + ".o", "", "C++", "auto");
    //cerr << result << endl;
    ASSERT_TRUE(!result.empty());
}

TEST(eDSLTest, ConjGradCOO) {
    Iter t('t'), i('i'), j('j'), n('n');
    Const T('T'), N('N'), M('M'), K('K');   // N=#rows/cols, M=#nnz, K=#iterations
    Func row("row"), col("col");
    Real zero(0.0);

    // Iteration spaces:
    Space cpy("cpy", 0 <= i < N);
    Space sca("sca", 1 <= t <= T);
    Space vec("vec", 1 <= t <= T ^ 0 <= i < N);
    Space coo("coo", 1 <= t <= T ^ 0 <= n < M ^ i==row(n) ^ j==col(n));
    //Space spv("spv", 1 <= t <= T ^ 0 <= n < M ^ i==row(n));  // Sparse vector enables fusing dot products with SpMV.
    Space mtx = coo;

    // Data spaces:
    Space A("A", M), x("x", N), b("b", N), r("r", N), s("s", N), d("d", N);
    Space alpha("alpha"), beta("beta"), ds("ds"), rs("rs"), rs0("rs0"), tol("tol", 1E-6);

    string name = "conjgrad_coo";
    init(name, "rs", "d", "", {string("tol")}); //, to_string(0));

    Comp copy("copy", cpy, ((r[i]=(d[i])=b[i]+0)));
    Comp dinit("dinit", sca, ds=rs0=rs=zero+0);
    Comp mset("mset", sca, memSet(s));
    Comp spmv("spmv", mtx, (s[i] += A[n] * d[j]));
    Comp ddot("ddot", vec, (ds += d[i]*s[i]));
    Comp rdot0("rdot0", vec, (rs0 += r[i]*r[i]));
    Comp adiv("adiv", sca, (alpha = rs0/ds));
    Comp xadd("xadd", vec, (x[i] += alpha * d[i]));
    Comp rsub("rsub", vec, (r[i] -= alpha*s[i]));
    Comp rdot("rdot", vec, (rs += r[i]*r[i]));
    Comp bdiv("bdiv", sca, (beta = rs / rs0));
    Comp dadd("dadd", vec, (d[i] = r[i] + beta * d[i]));     // FMA: a+b*c
    //Comp check("check", sca, (rs <= tol), (t=T+1));

    fuse();             // Perform fusions
    reschedule();       // Schedule
    parallelize();      // Parallelize code.
    perfmodel();        // perfmodel annotates graph with performance attributes.

    print("out/" + name + ".json");
    string result = codegen("out/" + name + ".o", "", "C++"); //, "auto");
    //cerr << result << endl;
    ASSERT_TRUE(!result.empty());
}

TEST(eDSLTest, ConjGradCSR) {
    Iter t('t'), i('i'), j('j'), n('n');
    Const T('T'), N('N'), M('M'), K('K');   // N=#rows/cols, M=#nnz, K=#iterations
    Func rp("rp"), col("col");
    Real zero(0.0);

    // Iteration spaces:
    Space cpy("cpy", 0 <= i < N);
    Space sca("sca", 1 <= t <= T);
    Space vec("vec", 1 <= t <= T ^ 0 <= i < N);
    Space csr("csr", 1 <= t <= T ^ 0 <= i < N ^ rp(i) <= n < rp(i+1) ^ j==col(n));
    Space mtx = csr;

    // Data spaces:
    Space A("A", M), x("x", N), b("b", N), r("r", N), s("s", N), d("d", N);
    Space alpha("alpha"), beta("beta"), ds("ds"), rs("rs"), rs0("rs0"), tol("tol", 1E-6);

    string name = "conjgrad_csr";
    init(name, "rs", "d", "", {string("tol")}, to_string(0));

    Comp copy("copy", cpy, ((r[i]=(d[i])=b[i]+0)));
    //Comp mset("mset", sca, memSet(s));
    Comp dinit("dinit", sca, ds=rs0=rs=zero+0);
    Comp sinit("sinit", vec, (s[i] = zero + 0));
    Comp spmv("spmv", mtx, (s[i] += A[n] * d[j]));
    Comp ddot("ddot", vec, (ds += d[i]*s[i]));
    Comp rdot0("rdot0", vec, (rs0 += r[i]*r[i]));
    Comp adiv("adiv", sca, (alpha = rs0/ds));
    Comp xadd("xadd", vec, (x[i] += alpha * d[i]));
    Comp rsub("rsub", vec, (r[i] -= alpha*s[i]));
    Comp rdot("rdot", vec, (rs += r[i]*r[i]));
    Comp bdiv("bdiv", sca, (beta = rs / rs0));
    Comp dadd("dadd", vec, (d[i] = r[i] + beta * d[i]));     // FMA: a+b*c
    //Comp check("check", sca, (rs <= tol), (t=T+1));

    // Perform fusions
    fuse();
    parallelize();

    perfmodel();        // perfmodel annotates graph with performance attributes.
    print("out/" + name + ".json");
    //Digraph itergraph = ConjGradTest::CSRGraph();
    //reschedule(itergraph);
    string result = codegen("out/" + name + ".o", "", "C++", "auto");
    //cerr << result << endl;
    ASSERT_TRUE(!result.empty());
}

TEST(eDSLTest, ConjGradDSR) {
    Iter t('t'), i('i'), j('j'), m('m'), n('n');
    Const T('T'), N('N'), M('M'), R('R');   // N=#rows/cols, M=#nnz, R=#nonzero rows
    Func crp("crp"), crow("crow"), col("col");
    Real zero(0.0);

    // Iteration spaces:
    Space cpy("cpy", 0 <= i < N);
    Space sca("sca", 1 <= t <= T);
    Space vec("vec", 1 <= t <= T ^ 0 <= i < N);
    //Space csr("csr", 1 <= t <= T ^ 0 <= i < N ^ rp(i) <= n < rp(i+1) ^ j==col(n));
    //Space mtx = csr;
    Space dsr("dsr", 1 <= t <= T ^ 0 <= m < R ^ i==crow(m) ^ crp(m) <= n < crp(m+1) ^ j==col(n));
    Space mtx = dsr;

    // Data spaces:
    Space A("A", M), x("x", N), b("b", N), r("r", N), s("s", N), d("d", N);
    Space alpha("alpha"), beta("beta"), ds("ds"), rs("rs"), rs0("rs0"), tol("tol", 1E-6);

    string name = "conjgrad_dsr";
    init(name, "rs", "d", "", {string("tol")}, "0");

    Comp copy("copy", cpy, ((r[i]=(d[i])=b[i]+0)));
    Comp dinit("dinit", sca, ds=rs0=rs=zero+0);
    Comp sinit("sinit", vec, (s[i] = zero + 0));
    Comp spmv("spmv", mtx, (s[i] += A[n] * d[j]));
    Comp ddot("ddot", vec, (ds += d[i]*s[i]));
    Comp rdot0("rdot0", vec, (rs0 += r[i]*r[i]));
    Comp adiv("adiv", sca, (alpha = rs0/ds));
    Comp xadd("xadd", vec, (x[i] += alpha * d[i]));
    Comp rsub("rsub", vec, (r[i] -= alpha*s[i]));
    Comp rdot("rdot", vec, (rs += r[i]*r[i]));
    Comp bdiv("bdiv", sca, (beta = rs / rs0));
    Comp dadd("dadd", vec, (d[i] = r[i] + beta * d[i]));     // FMA: a+b*c
    //Comp check("check", sca, (rs <= tol), (t=T+1));

    // Perform fusions
    fuse();
    parallelize();

    perfmodel();        // perfmodel annotates graph with performance attributes.
    print("out/" + name + ".json");
    //Digraph itergraph = ConjGradTest::CSRGraph();
    //reschedule(itergraph);
    string result = codegen("out/" + name + ".h", "", "C++", "auto");
    //cerr << result << endl;
    ASSERT_TRUE(!result.empty());
}

TEST(eDSLTest, ConjGradCSB) {
    Iter t('t'), i('i'), j('j'), m('m'), n('n'), bi("bi"), bj("bj"), ei("ei"), ej("ej");
    Const T('T'), N('N'), M('M'), B('B', 128), NB("NB");   // T=#iter, N=#rows/cols, M=#nnz, B=blk_size, NB=#nonzero blocks
    Func bp("bp"), brow("brow"), bcol("bcol"), erow("erow"), ecol("ecol");
    Real zero(0.0);

    // Iteration spaces:
    Space cpy("cpy", 0 <= i < N);
    Space sca("sca", 1 <= t <= T);
    Space vec("vec", 1 <= t <= T ^ 0 <= i < N);
//    Space dsr("dsr", 1 <= t <= T ^ 0 <= m < R ^ i==crow(m) ^ crp(i) <= n < crp(i+1) ^ j==col(n));
//    Space mtx = dsr;
    Space csb("csb", 1 <= t <= T ^ 0 <= m < NB ^ bp(m) <= n < bp(m+1) ^ i==B*brow(m)+erow(n) ^ j==B*bcol(m)+ecol(n));
    //Space csb("csb", 1 <= t <= T ^ 0 <= m < NB ^ bp(m) <= n < bp(m+1) ^ bi==brow(m) ^ bj==bcol(m) ^ ei==erow(n) ^ ej==ecol(n) ^ i==B*bi+ei ^ j==B*bj+ej);
    Space mtx = csb;

    // Data spaces:
    Space A("A", M), x("x", N), b("b", N), r("r", N), s("s", N), d("d", N);
    Space alpha("alpha"), beta("beta"), ds("ds"), rs("rs"), rs0("rs0"), tol("tol", 1E-6);

    string name = "conjgrad_csb";
    init(name, "rs", "d", "", {string("tol")}, "0");

    Comp copy("copy", cpy, ((r[i]=(d[i])=b[i]+0)));
    Comp dinit("dinit", sca, ds=rs0=rs=zero+0);
    Comp sinit("sinit", vec, (s[i] = zero + 0));
    Comp spmv("spmv", mtx, (s[i] += A[n] * d[j]));
    Comp ddot("ddot", vec, (ds += d[i]*s[i]));
    Comp rdot0("rdot0", vec, (rs0 += r[i]*r[i]));
    Comp adiv("adiv", sca, (alpha = rs0/ds));
    Comp xadd("xadd", vec, (x[i] += alpha * d[i]));
    Comp rsub("rsub", vec, (r[i] -= alpha*s[i]));
    Comp rdot("rdot", vec, (rs += r[i]*r[i]));
    Comp bdiv("bdiv", sca, (beta = rs / rs0));
    Comp dadd("dadd", vec, (d[i] = r[i] + beta * d[i]));     // FMA: a+b*c
    //Comp check("check", sca, (rs <= tol), (t=T+1));

    // Perform fusions
    fuse();
    parallelize();

    perfmodel();        // perfmodel annotates graph with performance attributes.
    print("out/" + name + ".json");
    //Digraph itergraph = ConjGradTest::CSRGraph();
    //reschedule(itergraph);
    string result = codegen("out/" + name + ".c", "", "C++", "auto");
    //cerr << result << endl;
    ASSERT_TRUE(!result.empty());
}

TEST(eDSLTest, ConjGradELL) {
    Iter t('t'), i('i'), j('j'), k('k'), n('n');
    Const T('T'), N('N'), M('M'), K('K');   // N=#rows/cols, M=#nnz, R=#max nonzeros/row
    Func lcol("lcol");
    Real zero(0.0);

    // Iteration spaces:
    Space cpy("cpy", 0 <= i < N);
    Space sca("sca", 1 <= t <= T);
    Space vec("vec", 1 <= t <= T ^ 0 <= i < N);
    //Space csr("csr", 1 <= t <= T ^ 0 <= i < N ^ rp(i) <= n < rp(i+1) ^ j==col(n));
    //Space mtx = csr;
    Space ell("ell", 1 <= t <= T ^ 0 <= k < K ^ 0 <= i < N ^ j==lcol(k,i));
    Space mtx = ell;

    // Data spaces:
    Space A("A", K, N), x("x", N), b("b", N), r("r", N), s("s", N), d("d", N);
    Space alpha("alpha"), beta("beta"), ds("ds"), rs("rs"), rs0("rs0"), tol("tol", 1E-6);

    string name = "conjgrad_ell";
    init(name, "rs", "d", "", {string("tol")}, "0");

    Comp copy("copy", cpy, ((r[i]=(d[i])=b[i]+0)));
    Comp dinit("dinit", sca, ds=rs0=rs=zero+0);
    Comp sinit("sinit", vec, (s[i] = zero + 0));
    Comp spmv("spmv", mtx, (s[i] += A(k,i) * d[j]));
    Comp ddot("ddot", vec, (ds += d[i]*s[i]));
    Comp rdot0("rdot0", vec, (rs0 += r[i]*r[i]));
    Comp adiv("adiv", sca, (alpha = rs0/ds));
    Comp xadd("xadd", vec, (x[i] += alpha * d[i]));
    Comp rsub("rsub", vec, (r[i] -= alpha*s[i]));
    Comp rdot("rdot", vec, (rs += r[i]*r[i]));
    Comp bdiv("bdiv", sca, (beta = rs / rs0));
    Comp dadd("dadd", vec, (d[i] = r[i] + beta * d[i]));     // FMA: a+b*c
    //Comp check("check", sca, (rs <= tol), (t=T+1));

    // Perform fusions
    fuse();
    parallelize();

    perfmodel();        // perfmodel annotates graph with performance attributes.
    print("out/" + name + ".json");
    //Digraph itergraph = ConjGradTest::CSRGraph();
    //reschedule(itergraph);
    string result = codegen("out/" + name + ".h", "", "C++", "auto");
    //cerr << result << endl;
    ASSERT_TRUE(!result.empty());
}

TEST(eDSLTest, ConjGradDIA) {
    Iter t('t'), i('i'), j('j'), k('k'), n('n');
    Const T('T'), N('N'), M('M'), D('D');   // N=#rows/cols, M=#nnz, R=#max nonzeros/row
    Func doff("doff");
    Real zero(0.0);

    // Iteration spaces:
    Space cpy("cpy", 0 <= i < N);
    Space sca("sca", 1 <= t <= T);
    Space vec("vec", 1 <= t <= T ^ 0 <= i < N);
    //Space csr("csr", 1 <= t <= T ^ 0 <= i < N ^ rp(i) <= n < rp(i+1) ^ j==col(n));
    //Space mtx = csr;
    Space dia("dia", 1 <= t <= T ^ 0 <= i < N ^ 0 <= k < D ^ j==doff(k)+i);
    Space mtx = dia;

    // Data spaces:
    Space A("A", D, N), x("x", N), b("b", N), r("r", N), s("s", N), d("d", N);
    Space alpha("alpha"), beta("beta"), ds("ds"), rs("rs"), rs0("rs0"), tol("tol", 1E-6);

    string name = "conjgrad_dia";
    init(name, "rs", "d", "", {string("tol")}, "0");

    Comp copy("copy", cpy, ((r[i]=(d[i])=b[i]+0)));
    Comp dinit("dinit", sca, ds=rs0=rs=zero+0);
    Comp sinit("sinit", vec, (s[i] = zero + 0));
    Comp spmv("spmv", mtx, (j<N), (s[i] += A(k,i) * d[j]));
    Comp ddot("ddot", vec, (ds += d[i]*s[i]));
    Comp rdot0("rdot0", vec, (rs0 += r[i]*r[i]));
    Comp adiv("adiv", sca, (alpha = rs0/ds));
    Comp xadd("xadd", vec, (x[i] += alpha * d[i]));
    Comp rsub("rsub", vec, (r[i] -= alpha*s[i]));
    Comp rdot("rdot", vec, (rs += r[i]*r[i]));
    Comp bdiv("bdiv", sca, (beta = rs / rs0));
    Comp dadd("dadd", vec, (d[i] = r[i] + beta * d[i]));     // FMA: a+b*c
    //Comp check("check", sca, (rs <= tol), (t=T+1));

    // Perform fusions
    fuse();
    parallelize();

    perfmodel();        // perfmodel annotates graph with performance attributes.
    print("out/" + name + ".json");
    //Digraph itergraph = ConjGradTest::CSRGraph();
    //reschedule(itergraph);
    string result = codegen("out/" + name + ".h", "", "C++", "auto");
    //cerr << result << endl;
    ASSERT_TRUE(!result.empty());
}

TEST(eDSLTest, JacobiMethod) {
    Iter t('t'), s('s'), i('i'), j('j'), n('n');
    Const N('N'), M('M'), T('T');

    // Data spaces:
    Space A("A", N, N), x("x", T, N), b("b", N), err("err", 1.0), tol("tol", 1E-6);

    // Iteration space
    Space sca("sca", 1 <= t <= T);
    Space vec("vec", 1 <= t <= T ^ 0 <= i < N);
    Space mtx("mtx", 1 <= t <= T ^ 0 <= i < N ^ 0 <= j < N);

    string name = "jacobi";
    pdfg::init(name, "x", "d", "", {"tol"}, to_string(0));
    Comp clear("clear", sca, Math(err, Real(0.), "="));
    Comp init("init", vec, (x(t,i) = b(i)+0));
    Comp dot("dot", mtx, (i != j), (x(t,i) += -A(i,j) * x(t-1,j)));
    //Comp dot("dot", mtx, x(t,i) += -A(i,j) * x(t-1,j));
    Comp div("div", vec, (x(t,i) /= A(i,i)));
    Comp norm("norm", vec, (err += (x(t,i) - x(t-1,i)) * (x(t,i) - x(t-1,i))));
    // TODO: This works, but need to formalize exit predicates...
    Comp check("check", sca, (sqrt(err) <= tol), (t=T+1));
    // Perform fusions
    fuse(); //{"clear", "init", "dot", "div", "norm", "check"});

    // TODO Items:
    // 1) Fix schedule visitor to produce correct schedules (as in out/jacobi.in).
    // 2) Fix data mapping functions -- observe that reuse distance of x means only 2*N data needed.
    // 3) Fix data allocation as well (e.g., 'x' alloc size is wrong).
    // 4) Make it sparse!

    print("out/" + name + ".json");
    string result = codegen("out/" + name + ".c", "", "C++", "auto");
    //cerr << result << endl;
    ASSERT_TRUE(!result.empty());
}

TEST(eDSLTest, CP_ALS) {
    // TODO: Figure out how to do transpose and norm...

    // TACO-MTTKRP output for reference:
//    for (int32_t pX1 = X1_pos[0]; pX1 < X1_pos[1]; pX1++) {
//        int32_t iX = X1_coord[pX1];
//        for (int32_t pX2 = X2_pos[pX1]; pX2 < X2_pos[(pX1 + 1)]; pX2++) {
//            int32_t jX = X2_coord[pX2];
//            for (int32_t pX3 = X3_pos[pX2]; pX3 < X3_pos[(pX2 + 1)]; pX3++) {
//                int32_t kX = X3_coord[pX3];
//                double tk = X_vals[pX3];
//                for (int32_t rC = 0; rC < C2_dimension; rC++) {
//                    int32_t pC2 = kX * C2_dimension + rC;
//                    int32_t pB2 = jX * B2_dimension + rC;
//                    int32_t pA2 = iX * A2_dimension + rC;
//                    A_vals[pA2] = A_vals[pA2] + tk * C_vals[pC2] * B_vals[pB2];
//                }
//            }
//        }
//    }

    // TODO: These are transposes, figure out if can be handled by changing the data mapping, matrices are
    //   linearized, so should be possible.
    // MxN*NxP -> RxK*KxR
//    Space mm("mm", 0 <= i < I ^ 0 <= k < K ^ 0 <= j < J);
//    Comp mtm = mm + (C(i,k) += A(i,j) * B(j,k));
//    int i, j, k;
//    for (i = 0; i < N; i++)
//        for (j = 0; j < N; j++)
//            for (k = 0; k < N; k++)
//                res[i][j] += mat1[i][k] *
//                             mat2[k][j];

    Iter m("m"), i("i"), j("j"),  k("k"), q("q"), r("r");
    Func ind0("ind0"), ind1("ind1"),ind2("ind2");
    Const I("I"), J("J"), K("K"), M("M"), R("R");
    Real one(1.0);

    Space coo("coo", 0 <= m < M ^ i==ind0(m) ^ j==ind1(m) ^ k==ind2(m));
    Space krp = coo ^ 0 <= r < R;
    Space mtx("mtx", 0 <= i < I ^ 0 <= j < J ^ 0 <= k < K ^ 0 <= r < R);
    Space A("A",I,R), B("B",J,R), C("C",K,R), X("X",M);
    Space Akr("Akr",I,R), Bkr("Bkr",J,R), Ckr("Ckr",K,R);
    Space Asq("Asq",R,R), Bsq("Bsq",R,R), Csq("Csq",R,R);
    Space Ahp("Ahp",R,R), Bhp("Bhp",R,R), Chp("Chp",R,R);
    Space Ainv("Ainv",R,R), Binv("Binv",R,R), Cinv("Cinv",R,R);
    Space sums("sums",R), lmbda("lmbda",R);

    Space vecR("vecR", 0 <= r < R);
    Space hadR("hadR", 0 <= q < R ^ 0 <= r < R);
    Space mtxA("mtxA", 0 <= i < I ^ 0 <= r < R);
    Space mtxB("mtxB", 0 <= j < J ^ 0 <= q < R);
    Space mtxC("mtxC", 0 <= k < K ^ 0 <= r < R);
    Space mulA("mulA", 0 <= q < R ^ 0 <= r < R ^ 0 <= i < I);
    Space mulB("mulB", 0 <= q < R ^ 0 <= r < R ^ 0 <= j < J);
    Space mulC("mulC", 0 <= q < R ^ 0 <= r < R ^ 0 <= k < K);
    Space pmmA("pmmA", 0 <= i < I ^ 0 <= q < R ^ 0 <= r < R);
    Space pmmB("pmmB", 0 <= j < J ^ 0 <= q < R ^ 0 <= r < R);
    Space pmmC("pmmC", 0 <= k < K ^ 0 <= q < R ^ 0 <= r < R);

    init("cp_als", "", "f", "u", {"A", "B", "C", "lmbda"}); //, to_string(0));

    // Randomize factor matrices...
    Comp initA("Ainit", mtxA, (A(i,r) = urand()));
    Comp initB("Binit", mtxB, (B(j,r) = urand()));
    Comp initC("Cinit", mtxC, (C(k,r) = urand()));

    Comp mmC("Cmm", mulC, (Csq(q,r) += C(q,k) * C(k,r)));
    Comp mmB("Bmm", mulB, (Bsq(q,r) += B(q,j) * B(j,r)));
    // Hadamard (component-wise) product on RxR square matrices.
    Comp hadA("Ahad", hadR, (Ahp(q,r) = Csq(q,r) * Bsq(q,r)));
    // Now the M-P pseudoinverse (using SVD: A=USV* => A+ = VS+U*).
    Comp pinvA("Apinv", hadR, Ainv(q,r) = one / Ahp(q,r)); //pinv(Ahp));
    Comp krpA("Akrp", krp, (Akr(i,r) += X(m) * C(k,r) * B(j,r)));
    Comp mmAp("Apmm", pmmA, (A(i,q) += Akr(i,r) * Ainv(q,r)));
    // Norm A columns and store in \vec{\lambda}
    Comp ssqA("Assq", mtxA, (sums(r) += A(i,r) * A(i,r)));
    //Comp normA = vecR + (lmbda(r) = sqrt(lmbda(r)));

    Comp mmA("Amm", mulA, (Asq(q,r) += A(q,i) * A(i,r)));
    // Hadamard (component-wise) product on RxR square matrices.
    Comp hadB("Bhad", hadR, (Bhp(q,r) = Csq(q,r) * Asq(q,r)));
    // Now the M-P pseudoinverse.
    Comp pinvB("Bpinv", hadR, Binv(q,r) = one / Bhp(q,r)); //pinv(Bhp));
    Comp krpB("Bkrp", krp, (Bkr(j,r) += X(m) * C(k,r) * A(i,r)));
    Comp mmBp("Bpmm", pmmB, (B(j,q) += Bkr(j,r) * Binv(q,r)));
    // Norm B columns and store in \vec{\lambda}
    Comp ssqB("Bssq", mtxB, (sums(r) += B(j,r) * B(j,r)));
    //Comp normB = vecR + (lmbda(r) = sqrt(lmbda(r)));

    // Hadamard (component-wise) product on RxR square matrices.
    Comp hadC("Chad", hadR, (Chp(q,r) = Bsq(q,r) * Asq(q,r)));
    // Now the M-P pseudoinverse.
    Comp pinvC("Cpinv", hadR, Cinv(q,r) = one / Chp(q,r)); //pinv(Chp));
    Comp krpC("Ckrp", krp, (Ckr(k,r) += X(m) * B(j,r) * A(i,r)));
    Comp mmCp("Cpmm", pmmC, (C(k,q) += Ckr(k,r) * Cinv(r,r)));
    // Norm C columns and store in \vec{\lambda}
    Comp ssqC("Cssq", mtxC, (sums(r) += C(k,r) * C(k,r)));
    Comp norm("norm", vecR, (lmbda(r) = sqrt(sums(r))));

    print("out/cp_als.json");
    string result = codegen("out/cp_als.h");

    // cp_als in sktensor returns Kruskal tensor P=(U,\lambda), where U(0)=A, U(1)=B, U(2)=C

    //cerr << result << endl;
    ASSERT_TRUE(!result.empty());
}

TEST(eDSLTest, CP_ALS_COO) {
    const unsigned N = 3;

    Iter m('m'), q('q'), r('r'), t('t');
    Const M("M"), R("R"), T("T");
    Space X("X",M);

    vector<string> names(N+1);
    Func indices[N];
    Iter iters[N];
    Const uppers[N];
    Space matrices[N];
    Space inits[N];
    Space new_mtx[N];
    Space V("V", R, R), Vinv("Vinv", R, R);
    Space Y("Y", R, R);
    Space lmbda("lmbda", R), sums("sums",R);
    Real one(1.0);
    Real zero(0.0);

    Space coo("coo", 0 <= t < T ^ 0 <= m < M);       // The 'coo' iteration space visits each element in the sparse tensor...

    for (int n = 0; n < N; n++) {
        names[n] = string(1, 'A' + n);
        iters[n] = Iter(string(1, 'i' + n));
        uppers[n] = Const(string(1, 'I' + n));
        inits[n] = Space(names[n], 0 <= r < R ^ 0 <= iters[n] < uppers[n]);
        matrices[n] = Space(names[n], uppers[n], R);
        //matrices[n] = Space(names[n], 0 <= iters[n] < uppers[n] ^ 0 <= r < R);
        //new_mtx[n] = Space(names[n] + "new", 0 <= iters[n] < uppers[n] ^ 0 <= r < R);
        new_mtx[n] = Space(names[n] + "new", uppers[n], R);
        indices[n] = Func("ind" + to_string(n));
        coo ^= (iters[n] == indices[n](m));
    }

    Space krp = coo ^ 0 <= r < R;
    Space vec("vec", 0 <= t < T ^ 0 <= r < R);
    Space had("had", 0 <= t < T ^ 0 <= r < R ^ 0 <= q < R);
    Space sca("sca", 0 <= t < T);
    names[N] = "lmbda";

    string fxn = "cp_als_" + to_string(N) + "d_coo";
    init(fxn, "", "f", "u", names, to_string(0));

    // Build the computations...

    for (int n = 0; n < N; n++) {
        // Randomize factor matrices...
        Comp init(names[n] + "init", inits[n], (matrices[n](iters[n],r) = urand()));
    }

    //int n = 0;
    int s = 0;
    for (int n = 0; n < N; n++) {
        //Comp vinit("Vinit", sca, arrInit(V, 1.0));
        // Initialize 'V' to one
        Comp vinit("Vinit" + to_string(n), had, (V(r,q)=one+0));
        for (int p = 0; p < N; p++) {
            if (n != p) {
                // MatrixMult: U(m)^T * U(m)
                //Comp yinit("Yinit" + to_string(p), sca, memSet(Y));
                // Initialize 'Y' to zero
                Comp yinit("Yinit" + to_string(s), had, (Y(r,q)=zero+0));
                // Multply factor matrices by transpose and store in 'Y'
                Space mmul(names[p] + "mmul", 0 <= t < T ^ 0 <= r < R ^ 0 <= q < R ^ 0 <= iters[p] < uppers[p]);
                Comp mm(names[p] + "Ymm" + to_string(s), mmul, (Y(r,q) +=
                        matrices[p](iters[p],r) * matrices[p](q,iters[p])));
                // Hadamard: V *= Y
                // Pointwise multiplication (Hadamard) of V*Y
                Comp hp("VYhp" + to_string(s), had, (V(r,q) *= Y(r,q)));
                s += 1;
            }
        }

        // MTTKRP...
        Expr prod = X(m) + 0;
        for (int p = N - 1; p >= 0; p--) {
            if (n != p) {
                prod = prod * matrices[p](iters[p], r);
            }
        }
        Comp mttkrp(names[n] + "krp", krp, (new_mtx[n](iters[n],r) += prod));

        // Moore-Penrose Pseudoinverse of V
        Comp inv(names[n] + "pinv", sca, Vinv=pinv(V,Vinv));
        // Multiply factor matrix by Vinv.
        Space pmm(names[n] + "pmm", 0 <= t < T ^ 0 <= iters[n] < uppers[n] ^ 0 <= r < R ^ 0 <= q < R);
        Comp mmp(names[n] + "mmp", pmm, (new_mtx[n](iters[n],r) += new_mtx[n](iters[n],q) * Vinv(r,q)));

        // Normalize...
        Space sum(names[n] + "sum", 0 <= t < T ^ 0 <= r < R ^ 0 <= iters[n] < uppers[n]);
        // Sum of squares of factor matrix.
        Comp ssq(names[n] + "ssq", sum, (sums(r) += new_mtx[n](iters[n],r) * new_mtx[n](iters[n],r)));
        // Compute the Froebenius norm
        Comp norm(names[n] + "norm", vec, (lmbda(r) = sqrt(sums(r))));
        // Finally, normalize factor matrix by lambda.
        // U[n] = Unew / lmbda
        Comp div(names[n] + "div", sum, (matrices[n](iters[n],r) = new_mtx[n](iters[n],r) / lmbda(r)));
    }

    fuse(); // Fuse-all nodes...
    print("out/" + fxn + ".json");
    string result = codegen("out/" + fxn + ".h", "", "C++");

    // cp_als in sktensor returns Kruskal tensor P=(U,\lambda), where U(0)=A, U(1)=B, U(2)=C

    //cerr << result << endl;
    ASSERT_TRUE(!result.empty());
}

TEST(eDSLTest, Transpose) {
    const int M = 2;
    const int N = 3;
    float A[] = {0., 2., 4., 6., 8., 10.};

#define offset2(i,j,K) ((j)+(i)*(K))
#define A(i,j) A[offset2((i),(j),(N))]
#define A_T(i,j) A[offset2((j),(i),(N))]

    cout << "Matrix:\n";
    for (int i = 0; i < M; i++) {
        for (int j = 0; j < N; j++) {
            cout << A(i,j) << " ";
        }
        cout << "\n";
    }

    cout << "Transpose:\n";
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < M; j++) {
            //cout << A_T(i,j) << " ";
            cout << A(j,i) << " ";
        }
        cout << "\n";
    }

#undef A
#undef A_T
#undef offset2
}

TEST(eDSLTest, SGeMM) {
    // C(i,j) = C(i,j) * beta + alpha * A(i,k) * B(k,j)
//    for (i = 0; i < N; i++) {
//        for (j = 0; j < M; j++) {
//            C[i][j] *= beta;
//            for (k = 0; k < K; k++)
//                C[i][j] += alpha * A[i][k] * B[k][j];
//        }
//    }

    Iter i('i'), j('j'), k('k');
    Const N('N'), M('M'), P('P');
    Space A("A", N, P), B("B", P, M), C("C", N, M), D("D", N, M);
    Space a("a"), b("b");

    auto sub = a * A(i,k);
    init("sgemm");
    Comp init("init", (0 <= i < N ^ 0 <= j < M), (C(i,j) *= b));
    Comp gemm("gemm", (0 <= i < N ^ 0 <= j < M ^ 0 <= k < P), (C(i,j) += a * A(i,k) * B(k,j)));

    // This makes an excellent opportunity to implement fusion!
    pdfg::fuse(init, gemm);
    print("out/sgemm.json");
    string result = codegen("out/sgemm.o");
    //cerr << result << endl;
    ASSERT_TRUE(!result.empty());
}

TEST(eDSLTest, SDDMM) {
    // A(i,j) = B(i,j) * C(i,k) * D(k,j)
    // A=B*CD, wher
    //
    //
    //
    // e A and B are sparse matrices, C and D are dense matrices, * denotes component-wise multiplication
//    int A1_pos = A.d1.pos[0];
//    int A2_pos = A.d2.pos[A1_pos];
//    for (int B1_pos = B.d1.pos[0]; B1_pos < B.d1.pos[1]; B1_pos++) {
//        int iB = B.d1.idx[B1_pos];
//        for (int B2_pos = B.d2.pos[B1_pos]; B2_pos < B.d2.pos[B1_pos + 1]; B2_pos++) {
//            int jB = B.d2.idx[B2_pos];
//            for (int kC = 0; kC < K; kC++) {
//                int C2_pos = (iB * I) + kC;
//                int D2_pos = (jB * J) + kC;
//                A_val[A2_pos] += (B.vals[B2_pos] * C.vals[C2_pos]) * D.vals[D2_pos];
//            }
//            A2_pos++;
//        }
//        if (A.d2.pos[A1_pos + 1] > A.d2.pos[A1_pos]) {
//            A1_pos++;
//        }
//    }

    Iter i('i'), j('j'), k('k');
    Const I('I'), J('J'), K('K');
    Space sddm("sddm", 0 <= i < I ^ 0 <= j < J ^ 0 <= k < K);
    Space A("A", I, J), B("B", I, J), C("C", I, K), D("D", K, J);

    init("sddm");
    Comp comp = sddm + (A(i,j) += B(i,j) * C(i,k) * D(k,j));
    print("out/sddm.json");
    string result = codegen("out/sddm.o");
    //cerr << result << endl;
    ASSERT_TRUE(!result.empty());
}

TEST(eDSLTest, Backprop) {
    Space x("xor"); //, {0., 0., 0., 1., 1., 0., 1., 1.});
    Space layers("layers"); //, {2., 2., 2., 1.});
    Space eta("eta", 0.1);
    Space theta;

}

TEST(eDSLTest, CSR_COO_Insp) {
    Iter i("i"), j("j"), n("n");
    Const N("N"), NNZ("NNZ");
    Func rp("rp"), row("row"), col("col");
    Space csr("Icsr", 0 <= i < N ^ rp(i) <= n < rp(i+1) ^ j==col(n));

    // CSR->COO Inspector:
    init("csr_coo_insp");
    //Comp insp_cnt = csr + (NNZ+=1);
    Space snnz("nnz");
    Comp insp_nnz = snnz + (NNZ=rp(N)+0);
    Space insp("coal", 0 <= i < N ^ rp(i) <= n < rp(i+1));
    Comp insp_row = insp + (row(n)=i+0);
    string result = codegen();
    //cerr << result << endl;
    string expected = "#include <stdio.h>\n"
                      "#include <stdlib.h>\n"
                      "#include <stdint.h>\n"
                      "#include <sys/time.h>\n\n"
                      "#define min(x,y) (((x)<(y))?(x):(y))\n"
                      "#define max(x,y) (((x)>(y))?(x):(y))\n"
                      "#define floord(x,y) ((x)/(y))\n"
                      "#define offset2(i,j,M) ((j)+(i)*(M))\n"
                      "#define offset3(i,j,k,M,N) ((k)+((j)+(i)*(M))*(N))\n"
                      "#define offset4(i,j,k,l,M,N,P) ((l)+((k)+((j)+(i)*(M))*(N))*(P))\n"
                      "#define arrinit(ptr,val,size) for(unsigned __i__=0;__i__<(size);__i__++) (ptr)[__i__]=(val)\n\n"
                      "void csr_coo_insp(const unsigned N, const unsigned* rp, unsigned* NNZ, unsigned* row);\n"
                      "inline void csr_coo_insp(const unsigned N, const unsigned* rp, unsigned* NNZ, unsigned* row) {\n\n"
                      "#undef s0\n"
                      "#define s0() NNZ=rp(N)\n\n"
                      "s0();\n\n"
                      "#define rp(i) rp[(i)]\n"
                      "#define rp1(i) rp[(i+1)]\n\n"
                      "#undef s0\n"
                      "#define s0(i,n) row((n))=(i)\n\n"
                      "unsigned t1,t2;\n"
                      "for(t1 = 0; t1 <= N-1; t1++) {\n"
                      "  for(t2 = rp(t1); t2 <= rp1(t1)-1; t2++) {\n"
                      "    s0(t1,t2);\n"
                      "  }\n"
                      "}\n"
                      "\n"
                      "}    // csr_coo_insp\n";
    ASSERT_EQ(result, expected);
}