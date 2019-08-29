#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <sys/time.h>

#define min(x,y) (((x)<(y))?(x):(y))
#define max(x,y) (((x)>(y))?(x):(y))
//#define intdiv(x,y) ((y)>0)?((x)/(y)):0
#define intdiv(x,y) ((x)/(y))
#define floord(n,d) intdiv((n),(d))
//#define BDEBUG 1

typedef float real;
//typedef uint_fast8_t itype;
//typedef uint_least16_t itype;
typedef unsigned itype;

double get_time();
inline double get_time() { struct timeval tv; gettimeofday(&tv, 0); return (double) tv.tv_sec + (((double) tv.tv_usec) * 1E-6); }

typedef struct
{
  unsigned N_R;
  int N_C;
  int NNZ;
  real* A;
  itype* col;
  itype* index;
} csr_data_t;

typedef struct
{
  int R;
  int C;
  int N_R;
  int N_C;
  itype* bset;
  itype nb;
  itype* b_col;
  itype* b_index;
  real* A_prime;
} bsr_data_t;

#include <iegen_util.h>

csr_data_t* csr_bsr_setup(const char* path, real **x, real **y);
inline csr_data_t* csr_bsr_setup(const char* path, real **x, real **y) {
  csr_data_t* restrict csr = calloc(1,sizeof(csr_data_t));

#define read() csr_read(path,csr)

  read();

  *x = calloc(csr->N_C, sizeof(real));
  array_init(*x, 1.0, csr->N_C);
  *y = calloc(csr->N_R, sizeof(real));
  //array_init(*y, 0, csr->N_R);

  return csr;
}  // csr_bsr_setup

bsr_data_t* csr_bsr_insp(csr_data_t const *csr, itype R, itype C);
inline bsr_data_t* csr_bsr_insp(csr_data_t const *csr, itype R, itype C) {
  int N_R = csr->N_R;
    int N_C = csr->N_C;
//  itype NNZ = csr->NNZ;
  itype b_set[N_C/C+1];
  itype nb = 0;
  itype b = nb;
  itype nb_max = (N_R/R)*(N_C/C);
  itype nb_est = nb_max / 10;
  itype gf=2;

  real* restrict A = csr->A;
  itype* restrict col = csr->col;
  itype* restrict index = csr->index;

  itype* restrict b_index = calloc(N_R/R+1,sizeof(itype));
  itype* restrict b_col = calloc(nb_est, sizeof(itype));
  real* restrict A_prime = calloc(nb_est*R*C,sizeof(real));

#define lookup(ii,i,j,k) b=b_set[(k)/C]
//#define extract(ii,i,j,k) b_col[nb]=(k)/C
#define extract(ii,i,j,k) { if (nb == nb_est) {\
  nb_est *= gf;\
  b_col = realloc(b_col, nb_est * sizeof(itype));\
  A_prime = realloc(A_prime, nb_est * R * C * sizeof(real)); }\
b_col[nb]=(k)/C; }
//#define count(ii,i,j,k) b=++nb
#define count(ii,i,j,k) nb += !b
#define insert(ii,i,j,k) { b_set[(k)/C] = b ? b : nb; b=b_set[(k)/C]; }
#define offset(ii,i,j,k) b_index[(ii)+1] = nb
#define copy(ii,i,j,k) A_prime[offset3((b)-1,((i)-(ii)*R),((k)-((k)/C)*C),R,C)]=A[(j)]
#define clear(ii) memset(b_set, 0, (N_C/C+1) * sizeof(itype))
// R_A->A := {[i,j] -> [ii,i,j,k] : ri=i-ii*R && ck=k-(k/C)*C && A_prime(k/C,ri,ck)=A(j)\}

