#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <sys/time.h>
#include <oski/oski.h>

struct sparse_matrix {
    int max_row_len;
    int nrows;
    int ncols;
    int nnz;
    int *rows;
    int *cols;
    float *vals;
};

double timer() {
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return (double) tv.tv_sec + ((double) tv.tv_usec) * 1E-6;
}

void load_sparse_matrix(char *filename, struct sparse_matrix *A) {
    FILE *in;
    char data[1024];
    int i;
    in = fopen(filename, "r");
    if(in==NULL){
        fprintf(stderr, "something might be wrong with the file\n");
        return;
    }

    fgets(data, 1024, in);
    fscanf(in, "%d %d %d\n", &(A->nrows),&(A->ncols), &(A->nnz));

    A->rows = (int *) malloc(sizeof(int) * ((A->nrows) + 1));
    A->cols = (int *) malloc(sizeof(int) * (A->nnz));
    A->vals = (float *) malloc(sizeof(float) * (A->nnz));
    A->max_row_len = -1;

    for(i = 0; i <= A->nrows; i++){
        int temp;
        fscanf(in, "%d", &temp);
        temp--;
        A->rows[i] = temp;
        if(i > 0)
            if(A->rows[i] - A->rows[i-1] > A->max_row_len)
                A->max_row_len = A->rows[i] - A->rows[i-1];
    }

    for(i = 0; i < A->nnz; i++){
        int temp;
        fscanf(in, "%d", &temp);
        temp--;
        //fprintf(stderr, "%d\n", temp);
        A->cols[i] = temp;
        if(A->cols[i] < 0)
            fprintf(stderr, "%d\n", A->cols[i]);
    }

    for(i = 0; i < A->nnz; i++){
        char temp[20];
        float data;
        fscanf(in, "%s", temp);
        data = atof(temp);
        //fprintf(stderr, "%f\n", data);
        A->vals[i] = data; //1.0;
    }
}

float *csr_spmv(struct sparse_matrix *a, float *x) {
    int i, j;

    int *cols = a->cols;
    float *vals = a->vals;
    int *rows = a->rows;
    float *y = calloc(a->nrows, sizeof(float));

    //double tstart = timer();
    for(i = 0; i < a->nrows; i++){
        for(j = rows[i]; j < rows[i+1]; j++){
            y[i]+=vals[j] * x[cols[j]];
        }
    }
    //fprintf(stderr, "csr_spmv::%.6lf seconds elapsed\n", (timer() - tstart));

    return y;
}

#define R 8
#define C R

char* read_xforms(const char* fname) {
    char *xforms = NULL;
    int nread = 0;
    int fsize = 0;

    FILE* fp = fopen(fname, "rt");
    if (fp) {
        // obtain file size:
        fseek(fp, 0 , SEEK_END);
        fsize = ftell(fp);
        rewind(fp);

        xforms = (char*) malloc(fsize+1);
        nread = fread(xforms, sizeof(char), fsize, fp);
        xforms[nread] = '\0';
        fclose(fp);
    } else {
        fprintf(stderr, "ERROR: Could not read transforms file '%s'\n", fname);
    }
    return xforms;
}

double* oski_spmv(struct sparse_matrix *a, double *x) {
    char *xforms;
    int nrows = a->nrows;
    int ncols = a->ncols;
    int *rows = a->rows;
    int *cols = a->cols;
    float alpha = 1.0, beta = 0.0;
    double tstart;
    double *y = calloc(nrows, sizeof(double));
    double *vals = malloc(sizeof(double) * a->nnz);
    int i, ntimes = 10;

    for (i = 0; i < a->nnz; i++) {
        vals[i] = (double) a->vals[i];
    }

    xforms = read_xforms("./src/oski/xform.txt");

    oski_Init();
//    oski_matrix_t csr = oski_CreateMatCSR(rows, cols, vals, nrows, ncols, SHARE_INPUTMAT,
//                        2, INDEX_ZERO_BASED, INDEX_SORTED); //, LAYOUT_ROWMAJ);
    oski_matrix_t csr = oski_CreateMatCSR( rows, cols, vals, nrows, ncols, /* CSR arrays */
                                   COPY_INPUTMAT, /* ”copy mode” */
            /* remaining args specify how to interpret non−zero pattern */
                                   1, INDEX_ZERO_BASED);

    oski_vecview_t x_view = oski_CreateVecView(x, ncols, STRIDE_UNIT);
    oski_vecview_t y_view = oski_CreateVecView(y, nrows, STRIDE_UNIT);

    //if (xforms == NULL) {
        // Tune it...
        oski_SetHint(csr, HINT_SINGLE_BLOCKSIZE, R, C);

        tstart = timer();
        for (i = 0; i < ntimes; i++) {
            memset(y, 0, nrows * sizeof(double));
            oski_MatMult(csr, OP_NORMAL, alpha, x_view, beta, y_view);
        }

        oski_TuneMat(csr);
        fprintf(stderr, "oski_insp::%.6lf seconds elapsed\n", (timer() - tstart));
    //} else {
    if (xforms != NULL) {
        oski_ApplyMatTransforms(csr, xforms);
    }

    // Run it!
    memset(y, 0, nrows * sizeof(double));
    tstart = timer();
    oski_MatMult(csr, OP_NORMAL, alpha, x_view, beta, y_view);
    fprintf(stderr, "oski_spmv::%.6lf seconds elapsed\n", (timer() - tstart));

    oski_DestroyVecView(x_view);
    oski_DestroyVecView(y_view);
    oski_DestroyMat(csr);
    oski_Close();

    free(vals);

    return y;
}

int vec_comp(int n, float *y, double *y2) {
    int i;
    for (i = 0; i < n; i++) {
        if (fabs((y2[i] - (double) y[i])) >= 0.01) {
            fprintf(stderr, "Values don't match at %d, expected %f obtained %f\n",i,y[i],y2[i]);
            return 0;
        }
    }
    return 1;
}

int main(int argc,char **argv) {
    int i;
    struct sparse_matrix a;

    load_sparse_matrix(argv[1], &a);

    float *x = malloc(sizeof(float) * a.ncols);
    double *x2 = malloc(sizeof(double) * a.ncols);

    for (i = 0; i < a.ncols; i++) {
        x[i] = 1.0f;
        x2[i] = 1.0;
    }

    float *y = csr_spmv(&a, x);

    double *y2 = oski_spmv(&a, x2);

    vec_comp(a.nrows, y, y2);

    free(x);
    free(y);
    free(x2);
    free(y2);
    free(a.rows);
    free(a.cols);
    free(a.vals);
}
