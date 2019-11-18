#include <omp.h>
#include <sys/time.h>

#include <util/LIKWID.hpp>
#include <util/MatrixIO.hpp>
using util::TensorIO;

#include <cstring>
#include <string>
using std::string;
using std::to_string;

#include <vector>
using std::vector;

// Inspectors
#include <coo_csf_insp.h>
#include <coo_hicoo_insp.h>

// Executors
#include <cp_als_nd_coo.h>
#include <cp_als_nd_csf.h>
#include <cp_als_nd_hicoo.h>

double get_wtime() {
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return (double) tv.tv_sec + (((double) tv.tv_usec) * 1E-6);
}

void setup(const char* tnsfile, const unsigned rank, unsigned* nnz, unsigned* order,
           unsigned** dims, unsigned** indices, float** vals, float*** factors, float** lambda) {
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
        (*factors)[n] = (float*) calloc((*dims)[n] * rank, sizeof(float));
    }

    *lambda = (float*) calloc(rank, sizeof(float));
}

void teardown(unsigned order, unsigned** indices, unsigned** dims, float** vals, float*** factors, float** lambda) {
    free(*indices);
    free(*dims);
    free(*vals);
    free(*lambda);
    for (unsigned n = 0; n <  order; n++) {
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

    unsigned nnz, order, maxiter;
    unsigned niter = 0;
    unsigned size = 0;
    unsigned* indices;
    unsigned* dims;

    unsigned** fptr;
    unsigned** findx;
    unsigned* bptr;
    unsigned short* bindices;
    unsigned char* eindices;
    unsigned nb;
    unsigned bs = 128;

    double err = 1.0, tol = 1e-10;
    float* vals;
    float** factors;
    float* lambda;
    float* bval;

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
        maxiter = atoi(argv[4]);
    } else {
        maxiter = 50;
    }

    if (argc > 5) {
        rank = atoi(argv[5]);
    } else {
        rank = 10;
    }

    if (argc > 6) {
        bs = atoi(argv[6]);
    } else {
        bs = 128;
    }

    setup(tensor, rank, &nnz, &order, &dims, &indices, &vals, &factors, &lambda);

#ifdef LIKWID_PERF
    LIKWID_MARKER_INIT
#endif

    if (strlen(format) > 0) {
        itime = get_wtime();
        if (!strncmp(format, "csf", 3)) {
            coo_csf_insp(dims, indices, nnz, order, &fptr, &findx);
        } else if (!strncmp(format, "csb", 3) || !strncmp(format, "hic", 3)) {
            //nb = coo_hicoo_insp(vals, bs, dims, indices, nnz, order, &bval, &bindices, &bptr, &eindices);
            nb = coo_hicoo_insp(vals, bs, nnz, order, dims, indices, &bval, &bindices, &bptr, &eindices);
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

        ptime = get_wtime();
        if (strstr(format, "coo")) {
            //err = cpd_coo(vals, dims, maxiter, nnz, order, indices, factors, lambda);
            cp_als_nd_coo(vals, nnz, order, rank, maxiter, dims, indices, factors, lambda);
        } else if (strstr(format, "csf")) {
            //err = cpd_csf(vals, dims, maxiter, order, findx, fptr, factors, lambda);
            cp_als_nd_csf(vals, nnz, order, rank, maxiter, dims, fptr, findx, factors, lambda);
        } else if (strstr(format, "csb") || strstr(format, "hic")) {
            //err = cpd_hicoo(bval, bs, dims, nb, maxiter, order, bptr, bindices, eindices, lambda);
            cp_als_nd_hicoo(bval, bs, nnz, order, nb, rank, maxiter, dims, bindices, bptr, eindices, factors, lambda);
        } else if (strstr(format, "splatt")) {
            //cpd_als_nd_splatt(vals, nnz, order, rank, maxiter, dims, fptr, findx, factors, lambda);
        } else {
            fprintf(stderr, "%s: Unrecognized format: '%s'\n", name, format);
            return -1;
        }

        niter = maxiter;

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
            unsigned ncrds = 0;
            for(unsigned p = fptr[0][0]; p < fptr[0][1]; p++) {
                for (unsigned q = fptr[1][p]; q < fptr[1][p+1]; q++) {
                    for (unsigned r = fptr[2][q]; r < fptr[2][q+1]; q++) {
                        if (order > 3) {
                            for (unsigned s = fptr[3][r]; s < fptr[3][r+1]; s++) {
                                ncrds++;
                            }
                        } else {
                            ncrds++;
                        }
                    }
                }
            }
            size = ((ncrds + nnz) * sizeof(int)) + (nnz * sizeof(float));
        } else if (strstr(format, "csb") || strstr(format, "hic")) {
            size = (sizeof(int) * ((order + 1) * nb + 1)) + (sizeof(char) * order* nnz) + (sizeof(float) * nnz);
        }

        fprintf(stdout, "%s: format=%s,niter=%d,A=%lf,B=%lf,C=%lf,lambda=%lf,order=%u,rank=%u,nprocs=%d,nruns=%d,exec-time=%lf,insp-time=%lf,size=%u\n",
            name, format, niter, factors[0][0], factors[1][0], factors[2][0], lambda[0], order, rank, nproc, nruns, tsum / (double) nruns, itime, size);
    }

    teardown(order, &indices, &dims, &vals, &factors, &lambda);

    return 0;
}
