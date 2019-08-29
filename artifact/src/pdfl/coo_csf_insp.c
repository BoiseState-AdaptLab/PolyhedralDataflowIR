#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <sys/time.h>

//#define SPLATT_ENABLED 1
#ifdef SPLATT_ENABLED
#include <splatt.h>
#endif

#define EPS 0.01
#define abs(x) (((x) < 0.0) ? -(x) : (x))
#define offset2(i,j,N) ((i)*(N)+(j))
#define offset3(i,j,k,M,N) ((k)+(N)*((j)+(M)*(i)))
#define array_init(ptr,val,size) for(unsigned __i__=0; __i__<(size);__i__++) (ptr)[__i__]=(val)
#define min(x,y) (((x)<(y))?(x):(y))
#define max(x,y) (((x)>(y))?(x):(y))
//#define intdiv(x,y) ((y)>0)?((x)/(y)):0
#define intdiv(x,y) ((x)/(y))
#define floord(n,d) intdiv((n),(d))
//#define BDEBUG 1

typedef float real;
//typedef uint_fast32_t itype;
typedef int32_t itype;
//typedef unsigned itype;

double get_time();
inline double get_time() { struct timeval tv; gettimeofday(&tv, 0); return (double) tv.tv_sec + (((double) tv.tv_usec) * 1E-6); }

itype _tns_order;

typedef struct
{
  itype nnz;
  itype order;
  real* B;
  itype* dims;
  itype* index;
} coo_data_t;

typedef struct
{
  itype* levels;
  itype** offsets;
  itype** indices;
  real* B_prime;
} csf_data_t;

//#include <iegen_util.h>

typedef struct _coo_node_t {
    itype *coords;
    real val;
    struct _coo_node_t *next;
} coo_node_t;

int coo_tnode_comp(const void *a, const void *b);
inline int coo_tnode_comp(const void *a, const void *b) {
    coo_node_t *tn1 = (coo_node_t*) a;
    coo_node_t *tn2 = (coo_node_t*) b;

    int comp = 0;
    for (itype i = 0; i < _tns_order && !comp; i++) {
        comp = tn1->coords[i] - tn2->coords[i];
    }

    return comp;
}

void coo_read(const char *fpath, coo_data_t *coo) {
    char line[1024];
    char *pch;
    char found = 0;
    unsigned lnum = 0;
    unsigned nnz = 0;
    unsigned order = 0;
    unsigned pos = 0;
    coo_node_t *nodes = NULL;

    FILE *in = fopen(fpath, "r");

    while (!found && fgets(line, sizeof(line), in) != NULL) {
        found = (strlen(line) > 0 && line[0] != '#');
    }

    if (found) {
        // Tokenize 1st line and get order
        for (pch = strtok (line, " "); pch != NULL; pch = strtok (NULL, " ")) {
            order += 1;
        }
        order -= 1;     // Subtract 1 for value

        // Count lines (nonzeros)
        nnz += 1;
        while (fgets(line, sizeof(line), in) != NULL) {
            if (strlen(line) > 0 && line[0] != '#') {
                nnz += 1;
            }
        }

        coo->nnz = nnz;
        coo->order = order;
        coo->dims = calloc(order, sizeof(itype));
        coo->index = calloc(nnz * order, sizeof(itype));
        coo->B = calloc(nnz, sizeof(real));

        nodes = calloc(nnz, sizeof(coo_node_t));
        for (itype i = 0; i < nnz; i++) {
            nodes[i].coords = calloc(order, sizeof(itype));
        }
        rewind(in);

        while (fgets(line, sizeof(line), in) != NULL) {
            if (strlen(line) > 0 && line[0] != '#') {
                pos = 0;
                for (pch = strtok (line, " "); pch != NULL; pch = strtok (NULL, " ")) {
                    if (pos < order) {
                        // offset2(i,j,N) (i)*(N)+(j)
                        itype ndx = (itype) atoi(pch);
                        coo->dims[pos] = max(ndx, coo->dims[pos]);
                        nodes[lnum].coords[pos] = ndx - 1;
                    } else {
                        //coo->vals[lnum] = (real) atof(pch);
                        nodes[lnum].val = (real) atof(pch);
                    }
                    pos += 1;
                }
                lnum += 1;
            }
        }
    }

    if (nodes != NULL) {
        _tns_order = order;
        qsort(nodes, nnz, sizeof(coo_node_t), coo_tnode_comp);

        for (itype i = 0; i < nnz; i++) {
            for (itype j = 0; j < order; j++) {
                coo->index[j * nnz + i] = nodes[i].coords[j];
            }
            coo->B[i] = nodes[i].val;
        }

        for (itype i = 0; i < nnz; i++) {
            free(nodes[i].coords);
        }
        free(nodes);
    }

    fclose(in);
}

