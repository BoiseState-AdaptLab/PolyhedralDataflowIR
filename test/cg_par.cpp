#include <omp.h>
#include <sys/time.h>

#include <util/LIKWID.hpp>
#include <util/MatrixIO.hpp>
using util::MatrixIO;

#include <cstring>
#include <string>
using std::string;
using std::to_string;

#include <vector>
using std::vector;

#ifdef EIGEN_EXEC
#include <Eigen/Sparse>
#include <Eigen/SparseExtra>
#include <Eigen/IterativeLinearSolvers>
using Eigen::VectorXd;
using Eigen::SparseMatrix;
using Eigen::ConjugateGradient;
using Eigen::Lower;
using Eigen::Upper;
typedef Eigen::Triplet<double> Triple;
#endif

// Inspectors
#include "coo_csr_insp.h"
#include "coo_dsr_insp.h"
#include "coo_ell_insp.h"
#include "coo_csb_insp.h"
#include "coo_dia_insp.h"

// Executors
#include "conjgrad_coo.h"
#include "conjgrad_csr.h"
#include "conjgrad_dsr.h"
#include "conjgrad_ell.h"
#include "conjgrad_csb.h"
#include "conjgrad_dia.h"

double get_wtime() {
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return (double) tv.tv_sec + (((double) tv.tv_usec) * 1E-6);
}

void setup(const char* matrix, unsigned* nnz, unsigned* nrow, unsigned* ncol, unsigned* maxiter,
           unsigned** rows, unsigned** cols, double* __restrict* vals, double* __restrict* x, double* __restrict* b
#ifdef EIGEN_EXEC
           , SparseMatrix<double>& Aspm, VectorXd& xVec, VectorXd& bVec) {
#else
           ) {
#endif
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
    double itime;
    double tsum = 0.0;
    double isum = 0.0;
    int nproc = 1;
    int pid = 0;
    int nruns = 1;

    const char* name = argv[0];
    const char* matrix = argv[1];
    char format[16] = {'\0'};

    unsigned nnz, nrow, ncol, maxiter = 500;
    unsigned niter = 0;
    unsigned size = 0;
    unsigned* rows;
    unsigned* cols;

    unsigned* rowptr;
    unsigned* bptr;
    unsigned* brow;
    unsigned* bcol;
    unsigned char* erow;
    unsigned char* ecol;
    unsigned nb;
    unsigned bs = 128;
    double* bval;
    unsigned nzr;
    unsigned* crp;
    unsigned* crow;
    unsigned nell;
    unsigned* lcol;
    double* lval;
    unsigned ndia;
    int* doff;
    double* dval;

    double err = 1.0, tol = 1e-10;
    double* __restrict vals;
    double* __restrict x;
    double* __restrict b;

#ifdef EIGEN_EXEC
    // Eigen Objects:
    VectorXd xVec, bVec;
    SparseMatrix<double> Aspm;
    ConjugateGradient<SparseMatrix<double>, Lower|Upper> cg;
#endif

    #pragma omp parallel
    {
    nproc = omp_get_num_threads();
    }

    if (argc > 2) {
        nruns = atoi(argv[2]);
    }

    if (argc > 3) {
        strcpy(format, argv[3]);
    } else {
        strcpy(format, "coo");
    }

    setup(matrix, &nnz, &nrow, &ncol, &maxiter, &rows, &cols, &vals, &x, &b
#ifdef EIGEN_EXEC
    , Aspm, xVec, bVec);
    cg.setMaxIterations(maxiter);
    cg.setTolerance(tol);
#else
    );
#endif

#ifdef LIKWID_PERF
    LIKWID_MARKER_INIT
