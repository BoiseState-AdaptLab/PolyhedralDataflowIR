//#include <iostream>
#include "BCSRBenchmark.h"

using namespace std;

int main(int argc, char *argv[]) {
    BCSRBenchmark benchmark(argc, argv);
    benchmark.run();
    benchmark.report();

    return (benchmark.valid() ? 0 : -1);
}