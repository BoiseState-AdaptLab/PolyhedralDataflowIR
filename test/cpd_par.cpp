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

// Inspectors
#include "coo_csf_insp.h"
//#include "coo_hicoo_insp.h"

// Executors
#include "mttkrp_csf.h"
//#include "mttkrp_hicoo.h"

double get_wtime() {
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return (double) tv.tv_sec + (((double) tv.tv_usec) * 1E-6);
}

void setup(const char* tnsfile, const unsigned rank, unsigned* nnz, unsigned* maxiter, unsigned* order, unsigned** dims,
           unsigned** indices, float** __restrict vals, float*** __restrict factors, float** __restrict lambda) {
    *maxiter = 50;

    // Read tensor file
    TensorIO tns(tnsfile, rank);
    tns.read();
    *nnz = tns.nnz();
    *order = tns.order();

    unsigned nbytes = (*order) * (*nnz) * sizeof(unsigned);
    *indices = (unsigned*) malloc(nbytes);
    memcpy(*indices, tns.indices(), nbytes);

    nbytes = *order * sizeof(unsigned);
    *dims = (unsigned*) malloc(nbytes);
    memcpy(*dims, tns.dims(), nbytes);

    nbytes = *nnz * sizeof(float);
    *vals = (float*) malloc(nbytes);
    memcpy(*vals, tns.vals(), nbytes);

    *factors = (float**) calloc(*order, sizeof(float*));
    for (unsigned n = 0; n <  *order; n++) {
        (*factors)[n] = (float*) calloc((*dims)[n] * rank);
    }

    *lambda = (float*) calloc(rank, sizeof(float));
}

void teardown(unsigned order, unsigned** indices, unsigned** dims, float** __restrict* vals, float*** __restrict factors, float** __restrict lambda) {
    free(*indices);
    free(*dims);
    free(*vals);
    free(*lambda);
    for (unsigned n = 0; n <  *order; n++) {
        free((*factors)[n]);
    }
    free(*factors);
}

int main(int argc, char **argv) {
    double ptime;
    double itime;
    double tsum = 0.0;
    double isum = 0.0;
    int nproc = 1;
    int pid = 0;
    int nruns = 1;
    int rank = 10;
    
    const char* name = argv[0];
    const char* tensor = argv[1];
    char format[16] = {'\0'};

    unsigned nnz, order, maxiter = 500;
    unsigned niter = 0;
    unsigned size = 0;
    unsigned* indices;
    unsigned* dims;

    unsigned** fptr;
    unsigned** findx;
    unsigned* bptr;
    unsigned* bindices;
    unsigned char* eindices;
    unsigned nb;
    unsigned bs = 128;
    double* bval;

    double err = 1.0, tol = 1e-10;
    double* __restrict vals;
    double** __restrict factors;
    double* __restrict lambda;

#ifdef SPLATT_EXEC
    // SPLATT Objects:
    VectorXd xVec, bVec;
    SparseMatrix<double> Aspm;
    ConjugateGradient<SparseMatrix<double>, Lower|Upper> cg;
#endif

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
        rank = atoi(argv[4]);
    } else {
        rank = 10;
    }

    if (argc > 5) {
        bs = atoi(argv[5]);
    } else {
        bs = 128;
    }

    setup(tensor, rank, &nnz, &maxiter, &order, &dims, &indices, &vals, &factors, &lambda);

#ifdef LIKWID_PERF
    LIKWID_MARKER_INIT
#endif

    if (strlen(format) > 0) {
        itime = get_wtime();
        if (!strncmp(format, "csf", 3)) {
            coo_csf_insp(dims, indices, nnz, order, &fptr, &findx);
        } else if (!strncmp(format, "csb", 3) || !strncmp(format, "hic", 3)) {
            nb = coo_hicoo_insp(vals, bs, dims, indices, nnz, order, &bval, &bindices, &bptr, &eindices);
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

#ifdef SPLATT_EXEC
        ptime = get_wtime();
        cg.compute(Aspm);
        xVec = cg.solve(bVec);
        niter = cg.iterations();
        err = cg.error();
        x = xVec.data();
#else
        if (strstr(format, "coo")) {
            ptime = get_wtime();
            err = cpd_coo(vals, dims, maxiter, nnz, order, indices, factors, lambda);
        } else if (strstr(format, "csf")) {
            ptime = get_wtime();
            err = cpd_csf(vals, dims, maxiter, order, findx, fptr, factors, lambda);
        } else if (strstr(format, "csb") || strstr(format, "hic")) {
            ptime = get_wtime();
            err = cpd_hicoo(bval, bs, dims, nb, maxiter, order, bptr, bindices, eindices, lambda);
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
            size = (nnz * order * sizeof(int)) + sizeof(float) * nnz;
        } else if (strstr(format, "csf")) {
            size = (sizeof(int) * (nnz + fptr[0][1] + 1)) + sizeof(float) * nnz;
        } else if (strstr(format, "csb") || strstr(format, "hic")) {
            size = (sizeof(int) * ((order + 1) * nb + 1)) + (sizeof(char) * order* nnz) + (sizeof(float) * nnz);
        })

        fprintf(stdout, "%s: format=%s,niter=%d,A=%lf,B=%lf,C=%lf,lambda=%lf,order=%u,rank=%u,nprocs=%d,nruns=%d,exec-time=%lf,insp-time=%lf,size=%u\n",
            name, format, niter, factors[0][0], factors[1][0], factors[2][0], lambda[0], order, rank, nproc, nruns, tsum / (double) nruns, itime, size);
    }

    teardown(order, &indices, &dims, &vals, &factors, &lambda);

    return 0;
}