//itype ub = floord(N_R-1,R);
itype ub = N_R/R-(N_R%R != 0);
for(itype ii = 0; ii <= ub; ii++) {
  clear(ii);
  for(itype i = R*ii; i <= min(N_R-1,R*ii+(R-1)); i++) {
    for(itype j = index[i]; j <= index[i+1]-1; j++) {
      itype k=col[j];
      lookup(ii, i, j, k);
      extract(ii, i, j, k);
      count(ii, i, j, k);
      insert(ii, i, j, k);
      copy(ii,i,j,k);
    }
  }
  offset(ii,i,j,k);
}

  // Now realloc b_col and A_prime to be the size they should be...
  b_col = realloc(b_col, nb * sizeof(itype));
  A_prime = realloc(A_prime, nb*R*C * sizeof(real));

  bsr_data_t* bsr = calloc(1, sizeof(bsr_data_t));
  bsr->R = R;
  bsr->C = C;
  bsr->N_R = N_R;
  bsr->N_C = N_C;
  bsr->nb = nb;
  bsr->b_index = b_index;
  bsr->A_prime = A_prime;
  bsr->b_col = b_col;

#ifdef BDEBUG
    fprintf(stderr,"nb=%d,nd=%d\n", nb, N_C/C);
    fprintf(stderr, "b_index(%d): [", N_R/R+1);
    for (itype i = 0; i <= N_R/R; i++) {
        fprintf(stderr, "%u ", b_index[i]);
    }
    fprintf(stderr, "]\n");
    fprintf(stderr, "b_col(%d): [", nb);
    for (itype i = 0; i < bsr->nb; i++) {
        fprintf(stderr, "%u ", b_col[i]);
    }
    fprintf(stderr, "]\n");
#endif

  return bsr;
}  // csr_bsr_insp

void bsr_write(bsr_data_t const *bsr) {
    int R = bsr->R;
    int C = bsr->C;

    FILE *out = fopen("csr-bsr.csv", "w");
    fprintf(out, "N_R,N_C,NB,ND,R,C,Size\n");
    fprintf(out, "%d,%d,%d,%d,%d,%d,%d\n", bsr->N_R, bsr->N_C, bsr->nb, bsr->N_C/bsr->C, R, C, bsr->nb * R * C);
    fprintf(out, "ii,jj,kk,i,k,val\n");

//#define write(ii,jj,i,k) fprintf(out, "%d,%d,%d,%d,%d,%f\n", (ii), (jj), bsr->b_col[(jj)], (i), (k), bsr->A_prime[offset3((jj),(i)-R*(ii), (k)-C*bsr->b_col[(jj)], R, C)])
#define write(ii,jj) fprintf(out, "%d,%d,%d\n", (ii), (jj), bsr->b_col[(jj)])

    for(itype ii = 0; ii < (bsr->N_R-1)/R; ii++) {
        for (itype jj = bsr->b_index[ii]; jj < bsr->b_index[ii+1]; jj++) {
            write(ii, jj);
//            for (itype i = R * ii; i <= min(bsr->N_R-1, R * ii + (R - 1)); i++) {
//                for (itype k = C * bsr->b_col[jj]; k <= min(C * bsr->b_col[jj] + (C - 1), bsr->N_C-1); k++) {
//                    write(ii, jj, i, k);
//                }
//            }
        }
    }

    fclose(out);
}

