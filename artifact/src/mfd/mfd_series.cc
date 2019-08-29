#include <iostream>
#include "MiniFluxBenchmark-series.h"
using namespace std;

int main(int argc, char *argv[]) {
    MiniFluxBenchmark benchmark(argc, argv);
    benchmark.run();
    benchmark.report();
    return (benchmark.valid() ? 0 : -1);
}
