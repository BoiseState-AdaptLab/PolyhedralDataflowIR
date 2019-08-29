#include "smv.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include<string.h>
#include <sys/time.h>

void smv(struct sparse_matrix *A, REAL *x, REAL *y)
{
	int row;
	int col_index;
	REAL temp;

        int i;
	if (DEBUG) 
		fprintf(stderr, "smv::Inside smv method\n");

    // Manu
	int *cols = A->cols;
	REAL *vals = A->vals;
	int *rows = A->rows; 

    //  	for(i=0; i < 100; i++){
	for(row = 0; row < A->nrows; row++){
		temp = 0;
		for(col_index = rows[row]; col_index < rows[row+1]; col_index++){
			temp+=vals[col_index] * x[cols[col_index]];
		}
		y[row]=temp;
	}
    //  }

	if (DEBUG)
		fprintf(stderr, "smv::Done with smv\n");

}