// TODO: Make structs const in code generator.
void csr_bsr_exec(csr_data_t const *csr, bsr_data_t const *bsr, real const *x, real *y);
inline void csr_bsr_exec(csr_data_t const *csr, bsr_data_t const *bsr, real const *x, real *y) {
  int R = bsr->R;
  int C = bsr->C;
  itype N_R = bsr->N_R;
  itype N_C = bsr->N_C;

  real ytmp[R];
  real xtmp[C];
//  real Atmp[R*C];

  // TODO: Only unroll struct members that are used within the code.
  //itype* restrict bset = bsr->bset;
  //itype nb = bsr->nb;
  itype* restrict b_col = bsr->b_col;
  itype* restrict b_index = bsr->b_index;
  real* restrict A_prime = bsr->A_prime;

  itype* restrict index = csr->index;
  itype* restrict col = csr->col;
  real* restrict A = csr->A;

  //bsr_write(bsr);

    // TODO: Move vector initializations into setup function, so will not be included in timing.
//  real x[N_C];
//  array_init(x, 1.0, N_C);
//  real y[N_R];
//  array_init(y, 0, N_R);

//#define spmv(ii,jj,i,k,t) y[(i)]+=A_prime[offset3((jj),(i)-R*(ii), (k)+(t)-C*b_col[(jj)], R, C)]*x[(k)+(t)]
//#define spmv(ii,jj,i,k,t) y[(i)]+=A_prime[offset3((jj),(i)-R*(ii), (k)+(t)-C*b_col[(jj)], R, C)]*xtmp[(t)]
//#define spmv(ii,jj,i,k,s,t) ytmp[(s)]+=Atmp[offset2((i)+(s)-R*(ii), (k)+(t)-C*b_col[(jj)], C)]*xtmp[(t)]
#define spmv(ii,jj,i,k,s,t) ytmp[(s)]+=A_prime[offset3((jj),(i)+(s)-R*(ii), (k)+(t)-C*b_col[(jj)], R, C)]*xtmp[(t)]

    //itype ub = floord(N_R-1,R);
    itype ub = N_R/R-(N_R%R != 0);

    //#pragma omp parallel for schedule(runtime)
    for(itype ii = 0; ii <= ub; ii++) {
        #pragma omp simd
        for(itype jj = b_index[ii]; jj <= b_index[ii+1]-1; jj++) {
            //for(itype i = R*ii; i < R*(ii+1); i++) {
            //for(itype i = R*ii; i <= min(N_R-1,R*ii+(R-1)); i++) {
            itype i = R*ii;
            ytmp[0] = y[i];
            ytmp[1] = y[i+1];
            ytmp[2] = y[i+2];
            ytmp[3] = y[i+3];
            ytmp[4] = y[i+4];
            ytmp[5] = y[i+5];
            ytmp[6] = y[i+6];
            ytmp[7] = y[i+7];

//            for (itype ri = 0; ri < R; ri++) {
//              Atmp[ri*C+0] = A_prime[offset3((jj),ri,0,R,C)];
//              Atmp[ri*C+1] = A_prime[offset3((jj),ri,1,R,C)];
//              Atmp[ri*C+2] = A_prime[offset3((jj),ri,2,R,C)];
//              Atmp[ri*C+3] = A_prime[offset3((jj),ri,3,R,C)];
//              Atmp[ri*C+4] = A_prime[offset3((jj),ri,4,R,C)];
//              Atmp[ri*C+5] = A_prime[offset3((jj),ri,5,R,C)];
//              Atmp[ri*C+6] = A_prime[offset3((jj),ri,6,R,C)];
//              Atmp[ri*C+7] = A_prime[offset3((jj),ri,7,R,C)];
//            }

            //for(itype k = C*b_col[jj]; k <= min(C*b_col[jj]+(C-1),N_C-1); k++) {
            itype k = C*b_col[jj];
            xtmp[0] = x[k];
            xtmp[1] = x[k+1];
            xtmp[2] = x[k+2];
            xtmp[3] = x[k+3];
            xtmp[4] = x[k+4];
            xtmp[5] = x[k+5];
            xtmp[6] = x[k+6];
            xtmp[7] = x[k+7];

            spmv(ii,jj,i,k,0,0);
            spmv(ii,jj,i,k,0,1);
            spmv(ii,jj,i,k,0,2);
            spmv(ii,jj,i,k,0,3);
            spmv(ii,jj,i,k,0,4);
            spmv(ii,jj,i,k,0,5);
            spmv(ii,jj,i,k,0,6);
            spmv(ii,jj,i,k,0,7);

            spmv(ii,jj,i,k,1,0);
            spmv(ii,jj,i,k,1,1);
            spmv(ii,jj,i,k,1,2);
            spmv(ii,jj,i,k,1,3);
            spmv(ii,jj,i,k,1,4);
            spmv(ii,jj,i,k,1,5);
            spmv(ii,jj,i,k,1,6);
            spmv(ii,jj,i,k,1,7);

            spmv(ii,jj,i,k,2,0);
            spmv(ii,jj,i,k,2,1);
            spmv(ii,jj,i,k,2,2);
            spmv(ii,jj,i,k,2,3);
            spmv(ii,jj,i,k,2,4);
            spmv(ii,jj,i,k,2,5);
            spmv(ii,jj,i,k,2,6);
            spmv(ii,jj,i,k,2,7);

            spmv(ii,jj,i,k,3,0);
            spmv(ii,jj,i,k,3,1);
            spmv(ii,jj,i,k,3,2);
            spmv(ii,jj,i,k,3,3);
            spmv(ii,jj,i,k,3,4);
            spmv(ii,jj,i,k,3,5);
            spmv(ii,jj,i,k,3,6);
            spmv(ii,jj,i,k,3,7);

            spmv(ii,jj,i,k,4,0);
            spmv(ii,jj,i,k,4,1);
            spmv(ii,jj,i,k,4,2);
            spmv(ii,jj,i,k,4,3);
            spmv(ii,jj,i,k,4,4);
            spmv(ii,jj,i,k,4,5);
            spmv(ii,jj,i,k,4,6);
            spmv(ii,jj,i,k,4,7);

            spmv(ii,jj,i,k,5,0);
            spmv(ii,jj,i,k,5,1);
            spmv(ii,jj,i,k,5,2);
            spmv(ii,jj,i,k,5,3);
            spmv(ii,jj,i,k,5,4);
            spmv(ii,jj,i,k,5,5);
            spmv(ii,jj,i,k,5,6);
            spmv(ii,jj,i,k,5,7);

            spmv(ii,jj,i,k,6,0);
            spmv(ii,jj,i,k,6,1);
            spmv(ii,jj,i,k,6,2);
            spmv(ii,jj,i,k,6,3);
            spmv(ii,jj,i,k,6,4);
            spmv(ii,jj,i,k,6,5);
            spmv(ii,jj,i,k,6,6);
            spmv(ii,jj,i,k,6,7);

            spmv(ii,jj,i,k,7,0);
            spmv(ii,jj,i,k,7,1);
            spmv(ii,jj,i,k,7,2);
            spmv(ii,jj,i,k,7,3);
            spmv(ii,jj,i,k,7,4);
            spmv(ii,jj,i,k,7,5);
            spmv(ii,jj,i,k,7,6);
            spmv(ii,jj,i,k,7,7);

//          for (k += 8; k <= min(C*b_col[jj]+(C-1),N_C-1); k++) {
//            spmv(ii,jj,i,k,0);
//          }

            y[i] = ytmp[0];
            y[i+1] = ytmp[1];
            y[i+1] = ytmp[1];
            y[i+2] = ytmp[2];
            y[i+3] = ytmp[3];
            y[i+4] = ytmp[4];
            y[i+5] = ytmp[5];
            y[i+6] = ytmp[6];
            y[i+7] = ytmp[7];
            //}

            //fprintf(stderr, "i_out=%u, k_out=%u\n", i+8, k+8);
        }
    }

#define csr(i,j) y[(i)]+=A[(j)]*x[col[(j)]]

  //fprintf(stderr, "i_in=%u, k_in=%u\n", (N_R/R)*R, col[index[(N_R/R)*R]]);

  for (itype i = (N_R/R)*R; i <= N_R-1; i += 1)
    for (itype j = index[i]; j <= index[i+1]-1; j += 1)
      csr(i, j);

  //fprintf(stderr, "i_out=%u, k_out=%u\n", N_R-1, col[index[N_R-1]]);
}  // csr_bsr_exec

