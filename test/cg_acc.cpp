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

//#include <Eigen/Sparse>
//#include <Eigen/SparseExtra>
//#include <Eigen/IterativeLinearSolvers>
//using Eigen::VectorXd;
//using Eigen::SparseMatrix;
//using Eigen::ConjugateGradient;
//using Eigen::Lower;
//using Eigen::Upper;
//typedef Eigen::Triplet<double> Triple;

// Inspectors
#include "coo_csr_insp.h"
#include "coo_dsr_insp.h"
#include "coo_ell_insp.h"
#include "coo_ellmap_insp.h"
#include "coo_csb_insp.h"
#include "coo_dia_insp.h"

// Executors
#include "conjgrad_coo_acc.h"
#include "conjgrad_csr.h"
#include "conjgrad_dsr.h"
#include "conjgrad_ell.h"
#include "conjgrad_ellmap.h"
#include "conjgrad_csb.h"
#include "conjgrad_dia.h"

double get_wtime() {
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return (double) tv.tv_sec + (((double) tv.tv_usec) * 1E-6);
}

void setup(const char* matrix, unsigned* nnz, unsigned* nrow, unsigned* ncol, unsigned* maxiter,
           unsigned** rows, unsigned** cols, double* __restrict* vals, double* __restrict* x, double* __restrict* b) {
           //SparseMatrix<double>& Aspm, VectorXd& xVec, VectorXd& bVec) {
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

    *x = (double*) calloc(*ncol, sizeof(double));
    *b = (double*) malloc(*nrow * sizeof(double));

//    xVec.resize(*ncol);
//    bVec.resize(*nrow);

    // Initialize b to something more interesting than 0
    for (unsigned i = 0; i < *nrow; i++) {
        (*b)[i] = 1.0;
        //bVec(i) = (*b)[i];
    }
}

void teardown(unsigned** rows, unsigned** cols, double* __restrict* vals, double* __restrict* x, double* __restrict* b) {
    free(*rows);
    free(*cols);
    free(*vals);
    free(*x);
    free(*b);
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
    unsigned bs;
    double* bval;
    unsigned nzr;
    unsigned* crp;
    unsigned* crow;
    unsigned nell;
    unsigned* lcol;
    unsigned* lmap;
    double* lval;
    unsigned ndia;
    int* doff;
    double** dval;

    double err = 1.0, tol = 1e-10;
    double* __restrict vals;
    double* __restrict x;
    double* __restrict b;

    // Eigen Objects:
//    VectorXd xVec, bVec;
//    SparseMatrix<double> Aspm;
//    ConjugateGradient<SparseMatrix<double>, Lower|Upper> cg;

    #pragma omp parallel num_threads(1)
    {
    nproc = omp_get_max_threads();
    }

    if (argc > 2) {
        nruns = atoi(argv[2]);
    }

    if (argc > 3) {
        strcpy(format, argv[3]);
    } else {
        strcpy(format, "coo");
    }

    if (argc > 4) {
        bs = atoi(argv[4]);
    } else {
        bs = 1;
    }

    setup(matrix, &nnz, &nrow, &ncol, &maxiter, &rows, &cols, &vals, &x, &b); //, Aspm, xVec, bVec);
//    cg.setMaxIterations(maxiter);
//    cg.setTolerance(tol);

#ifdef LIKWID_PERF
    LIKWID_MARKER_INIT
#endif

    // Invoke the inspector...
    if (strlen(format) > 0) {
        itime = get_wtime();
        if (!strncmp(format, "csr", 3)) {
            rowptr = (unsigned*) calloc(nrow + 1, sizeof(unsigned));
            coo_csr_insp(nnz, rows, rowptr);
        } else if (!strncmp(format, "csb", 3)) {
            if (bs < 2) {
                bs = 128;
            }
            nb = coo_csb_insp(vals, bs, nnz, cols, rows, &bval, &bcol, &bptr, &brow, &ecol, &erow);
        } else if (!strncmp(format, "dsr", 3)) {
            nzr = coo_dsr_insp(rows, nnz, &crow, &crp);
        } else if (!strncmp(format, "ell", 3)) {
            nell = coo_ell_insp(nnz, rows, cols, vals, &nrow, &lcol, &lval);
        } else if (strstr(format, "lmap")) {
            nell = coo_ellmap_insp(nnz, rows, cols, vals, &nrow, &lmap);
        } else if (!strncmp(format, "dia", 3)) {
            ndia = coo_dia_insp(vals, nnz, cols, rows, &nrow, &dval, &doff);
//        } else if (!strncmp(format, "eig", 3)) {
//            vector<Triple> triples;
//            triples.reserve(nnz);
//            for (unsigned n = 0; n < nnz; n++) {
//                triples.push_back(Triple((rows)[n], (cols)[n], (vals)[n]));
//            }
//            Aspm.resize(nrow, ncol);
//            Aspm.setFromTriplets(triples.begin(), triples.end());
        } else {
            itime = 0.0;
        }
        if (itime != 0.0) {
            itime = get_wtime() - itime;
        }
    }

    // Time the executor...
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
        } else if (strstr(format, "lmap")) {
            fprintf(stderr, "N=%u,K=%u\n", nrow, nell);
            ptime = get_wtime();
            err = conjgrad_ellmap(vals, b, nell, nrow, maxiter, cols, lmap, x);
        } else if (strstr(format, "dia")) {
            ptime = get_wtime();
            err = conjgrad_dia(dval, b, ndia, nrow, ncol, maxiter, doff, x);
//        } else if (strstr(format, "eig")) {
//            ptime = get_wtime();
//            cg.compute(Aspm);
//            xVec = cg.solve(bVec);
//            niter = cg.iterations();
//            err = cg.error();
        } else {
            fprintf(stderr, "%s: Unrecognized format: '%s'\n", name, format);
            return -1;
        }