coo_data_t* coo_csf_setup(const char* path, real **A, real **C, real **D);
inline coo_data_t* coo_csf_setup(const char* path, real **A, real **C, real **D) {
  coo_data_t* restrict B = calloc(1,sizeof(coo_data_t));

#define read() coo_read(path,B)

  read();

  unsigned I = B->dims[0];
  unsigned K = B->dims[1];
  unsigned L = B->dims[2];
  unsigned J = (I + K) / 2;

    fprintf(stderr, "I=%u,J=%u,K=%u,L=%u,size=%lu\n", I, J, K, L, ((I*J)+(K*J)+(L*J))*sizeof(real));

  *A = calloc(I * J, sizeof(real));
  *C = calloc(K * J, sizeof(real));
  *D = calloc(L * J, sizeof(real));
    for (unsigned i = 0; i < K * J; i++)
        (*C)[i] = (real) (i+1);
    for (unsigned i = 0; i < L * J; i++)
        (*D)[i] = (real) (i*2);

  return B;
}  // coo_csf_setup

csf_data_t* coo_csf_insp(coo_data_t const *coo);
inline csf_data_t* coo_csf_insp(coo_data_t const *coo) {
    itype* restrict dims = coo->dims;
    itype* restrict indptr = coo->index;
    real* restrict B = coo->B;
    //itype* restrict levels = csf->levels;
    csf_data_t* restrict csf = calloc(1, sizeof(csf_data_t));

    itype order = coo->order;
    itype nnz = coo->nnz;
    itype pos[order];
    itype loc[order];
    itype mark[order];

//    levels[0] = 3;
//    levels[1] = 2;
//    levels[2] = 1;
//    levels[3] = 0;

    memset(pos, 0, order * sizeof(itype));
    memset(loc, 0, order * sizeof(itype));
    memset(mark, 0, order * sizeof(itype));

    itype** restrict offsets = calloc(order, sizeof(itype*));
    itype** restrict indices = calloc(order, sizeof(itype*));
    real* restrict B_prime = calloc(nnz, sizeof(real));

    // Copy dimensions and size up arrays...
    itype offsizes[order];
    offsizes[0] = 2;
    for (itype n = 1; n < order; n++) {
        offsizes[n] = nnz + 1;
    }
    for (itype n = 0; n < order; n++) {
        offsets[n] = calloc(offsizes[n], sizeof(itype));  // Worst-case for offsets is NNZ+1
        indices[n] = calloc(nnz, sizeof(itype));
    }

    // Peel off m == 0 to initialize indices
    for (itype n = 0; n < order; n++) {
        loc[n] = 0;
        pos[n] = 1;
        indices[n][0] = indptr[n * nnz];
        offsets[n][pos[n]]++;
        //fprintf(stderr, "dim=%u, indices[%u][%u]=%u, offsets[%u][%u]=%u\n", n, n, loc[n], indptr[levels[n] * nnz], n, pos[n], offsets[n][pos[n]]);
        for (itype m = 1; m < nnz; m++) {
            indices[n][m] = UINT32_MAX;
        }
        B_prime[0] = B[0];
    }

    //#pragma omp parallel for schedule(runtime)
    for (itype m = 1; m < nnz; m++) {
        for (itype n = 0; n < order; n++) {
            itype i = indptr[n * nnz + m];
            if (i != indices[n][loc[n]] || mark[n]) {
                loc[n]++;
                indices[n][loc[n]] = i;
                mark[n] = 0;

                offsets[n][pos[n]]++;
                //fprintf(stderr, "dim=%u, indices[%u][%u]=%u, offsets[%u][%u]=%u\n", n, n, loc[n], i, n, pos[n], offsets[n][pos[n]]);

                if (n < order-1) {
                    mark[n+1] = 1;
                    pos[n+1]++;
                    offsets[n+1][pos[n+1]] = offsets[n+1][pos[n+1] - 1];
                }
            }
        }
        // Copy values...
        B_prime[m] = B[m];
    }

    //csf_build(0, levels, order, 0, nnz, nnz, indptr, offsets, indices, pos);

    // Realloc offset and index arrays (all but last)
    for (itype n = 0; n < order - 1; n++) {
        //fprintf(stderr, "nindices[%u]=%u, noffsets[%u]=%u\n", n, loc[n]+1, n, pos[n]+1);
        indices[n] = realloc(indices[n], (loc[n]+1) * sizeof(itype));
        offsets[n] = realloc(offsets[n], (pos[n]+1) * sizeof(itype));
    }

    csf->offsets = offsets;
    csf->indices = indices;
    csf->B_prime = B_prime;

    return csf;
}  // coo_csf_insp


