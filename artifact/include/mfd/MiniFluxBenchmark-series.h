//
// Created by edavis on 6/7/17.
//

#ifndef BENCHMARK_MINIFLUXBENCHMARK_H
#define BENCHMARK_MINIFLUXBENCHMARK_H

#include "Benchmark.h"

//<Include>
#include "miniFluxDiv-seriesReduceCLO.h"

#ifndef dx
#define dx 0.5
#define factor1 (1.0/12.0)
#define factor2 2.0
#endif

typedef DType Real;

MiniFluxDivData *mini_flux_div_init(Configuration& config);
void mini_flux_div_truth(Real ** old_boxes, Real** new_boxes, Real *timer, Configuration& config);
void mini_flux_div_lc(Real** old_boxes, Real** new_boxes, Real *timer, Configuration& config);
bool mini_flux_div_comp(Real** new_boxes, Real** ref_boxes, Configuration& config, vector<int>& loc);

class MiniFluxBenchmark : public Benchmark {
public:
    MiniFluxBenchmark();
    MiniFluxBenchmark(int argc, char *argv[]);
    virtual ~MiniFluxBenchmark();

    void init();
    void finish();
    string error();
    void toCSV();
};

#endif //BENCHMARK_MINIFLUXBENCHMARK_H
