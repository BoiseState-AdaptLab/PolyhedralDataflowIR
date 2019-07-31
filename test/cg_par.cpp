#include <omp.h>
#include <sys/time.h>

#include <util/LIKWID.hpp>
#include <util/MatrixIO.hpp>
using util::MatrixIO;

#include <string>
using std::string;
using std::to_string;

#include <vector>
using std::vector;

#include <Eigen/Sparse>
#include <Eigen/SparseExtra>
#include <Eigen/IterativeLinearSolvers>
using Eigen::VectorXd;
using Eigen::SparseMatrix;
using Eigen::ConjugateGradient;
using Eigen::Lower;
using Eigen::Upper;
typedef Eigen::Triplet<double> Triple;

#include "conjgrad_coo.h"

double get_wtime() {
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return (double) tv.tv_sec + (((double) tv.tv_usec) * 1E-6);
}

void setup(const char* matrix, unsigned* nnz, unsigned* nrow, unsigned* ncol, unsigned* maxiter,
           unsigned** rows, unsigned** cols, double* __restrict* vals, double* __restrict* x, double* __restrict* b,
           SparseMatrix<double>& Aspm, VectorXd& xVec, VectorXd& bVec) {
    // Read matrix file
    MatrixIO mtx(matrix);
    mtx.read();

    *nnz = mtx.nnz();
    *nrow = mtx.nrows();
    *ncol = mtx.ncols();

    // Copy arrays...
    unsigned nbytes = *nnz * sizeof(unsigned);
    *rows = (unsigned*) malloc(nbytes);
    memcpy(*rows, mtx.rows(), nbytes);
    *cols = (unsigned*) malloc(nbytes);
    memcpy(*cols, mtx.cols(), nbytes);

    nbytes = *nnz * sizeof(double);
    *vals = (double*) malloc(nbytes);
    memcpy(*vals, mtx.vals(), nbytes);

#ifdef EIGEN_EXEC
    vector<Triple> triples;
    triples.reserve(*nnz);
    for (unsigned n = 0; n < *nnz; n++) {
        triples.push_back(Triple((*rows)[n], (*cols)[n], (*vals)[n]));
    }

    Aspm.resize(*nrow, *ncol);
    Aspm.setFromTriplets(triples.begin(), triples.end());

    xVec.resize(*ncol);
    bVec.resize(*nrow);

    for (unsigned i = 0; i < *nrow; i++) {
        bVec(i) = 1.0;
    }
#else
    *x = (double*) calloc(*ncol, sizeof(double));
    *b = (double*) malloc(*nrow * sizeof(double));

    // Initialize b to something more interesting than 0
    for (unsigned i = 0; i < *nrow; i++) {
        (*b)[i] = 1.0;
    }
#endif
}

void teardown(unsigned** rows, unsigned** cols, double* __restrict* vals, double* __restrict* x, double* __restrict* b) {
    free(*rows);
    free(*cols);
    free(*vals);
#ifndef EIGEN_EXEC
    free(*x);
    free(*b);
#endif
}

int main(int argc, char **argv) {
    double ptime;
    double tsum = 0.0;
    int nproc = 1;
    int pid = 0;
    int nruns = 1;

    const char* name = argv[0];
    const char* matrix = argv[1];

    unsigned nnz, nrow, ncol, maxiter = 500;
    unsigned niter = 0;
    unsigned* rows;
    unsigned* cols;

    double err = 1.0, tol = 1e-10;
    double* __restrict vals;
    double* __restrict x;
    double* __restrict b;

    // Eigen Objects:
    VectorXd xVec, bVec;
    SparseMatrix<double> Aspm;
    ConjugateGradient<SparseMatrix<double>, Lower|Upper> cg;

    #pragma omp parallel
    {
    nproc = omp_get_num_threads();
    }

    if (argc > 2) {
        nruns = atoi(argv[2]);
    }

    setup(matrix, &nnz, &nrow, &ncol, &maxiter, &rows, &cols, &vals, &x, &b, Aspm, xVec, bVec);

#ifdef EIGEN_EXEC
    cg.setMaxIterations(maxiter);
    cg.setTolerance(tol);
#endif

#ifdef LIKWID_PERF
    LIKWID_MARKER_INIT
#endif

    for (unsigned i = 0; i < nruns; i++) {
        ptime = get_wtime();

        //#pragma omp parallel for private(pid)
        //for (unsigned p = 0; p < nproc; p++) {
        //    pid = omp_get_thread_num();

#ifdef LIKWID_PERF
        LIKWID_MARKER_START(name);
#endif

#ifdef VTUNE_PERF
        __SSC_MARK(0x111); // start SDE tracing, note it uses 2 underscores
    __itt_resume(); // start VTune, again use 2 underscores
#endif

//        unsigned ndx = i;
//        ndx += nruns * pid;

#ifdef EIGEN_EXEC
        cg.compute(Aspm);
        xVec = cg.solve(bVec);
        niter = cg.iterations();
        err = cg.error();
        x = xVec.data();
#else
        err = conjgrad_coo(vals, b, nnz, nrow, maxiter, cols, rows, x);
        niter = maxiter;
#endif

#ifdef LIKWID_PERF
        LIKWID_MARKER_STOP(name);
#endif

#ifdef VTUNE_PERF
        __itt_pause(); // stop VTune
    __SSC_MARK(0x222); // stop SDE tracing
#endif

        //}

        ptime = get_wtime() - ptime;
        tsum += ptime;
    }

#ifdef LIKWID_PERF
    LIKWID_MARKER_CLOSE
#endif

    if (pid < 1) {
        fprintf(stdout, "%s: niter=%d,x=%lf,nprocs=%d,nruns=%d,time=%lf\n",
                name, niter, x[0], nproc, nruns, tsum / (double) nruns);
    }

    teardown(&rows, &cols, &vals, &x, &b);

    return 0;
}