#ifdef SPLATT_ENABLED
void splatt_setup(coo_data_t const *coo, const char *file, splatt_csf **tensor, splatt_idx_t *nmodes, double **opts,
                  real const *A, real const *C, real const *D, splatt_val_t **Aspl, splatt_val_t **Cspl, splatt_val_t **Dspl) {
    // A(i,j) = B(i,k,l) * D(l,j) * C(k,j)
    itype I = coo->dims[0];
    itype K = coo->dims[1];
    itype L = coo->dims[2];
    itype J = (I + K) / 2;

    *opts = splatt_default_opts();        /* allocate default options */
    /* load the tensor from a file */
    int status = splatt_csf_load(file, nmodes, tensor, *opts);
    if (status != SPLATT_SUCCESS) {
        fprintf(stderr, "Error loading tensor file '%s' into SPLATT.\n", file);
    }

    // Convert my matrices into SPLATT matrices:
    *Aspl = calloc(I * J, sizeof(splatt_val_t));
    *Cspl = calloc(K * J, sizeof(splatt_val_t));
    *Dspl = calloc(L * J, sizeof(splatt_val_t));

    for (itype i = 0; i < I * J; i++) {
        (*Aspl)[i] = (splatt_val_t) A[i];
    }
    for (itype i = 0; i < K * J; i++) {
        (*Cspl[i]) = (splatt_val_t) C[i];
    }
    for (itype i = 0; i < L * J; i++) {
        (*Dspl[i]) = (splatt_val_t) D[i];
    }
}

void splatt_cleanup(splatt_csf **Bspl, double **opts, splatt_val_t **Aspl, splatt_val_t **Cspl, splatt_val_t **Dspl) {
    splatt_free_csf(*Bspl, *opts);
    splatt_free_opts(*opts);
    free(*Aspl);
    free(*Cspl);
    free(*Dspl);
}

void splatt_exec(coo_data_t const *coo, splatt_csf *B, splatt_val_t *A, splatt_val_t *C, splatt_val_t *D, double *opts) {
    // A(i,j) = B(i,k,l) * D(l,j) * C(k,j)
    itype I = coo->dims[0];
    itype K = coo->dims[1];
    itype L = coo->dims[2];
    itype J = (I + K) / 2;

    splatt_idx_t mode = 0;
    splatt_idx_t ncols = J;
    splatt_val_t *C_D[2] = {C, D};

    int status = splatt_mttkrp(0,         // Which mode we are operating on.
                               J,         // How many columns each matrix has ('nfactors').
                               B,         // The CSF tensor to multipy with.
                               C_D,       // The row-major dense matrices to multiply with.
                               A,         // The output matrix
                               opts);     // SPLATT options array.
    if (status != SPLATT_SUCCESS) {
        fprintf(stderr, "SPLATT-MTTKRP has failed with error code: %d\n", status);
    }
}
#endif

