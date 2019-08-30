
#include "matrix_io.h"
#include <stdio.h>
#include <stdlib.h>
#include<string.h>


void load_sparse_matrix(char *filename, struct sparse_matrix *A)
{
	FILE *in;
	char data[1024];
	int i;
	in = fopen(filename, "r");
	if(in==NULL){
		printf("something might be wrong with the file\n");
	}
	fgets(data, 1024, in);
	//fprintf(stderr, "%s", data);
	fscanf(in, "%d %d %d\n", &(A->nrows),&(A->ncols), &(A->nnz));
	if (DEBUG)
		fprintf(stderr, "load_sparse_matrix:: rows = %d, cols= %d nnz = %d\n", A->nrows,A->ncols,A->nnz);


	A->rows = (int *) malloc(sizeof(int) * ((A->nrows) + 1));
	A->cols = (int *) malloc(sizeof(int) * (A->nnz));
	A->vals = (REAL *) malloc(sizeof(REAL) * (A->nnz));
        A->max_row_len = -1;
	if (DEBUG)
                fprintf(stderr, "load_sparse_matrix::reading row index\n");

	for(i = 0; i <= A->nrows; i++){
		int temp;
		fscanf(in, "%d", &temp);
		temp--;
		A->rows[i] = temp;
                if(i > 0)
                  if(A->rows[i] - A->rows[i-1] > A->max_row_len)
                   A->max_row_len = A->rows[i] - A->rows[i-1];
	}

	if (DEBUG)
                fprintf(stderr, "load_sparse_matrix::reading column index\n");

	for(i = 0; i < A->nnz; i++){
		int temp;
		fscanf(in, "%d", &temp);
		temp--;
		//fprintf(stderr, "%d\n", temp);
		A->cols[i] = temp;
	        if(A->cols[i] < 0)
		  fprintf(stderr, "%d\n", A->cols[i]);
	}

	if (DEBUG)
                fprintf(stderr, "load_sparse_matrix::reading values\n");

	for(i = 0; i < A->nnz; i++){
		char temp[20];
		REAL data;
		fscanf(in, "%s", temp);
		data = atof(temp);
		//fprintf(stderr, "%f\n", data);
		A->vals[i] = data; //1.0;
	}

	if (DEBUG)
                fprintf(stderr, "load_sparse_matrix::Loading sparse matrix done\n");
}