#endif

    if (strlen(format) > 0) {
        if (!strncmp(format, "csr", 3)) {
            itime = get_wtime();
            rowptr = (unsigned*) calloc(nrow + 1, sizeof(unsigned));
            coo_csr_insp(nnz, rows, rowptr);
        } else if (!strncmp(format, "csb", 3)) {
            itime = get_wtime();
            nb = coo_csb_insp(vals, bs, nnz, cols, rows, &bval, &bcol, &bptr, &brow, &ecol, &erow);
        } else if (!strncmp(format, "dsr", 3)) {
            itime = get_wtime();
            nzr = coo_dsr_insp(rows, nnz, &crow, &crp);
        } else if (!strncmp(format, "ell", 3)) {
            itime = get_wtime();
            nell = coo_ell_insp(nnz, rows, cols, vals, &lcol, &lval);
        } else if (!strncmp(format, "dia", 3)) {
            ndia = coo_dia_insp(vals, nnz, cols, rows, &dval, &doff);
        } else {
            itime = 0.0;
        }

        if (itime != 0.0) {
            itime = get_wtime() - itime;
        }
    }

    for (unsigned i = 0; i < nruns; i++) {
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

#ifdef EIGEN_EXEC
        ptime = get_wtime();
        cg.compute(Aspm);
        xVec = cg.solve(bVec);
        niter = cg.iterations();
        err = cg.error();
        x = xVec.data();
#else
        if (strstr(format, "coo")) {
            ptime = get_wtime();
            err = conjgrad_coo(vals, b, nnz, nrow, maxiter, cols, rows, x);
        } else if (strstr(format, "csr")) {
            ptime = get_wtime();
            err = conjgrad_csr(vals, b, nrow, maxiter, cols, rowptr, x);
        } else if (strstr(format, "csb")) {
            ptime = get_wtime();
            err = conjgrad_csb(bval, b, bs, nrow, nb, maxiter, bptr, brow, bcol, erow, ecol, x);
        } else if (strstr(format, "dsr")) {
            ptime = get_wtime();
            err = conjgrad_dsr(vals, b, nrow, nzr, maxiter, cols, crow, crp, x);
        } else if (strstr(format, "ell")) {
            ptime = get_wtime();
            err = conjgrad_ell(lval, b, nell, nrow, maxiter, lcol, x);
        } else if (strstr(format, "dia")) {
            ptime = get_wtime();
            err = conjgrad_dia(dval, b, ndia, nrow, maxiter, doff, x);
        } else {
            fprintf(stderr, "%s: Unrecognized format: '%s'\n", name, format);
            return -1;
        }

        niter = maxiter;
#endif

#ifdef LIKWID_PERF
        LIKWID_MARKER_STOP(name);
#endif

#ifdef VTUNE_PERF
        __itt_pause(); // stop VTune
    __SSC_MARK(0x222); // stop SDE tracing
#endif

        ptime = get_wtime() - ptime;
        tsum += ptime;
    }

#ifdef LIKWID_PERF
    LIKWID_MARKER_CLOSE
#endif

    if (pid < 1) {
        if (strstr(format, "coo")) {
            size = 2 * sizeof(int) * nnz + sizeof(double) * nnz;
        } else if (strstr(format, "csr")) {
            size = sizeof(int) * (nnz + nrow + 1) + sizeof(double) * nnz;
        } else if (strstr(format, "csb")) {
            size = sizeof(int) * (3 * nb + 1) + (sizeof(char) * 2 * nnz) + (sizeof(double) * nnz);
        } else if (strstr(format, "dsr")) {
            size = sizeof(int) * (nnz + nzr + nzr + 1) + sizeof(double) * nnz;
        } else if (strstr(format, "ell")) {
            size = sizeof(int) * (1 + (nrow * nell)) + sizeof(double) * (nrow * nell);
        } else if (strstr(format, "dia")) {
            size = sizeof(int) * (1 + (nrow * ndia)) + sizeof(double) * (nrow * ndia);
        }

        fprintf(stdout, "%s: format=%s,niter=%d,x=%lf,nprocs=%d,nruns=%d,exec-time=%lf,insp-time=%lf,size=%u\n",
            name, format, niter, x[0], nproc, nruns, tsum / (double) nruns, itime, size);
    }

    teardown(&rows, &cols, &vals, &x, &b);

    return 0;
}