void taco_exec(coo_data_t const *coo, csf_data_t *B, real *A, real *C, real *D) {
    // A(i,j) = B(i,k,l) * D(l,j) * C(k,j)
    itype I = coo->dims[0];
    itype K = coo->dims[1];
    itype J = (I + K) / 2;

    int A1_dimension = I;
    int A2_dimension = J;
    real* restrict A_vals = A;
    int* restrict B1_pos = (int*)(B->offsets[0]);
    int* restrict B1_coord = (int*)(B->indices[0]);
    int* restrict B2_pos = (int*)(B->offsets[1]);
    int* restrict B2_coord = (int*)(B->indices[1]);
    int* restrict B3_pos = (int*)(B->offsets[2]);
    int* restrict B3_coord = (int*)(B->indices[2]);
    real* restrict B_vals = (B->B_prime);
    int C2_dimension = J;
    real* restrict C_vals = C;
    int D2_dimension = J;
    real* restrict D_vals = D;

    for (int32_t pA = 0; pA < A1_dimension * A2_dimension; pA++) {
        A_vals[pA] = 0.0;
    }
#pragma omp parallel for schedule(dynamic,16)
    for (int32_t pB1 = B1_pos[0]; pB1 < B1_pos[1]; pB1++) {
        int32_t iB = B1_coord[pB1];
        for (int32_t pB2 = B2_pos[pB1]; pB2 < B2_pos[(pB1 + 1)]; pB2++) {
            int32_t kB = B2_coord[pB2];
            for (int32_t pB3 = B3_pos[pB2]; pB3 < B3_pos[(pB2 + 1)]; pB3++) {
                int32_t lB = B3_coord[pB3];
                real tl = B_vals[pB3];
                for (int32_t jC = 0; jC < C2_dimension; jC++) {
                    int32_t pC2 = kB * C2_dimension + jC;
                    int32_t pD2 = lB * D2_dimension + jC;
                    int32_t pA2 = iB * A2_dimension + jC;
                    A_vals[pA2] = A_vals[pA2] + tl * C_vals[pC2] * D_vals[pD2];
                }
            }
        }
    }
}

// TODO: Make structs const in code generator.
void coo_csf_exec(coo_data_t const *coo, csf_data_t const *csf, real *A, real const *C, real const *D);
inline void coo_csf_exec(coo_data_t const *coo, csf_data_t const *csf, real *A, real const *C, real const *D) {
    itype I = coo->dims[0];
    itype K = coo->dims[1];
    itype J = (I + K) / 2;
    real* restrict B = csf->B_prime;
    itype** restrict offsets = csf->offsets;
    itype** restrict indices = csf->indices;

    #pragma omp parallel for schedule(runtime) default(shared)
    for (itype p = offsets[0][0]; p < offsets[0][1]; p++) {   // 0-4
        itype i = indices[0][p];        // 0, 1, 2, 3
        for (itype q = offsets[1][p]; q < offsets[1][p+1]; q++) {
            itype k = indices[1][q];    // 0, 1, 0, 0, 1, 2, 0, 3
            for (itype r = offsets[2][q]; r < offsets[2][q + 1]; r++) {     // Leaf level => same size as value array
                itype l = indices[2][r];  // 0, 0, 0, 2, 0, 2, 1, 2
                real vB = B[r];
                #pragma omp simd
                for (itype j = 0; j < J; j++) {
                    A[offset2(i, j, J)] += vB * C[offset2(k, j, J)] * D[offset2(l, j, J)];
                }
            }
        }
    }
}  // coo_csf_exec