int main(int argc, const char ** argv) {
    real *x, *y;

  // 1) Call the setup function
  // TODO: Move vector initializations into setup function, so will not be included in timing.
  csr_data_t* csr = csr_bsr_setup(argv[1], &x, &y);

  double tstart = get_time();

  // 2) Call the inspector
  double tinsp = get_time();
  bsr_data_t* bsr = csr_bsr_insp(csr, atoi(argv[2]), atoi(argv[3]));
  fprintf(stderr,"csr_bsr_insp::%.6lf seconds elapsed\n", (get_time() - tinsp));

  fprintf(stderr, "csr_bsr_insp::NNZ=%u,NR=%u,NC=%u,NB=%u,ND=%u,R=%u,C=%u\n",
          csr->NNZ, bsr->N_R, bsr->N_C, bsr->nb, bsr->N_C/bsr->C, bsr->R, bsr->C);

  // 3) Call the executor
  double texec = get_time();
  csr_bsr_exec(csr, bsr, x, y);
  fprintf(stderr,"csr_bsr_exec::%.6lf seconds elapsed\n", (get_time() - texec));

  fprintf(stderr,"csr_bsr::%.6lf seconds elapsed\n", (get_time() - tstart));

  // TODO: Add verify function
  csr_bsr_verify(csr, bsr, x, y);
}  // main