#ifdef LIKWID_PERF
        LIKWID_MARKER_STOP(name);
#endif

#ifdef VTUNE_PERF
        __itt_pause(); // stop VTune
    __SSC_MARK(0x222); // stop SDE tracing
#endif

        niter = maxiter;
        ptime = get_wtime() - ptime;
        tsum += ptime;
    }

#ifdef LIKWID_PERF
    LIKWID_MARKER_CLOSE
#endif

    if (pid < 1) {
        double xval = x[0];
//        if (strstr(format, "eig")) {
//            strcpy(format, "csr");
//            xval = xVec[0];
//        } else {
//            xval = x[0];
//        }

        // Compute the data size...
        if (strstr(format, "coo")) {
            size = (2 * sizeof(int) * nnz) + (sizeof(double) * nnz);
        } else if (strstr(format, "csr")) {
            size = (sizeof(int) * (nnz + nrow + 1)) + (sizeof(double) * nnz);
        } else if (strstr(format, "csb")) {
            size = (sizeof(int) * (3 * nb + 1)) + (sizeof(char) * 2 * nnz) + (sizeof(double) * nnz);
        } else if (strstr(format, "dsr")) {
            size = (sizeof(int) * (nnz + 2 * nzr + 1)) + (sizeof(double) * nnz);
        } else if (strstr(format, "ell")) {
            size = (sizeof(int) * (1 + (nrow * nell))) + (sizeof(double) * (nrow * nell));
        } else if (strstr(format, "lmap")) {
            size = (sizeof(int) * nnz) + (sizeof(double) * nnz) + (sizeof(int) * (1 + (nrow * nell)));
        } else if (strstr(format, "dia")) {
            size = (sizeof(int) * (1 + (nrow * ndia))) + (sizeof(double) * (nrow * ndia));
        }

        printf("%s: format=%s,niter=%d,x=%lf,nprocs=%d,nruns=%d,exec-time=%lf,insp-time=%lf,size=%u,bs=%u,matrix='%s'\n",
            name, format, niter, xval, nproc, nruns, tsum / (double) nruns, itime, size, bs, matrix);
    }

    teardown(&rows, &cols, &vals, &x, &b);

    return 0;
}