void coo_exec(coo_data_t const *coo, real *A, real const *C, real const *D) {
    real* restrict B = coo->B;
    itype i, j, k, l, m;
    itype M = coo->nnz;
    itype I = coo->dims[0];
    itype K = coo->dims[1];
    itype J = (I + K) / 2;
    itype* restrict index = coo->index;

    #pragma omp parallel for schedule(runtime)
    for (m = 0; m < M; m++) {
        i = index[offset2(0, m, M)];
        k = index[offset2(1, m, M)];
        l = index[offset2(2, m, M)];
        for (j = 0; j < J; j++) {
            //fprintf(stderr, "i=%d,k=%d,l=%d,j=%d,r=%d,v=%g\n", i, k, l, j, m, B->vals[m]);
            A[offset2(i, j, J)] += B[m] * C[offset2(k, j, J)] * D[offset2(l, j, J)];
        }
    }
}

int coo_csf_verify(coo_data_t const *coo, csf_data_t const *csf, real *A, real const *C, real const *D);
inline int coo_csf_verify(coo_data_t const *coo, csf_data_t const *csf, real *A, real const *C, real const *D) {
    itype M = coo->nnz; // What is M, m indices into vals, must be NNZ!
    itype I = coo->dims[0];
    itype K = coo->dims[1];
    itype J = (I + K) / 2;
    real* restrict A2 = calloc(I * J, sizeof(real));

    double tinit = get_time();
    coo_exec(coo, A2, C, D);
    fprintf(stderr,"coo_exec::%.6lf seconds elapsed\n", (get_time() - tinit));

    for (itype i = 0; i < I * J; i++) {
        if (abs((A2[i] - A[i])/A2[i]) >= EPS) {
            fprintf(stderr, "Values don't match at %u, expected %f obtained %f\n", i, A2[i], A[i]);
            return i;
        }
    }

    free(A2);
    return -1;
}

int main(int argc, const char ** argv) {
    real *A, *C, *D;

  // 1) Call the setup function
  // TODO: Move vector initializations into setup function, so will not be included in timing.
  coo_data_t* coo = coo_csf_setup(argv[1], &A, &C, &D);

  //double tstart = get_time();

  // 2) Call the inspector
  double tinsp = get_time();
  csf_data_t* csf = coo_csf_insp(coo);
  fprintf(stderr,"coo_csf_insp::%.6lf seconds elapsed\n", (get_time() - tinsp));

  // 3) Call the executor
  double texec = get_time();
  coo_csf_exec(coo, csf, A, C, D);
  fprintf(stderr,"csf_exec::%.6lf seconds elapsed\n", (get_time() - texec));

  //printf(stderr,"coo_csf::%.6lf seconds elapsed\n", (get_time() - tstart));

  // 4) Call TACO compute kernel
  double ttaco = get_time();
  taco_exec(coo, csf, A, C, D);
  fprintf(stderr,"taco_exec::%.6lf seconds elapsed\n", (get_time() - ttaco));

#ifdef SPLATT_ENABLED
    // Call SPLATT setup
    splatt_csf *Bspl;
    splatt_idx_t ndim;
    double *opts;
    splatt_val_t *Aspl, *Cspl, *Dspl;
    splatt_setup(coo, argv[1], &Bspl, &ndim, &opts, A, C, D, &Aspl, &Cspl, &Dspl);

    // 5) Call SPLATT kernel
    double tsplatt = get_time();
    splatt_exec(coo, Bspl, Aspl, Cspl, Dspl, opts);
    fprintf(stderr,"splatt_exec::%.6lf seconds elapsed\n", (get_time() - tsplatt));
    splatt_cleanup(&Bspl, &opts, &Aspl, &Cspl, &Dspl);
#endif

    // TODO: Add verify function
  return coo_csf_verify(coo, csf, A, C, D) < 0;
}  // main
