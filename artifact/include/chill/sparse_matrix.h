
#ifndef __SPARSE_MATRIX_STRUCT__
#define __SPARSE_MATRIX_STRUCT__

#define SIZE 16
#define BLOCKSIZE 4

#define DEBUG 0
#define BILLION 1E9
typedef float REAL;


struct sparse_matrix {
    int max_row_len;
	int nrows;
	int ncols;	
	int nnz;
	int *rows;
	int *cols;
	REAL *vals;
};

struct sparse_args {
        struct sparse_matrix *s;
        REAL *x;
        REAL *b;
        int nt;
        int id;
};

void spmv(int n,int *index,float *a,float *y,float *x,int *col);
char spmv_comp(int n, float *b, float *b2);

#endif

