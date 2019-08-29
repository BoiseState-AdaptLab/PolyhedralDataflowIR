#define __rose_lt(x,y) ((x)<(y)?(x):(y))
#define __rose_gt(x,y) ((x)>(y)?(x):(y))
#define _P_DATA2_(_t20) _P_DATA2[_t20]
#define _P_DATA2__(_t20) _P_DATA2[_t20 + 1]
#define _P_DATA3_(_t28,_t29) _P_DATA3[1 * _t29]
#define _P_DATA3___(_t28,_t29) _P_DATA3[1 * _t29 + 1]
#define col_(_t24) col[1 * _t24]
#define col___(_t24) col[1 * _t24 + 1]
#define index_(i) index[1 * i]
#define index__(i) index[1 * i + 1]
#define index___(i) index[1 * i + 1]
#define index____(i) index[1 * i]
//#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "smv.h"
#include "matrix_io.h"
#include "sparse_matrix.h"
//#include "spmv.h"
#include <sys/time.h>
#include <math.h>
//void spmv_bcsr(int n, int *index, float *a, float *y, float *x, int *col,int nnz);

//#define N   62451
//#define N   83334
//#define N   121192
//#define N 39
//#define N     217918
//#define N     206500
//#define N     46835
//#define N    36417 
//#define N    525825 
//#define N    170998 
//#define N    1000005 
//#define N    14822 
#define N    73159 
#define N_R N
#define N_C N
#define R 8
#define C 8
#define ND  N_C/C

struct a_list 
{
  unsigned short col_;
  float a[(R*C)];
  struct a_list *next;
}
;

struct mk 
{
  struct a_list *ptr;
}
;

void spmv(int n,int *index,float *a,float *y,float *x,int *col) {
  int t8;
  int t6;
  int t4;
  int t2;
  int _t91;
  int _t90;
  int _t89;
  int _t88;
  int _t87;
  int _t86;
  int _t85;
  int _t84;
  int _t83;
  int _t82;
  int _t81;
  int _t80;
  int _t79;
  int _t78;
  int _t77;
  int _t76;
  int _t75;
  int _t74;
  int _t73;
  int _t72;
  int _t71;
  int _t70;
  int _t69;
  int _t68;
  int _t67;
  int _t66;
  int _t65;
  int _t64;
  int _t63;
  int _t62;
  int _t61;
  int _t60;
  int _t59;
  int _t58;
  int _t57;
  int _t56;
  int _t55;
  int _t54;
  int _t53;
  int _t52;
  int _t51;
  int _t50;
  int _t49;
  int _t48;
  int _t47;
  int _t46;
  int _t45;
  int _t44;
  int _t43;
  int _t42;
  int _t41;
  int _t40;
  int _t39;
  int _t38;
  int _t37;
  float _P2[R];
  int _t36;
  int _t35;
  int _t34;
  int _t33;
  int _t32;
  float _P1[R];
  int _t31;
  int _t30;
  int _t29;
  int _t28;
  int In_3;
  int In_2;
  int In_1;
  int _t27;
  int _t26;
  int _t25;
  struct a_list *_P_DATA6;
  int newVariable2;
  int newVariable1;
  struct mk *_P_DATA5;
  float *a_prime;
  struct a_list *_P_DATA4;
  unsigned short *_P_DATA3;
  int chill_count_1;
  int *_P_DATA2;
  int chill_count_0;
  int newVariable0;
  float *_P_DATA1;
  int _t24;
  int _t23;
  int _t22;
  int _t21;
  int _t20;
  int _t19;
  int _t18;
  int _t17;
  int _t16;
  int _t15;
  int _t14;
  int _t13;
  int _t12;
  int _t11;
  int _t10;
  int _t9;
  int _t8;
  int _t6;
  int _t7;
  int k;
  int _t4;
  int _t3;
  int _t2;
  int _t1;
  int i;
  int j;

  struct timeval t0, t1;
  double elapsed;

  // Inspector
  gettimeofday(&t0, NULL);


  _P_DATA1 = ((float *) (malloc(sizeof(float) * (ND+1)*R)));           // (ND+1)*R = 7807 * R => N_R = maxNB * R
  for (newVariable0 = 0; newVariable0 <= (N_R-1); newVariable0 += 1)
    _P_DATA1[newVariable0] = x[newVariable0];
  _P_DATA2 = ((int *) (malloc(sizeof(int) * (ND+1))));                // offset_index
  _P_DATA2[0] = 0;
  _P_DATA4 = 0;
  _P_DATA5 = ((struct mk *) (malloc(sizeof(struct mk) * (ND+1))));    // marked
  chill_count_1 = 0;
  _P_DATA2[0] = 0;
  for (t2 = 0; t2 < ND; t2 += 1) {
    for (t4 = 0; t4 < R; t4 += 1)
      for (t6 = index_(R * t2 + t4); t6 <= index__(R * t2 + t4) - 1; t6 += 1) {
        _t21 = (col_(t6) - 0) / R;
        _P_DATA5[_t21].ptr = 0;
      }
    for (t4 = 0; t4 <= (R-1); t4 += 1)
      for (t6 = index_(R * t2 + t4); t6 <= index__(R * t2 + t4) - 1; t6 += 1) {
        _t21 = (col_(t6) - 0) / R;
        _t23 = (col_(t6) - 0) % R;
        if (_P_DATA5[_t21].ptr == 0) {
          _P_DATA6 = ((struct a_list *) (malloc(sizeof(struct a_list) * 1)));
          _P_DATA6->next = _P_DATA4;
          _P_DATA4 = _P_DATA6;
          _P_DATA5[_t21].ptr = _P_DATA4;
          for (newVariable1 = 0; newVariable1 <= (R-1); newVariable1 += 1)
            for (newVariable2 = 0; newVariable2 <= (R-1); newVariable2 += 1)
              _P_DATA5[_t21].ptr->a[R * newVariable1 + 1 * newVariable2] = 0;
          _P_DATA5[_t21].ptr->col_ = _t21;
          chill_count_1 += 1;
        }
        _P_DATA5[_t21].ptr->a[R * t4 + 1 * _t23] = a[t6];
      }
    _P_DATA2[t2 + 1] = chill_count_1;
  }
  for (t2 = -chill_count_1 + 1; t2 <= 0; t2 += 1) {
    if (chill_count_1 + t2 <= 1) {
      _P_DATA3 = ((unsigned short *) (malloc(sizeof(unsigned short) * chill_count_1)));
      a_prime = ((float *) (malloc(sizeof(float) * (chill_count_1 * (R*C)))));
    }
    for (t4 = 0; t4 <= (R-1); t4 += 1)
      for (t6 = 0; t6 <= (R-1); t6 += 1)
        a_prime[-t2 * (R*C) + R * t4 + 1 * t6] = _P_DATA4->a[R * t4 + 1 * t6];
    _P_DATA3[-t2] = _P_DATA4->col_;
    _P_DATA6 = _P_DATA4->next;
    free(_P_DATA4);
    _P_DATA4 = _P_DATA6;
  }

  gettimeofday(&t1, NULL);
  elapsed = t1.tv_sec - t0.tv_sec + (t1.tv_usec - t0.tv_usec) / 1000000.0;
  fprintf(stderr, "Inspector::%.6lf seconds elapsed (NB=%d)\n", elapsed, chill_count_1);

//  FILE *dbg = fopen("./bcsr.csv", "w");
//  fprintf(dbg,"N_R,N_C,NB,ND,R,C,Size\n");
//  fprintf(dbg, "%d,%d,%d,%d,%d,%d,%d\n", N_R, N_C, chill_count_1, ND, R, C, chill_count_1*(R*C));
//  fprintf(dbg, "ii,jj,kk\n");
//  for (t2 = 0; t2 < ND; t2 += 1) {                             // (ND-1) == NB (?), t2 == ii
//    for (t4 = _P_DATA2_(t2); t4 < _P_DATA2__(t2); t4 += 1) {       // _P_DATA2__ == offset_index, t4 == b (or kk)
//      fprintf(dbg, "%d,%d,%d\n", t2, t4, _P_DATA3_(t2,t4));
//    }
//  }
//  fclose(dbg);

  // Executor...
    gettimeofday(&t0, NULL);

  for (t2 = 0; t2 <= (ND-1); t2 += 1)                                     // (ND-1) == NB (?), t2 == ii
    for (t4 = _P_DATA2_(t2); t4 <= _P_DATA2__(t2) - 1; t4 += 1) {       // _P_DATA2__ == offset_index, t4 == b (or kk)
      _P2[R * t2 - R * t2] = y[R * t2];
      _P2[R * t2 + 1 - R * t2] = y[R * t2 + 1];
      _P2[R * t2 + 2 - R * t2] = y[R * t2 + 2];
      _P2[R * t2 + 3 - R * t2] = y[R * t2 + 3];
      _P2[R * t2 + 4 - R * t2] = y[R * t2 + 4];
      _P2[R * t2 + 5 - R * t2] = y[R * t2 + 5];
      _P2[R * t2 + 6 - R * t2] = y[R * t2 + 6];
      _P2[R * t2 + (R-1) - R * t2] = y[R * t2 + (R-1)];
      t8 = R * _P_DATA3_(t2,t4);
      _P1[t8 - R * _P_DATA3_(t2,t4)] = _P_DATA1[t8];
      _P1[t8 + 1 - R * _P_DATA3_(t2,t4)] = _P_DATA1[t8 + 1];
      _P1[t8 + 2 - R * _P_DATA3_(t2,t4)] = _P_DATA1[t8 + 2];
      _P1[t8 + 3 - R * _P_DATA3_(t2,t4)] = _P_DATA1[t8 + 3];
      _P1[t8 + 4 - R * _P_DATA3_(t2,t4)] = _P_DATA1[t8 + 4];
      _P1[t8 + 5 - R * _P_DATA3_(t2,t4)] = _P_DATA1[t8 + 5];
      _P1[t8 + 6 - R * _P_DATA3_(t2,t4)] = _P_DATA1[t8 + 6];
      _P1[t8 + (R-1) - R * _P_DATA3_(t2,t4)] = _P_DATA1[t8 + (R-1)];
      _P1[t8 - R * _P_DATA3_(t2,t4)] = _P_DATA1[t8];
      _P1[t8 + 1 - R * _P_DATA3_(t2,t4)] = _P_DATA1[t8 + 1];
      _P1[t8 + 2 - R * _P_DATA3_(t2,t4)] = _P_DATA1[t8 + 2];
      _P1[t8 + 3 - R * _P_DATA3_(t2,t4)] = _P_DATA1[t8 + 3];
      _P1[t8 + 4 - R * _P_DATA3_(t2,t4)] = _P_DATA1[t8 + 4];
      _P1[t8 + 5 - R * _P_DATA3_(t2,t4)] = _P_DATA1[t8 + 5];
      _P1[t8 + 6 - R * _P_DATA3_(t2,t4)] = _P_DATA1[t8 + 6];
      _P1[t8 + (R-1) - R * _P_DATA3_(t2,t4)] = _P_DATA1[t8 + (R-1)];
      _P1[t8 - R * _P_DATA3_(t2,t4)] = _P_DATA1[t8];
      _P1[t8 + 1 - R * _P_DATA3_(t2,t4)] = _P_DATA1[t8 + 1];
      _P1[t8 + 2 - R * _P_DATA3_(t2,t4)] = _P_DATA1[t8 + 2];
      _P1[t8 + 3 - R * _P_DATA3_(t2,t4)] = _P_DATA1[t8 + 3];
      _P1[t8 + 4 - R * _P_DATA3_(t2,t4)] = _P_DATA1[t8 + 4];
      _P1[t8 + 5 - R * _P_DATA3_(t2,t4)] = _P_DATA1[t8 + 5];
      _P1[t8 + 6 - R * _P_DATA3_(t2,t4)] = _P_DATA1[t8 + 6];
      _P1[t8 + (R-1) - R * _P_DATA3_(t2,t4)] = _P_DATA1[t8 + (R-1)];
      _P1[t8 - R * _P_DATA3_(t2,t4)] = _P_DATA1[t8];
      _P1[t8 + 1 - R * _P_DATA3_(t2,t4)] = _P_DATA1[t8 + 1];
      _P1[t8 + 2 - R * _P_DATA3_(t2,t4)] = _P_DATA1[t8 + 2];
      _P1[t8 + 3 - R * _P_DATA3_(t2,t4)] = _P_DATA1[t8 + 3];
      _P1[t8 + 4 - R * _P_DATA3_(t2,t4)] = _P_DATA1[t8 + 4];
      _P1[t8 + 5 - R * _P_DATA3_(t2,t4)] = _P_DATA1[t8 + 5];
      _P1[t8 + 6 - R * _P_DATA3_(t2,t4)] = _P_DATA1[t8 + 6];
      _P1[t8 + (R-1) - R * _P_DATA3_(t2,t4)] = _P_DATA1[t8 + (R-1)];
      _P1[t8 - R * _P_DATA3_(t2,t4)] = _P_DATA1[t8];
      _P1[t8 + 1 - R * _P_DATA3_(t2,t4)] = _P_DATA1[t8 + 1];
      _P1[t8 + 2 - R * _P_DATA3_(t2,t4)] = _P_DATA1[t8 + 2];
      _P1[t8 + 3 - R * _P_DATA3_(t2,t4)] = _P_DATA1[t8 + 3];
      _P1[t8 + 4 - R * _P_DATA3_(t2,t4)] = _P_DATA1[t8 + 4];
      _P1[t8 + 5 - R * _P_DATA3_(t2,t4)] = _P_DATA1[t8 + 5];
      _P1[t8 + 6 - R * _P_DATA3_(t2,t4)] = _P_DATA1[t8 + 6];
      _P1[t8 + (R-1) - R * _P_DATA3_(t2,t4)] = _P_DATA1[t8 + (R-1)];
      _P1[t8 - R * _P_DATA3_(t2,t4)] = _P_DATA1[t8];
      _P1[t8 + 1 - R * _P_DATA3_(t2,t4)] = _P_DATA1[t8 + 1];
      _P1[t8 + 2 - R * _P_DATA3_(t2,t4)] = _P_DATA1[t8 + 2];
      _P1[t8 + 3 - R * _P_DATA3_(t2,t4)] = _P_DATA1[t8 + 3];
      _P1[t8 + 4 - R * _P_DATA3_(t2,t4)] = _P_DATA1[t8 + 4];
      _P1[t8 + 5 - R * _P_DATA3_(t2,t4)] = _P_DATA1[t8 + 5];
      _P1[t8 + 6 - R * _P_DATA3_(t2,t4)] = _P_DATA1[t8 + 6];
      _P1[t8 + (R-1) - R * _P_DATA3_(t2,t4)] = _P_DATA1[t8 + (R-1)];
      _P1[t8 - R * _P_DATA3_(t2,t4)] = _P_DATA1[t8];
      _P1[t8 + 1 - R * _P_DATA3_(t2,t4)] = _P_DATA1[t8 + 1];
      _P1[t8 + 2 - R * _P_DATA3_(t2,t4)] = _P_DATA1[t8 + 2];
      _P1[t8 + 3 - R * _P_DATA3_(t2,t4)] = _P_DATA1[t8 + 3];
      _P1[t8 + 4 - R * _P_DATA3_(t2,t4)] = _P_DATA1[t8 + 4];
      _P1[t8 + 5 - R * _P_DATA3_(t2,t4)] = _P_DATA1[t8 + 5];
      _P1[t8 + 6 - R * _P_DATA3_(t2,t4)] = _P_DATA1[t8 + 6];
      _P1[t8 + (R-1) - R * _P_DATA3_(t2,t4)] = _P_DATA1[t8 + (R-1)];
      _P1[t8 - R * _P_DATA3_(t2,t4)] = _P_DATA1[t8];
      _P1[t8 + 1 - R * _P_DATA3_(t2,t4)] = _P_DATA1[t8 + 1];
      _P1[t8 + 2 - R * _P_DATA3_(t2,t4)] = _P_DATA1[t8 + 2];
      _P1[t8 + 3 - R * _P_DATA3_(t2,t4)] = _P_DATA1[t8 + 3];
      _P1[t8 + 4 - R * _P_DATA3_(t2,t4)] = _P_DATA1[t8 + 4];
      _P1[t8 + 5 - R * _P_DATA3_(t2,t4)] = _P_DATA1[t8 + 5];
      _P1[t8 + 6 - R * _P_DATA3_(t2,t4)] = _P_DATA1[t8 + 6];
      _P1[t8 + (R-1) - R * _P_DATA3_(t2,t4)] = _P_DATA1[t8 + (R-1)];
      _P2[R * t2 + 0 - R * t2] += a_prime[t4 * (R*C) + R * 0 + 1 * 0] * _P1[R * _P_DATA3[t4] + 0 - R * _P_DATA3_(t2,t4)];
      _P2[R * t2 + 0 - R * t2] += a_prime[t4 * (R*C) + R * 0 + 1 * (0 + 1)] * _P1[R * _P_DATA3[t4] + (0 + 1) - R * _P_DATA3_(t2,t4)];
      _P2[R * t2 + 0 - R * t2] += a_prime[t4 * (R*C) + R * 0 + 1 * (0 + 2)] * _P1[R * _P_DATA3[t4] + (0 + 2) - R * _P_DATA3_(t2,t4)];
      _P2[R * t2 + 0 - R * t2] += a_prime[t4 * (R*C) + R * 0 + 1 * (0 + 3)] * _P1[R * _P_DATA3[t4] + (0 + 3) - R * _P_DATA3_(t2,t4)];
      _P2[R * t2 + 0 - R * t2] += a_prime[t4 * (R*C) + R * 0 + 1 * (0 + 4)] * _P1[R * _P_DATA3[t4] + (0 + 4) - R * _P_DATA3_(t2,t4)];
      _P2[R * t2 + 0 - R * t2] += a_prime[t4 * (R*C) + R * 0 + 1 * (0 + 5)] * _P1[R * _P_DATA3[t4] + (0 + 5) - R * _P_DATA3_(t2,t4)];
      _P2[R * t2 + 0 - R * t2] += a_prime[t4 * (R*C) + R * 0 + 1 * (0 + 6)] * _P1[R * _P_DATA3[t4] + (0 + 6) - R * _P_DATA3_(t2,t4)];
      _P2[R * t2 + 0 - R * t2] += a_prime[t4 * (R*C) + R * 0 + 1 * (0 + (R-1))] * _P1[R * _P_DATA3[t4] + (0 + (R-1)) - R * _P_DATA3_(t2,t4)];
      _P2[R * t2 + (0 + 1) - R * t2] += a_prime[t4 * (R*C) + R * (0 + 1) + 1 * 0] * _P1[R * _P_DATA3[t4] + 0 - R * _P_DATA3_(t2,t4)];
      _P2[R * t2 + (0 + 1) - R * t2] += a_prime[t4 * (R*C) + R * (0 + 1) + 1 * (0 + 1)] * _P1[R * _P_DATA3[t4] + (0 + 1) - R * _P_DATA3_(t2,t4)];
      _P2[R * t2 + (0 + 1) - R * t2] += a_prime[t4 * (R*C) + R * (0 + 1) + 1 * (0 + 2)] * _P1[R * _P_DATA3[t4] + (0 + 2) - R * _P_DATA3_(t2,t4)];
      _P2[R * t2 + (0 + 1) - R * t2] += a_prime[t4 * (R*C) + R * (0 + 1) + 1 * (0 + 3)] * _P1[R * _P_DATA3[t4] + (0 + 3) - R * _P_DATA3_(t2,t4)];
      _P2[R * t2 + (0 + 1) - R * t2] += a_prime[t4 * (R*C) + R * (0 + 1) + 1 * (0 + 4)] * _P1[R * _P_DATA3[t4] + (0 + 4) - R * _P_DATA3_(t2,t4)];
      _P2[R * t2 + (0 + 1) - R * t2] += a_prime[t4 * (R*C) + R * (0 + 1) + 1 * (0 + 5)] * _P1[R * _P_DATA3[t4] + (0 + 5) - R * _P_DATA3_(t2,t4)];
      _P2[R * t2 + (0 + 1) - R * t2] += a_prime[t4 * (R*C) + R * (0 + 1) + 1 * (0 + 6)] * _P1[R * _P_DATA3[t4] + (0 + 6) - R * _P_DATA3_(t2,t4)];
      _P2[R * t2 + (0 + 1) - R * t2] += a_prime[t4 * (R*C) + R * (0 + 1) + 1 * (0 + (R-1))] * _P1[R * _P_DATA3[t4] + (0 + (R-1)) - R * _P_DATA3_(t2,t4)];
      _P2[R * t2 + (0 + 2) - R * t2] += a_prime[t4 * (R*C) + R * (0 + 2) + 1 * 0] * _P1[R * _P_DATA3[t4] + 0 - R * _P_DATA3_(t2,t4)];
      _P2[R * t2 + (0 + 2) - R * t2] += a_prime[t4 * (R*C) + R * (0 + 2) + 1 * (0 + 1)] * _P1[R * _P_DATA3[t4] + (0 + 1) - R * _P_DATA3_(t2,t4)];
      _P2[R * t2 + (0 + 2) - R * t2] += a_prime[t4 * (R*C) + R * (0 + 2) + 1 * (0 + 2)] * _P1[R * _P_DATA3[t4] + (0 + 2) - R * _P_DATA3_(t2,t4)];
      _P2[R * t2 + (0 + 2) - R * t2] += a_prime[t4 * (R*C) + R * (0 + 2) + 1 * (0 + 3)] * _P1[R * _P_DATA3[t4] + (0 + 3) - R * _P_DATA3_(t2,t4)];
      _P2[R * t2 + (0 + 2) - R * t2] += a_prime[t4 * (R*C) + R * (0 + 2) + 1 * (0 + 4)] * _P1[R * _P_DATA3[t4] + (0 + 4) - R * _P_DATA3_(t2,t4)];
      _P2[R * t2 + (0 + 2) - R * t2] += a_prime[t4 * (R*C) + R * (0 + 2) + 1 * (0 + 5)] * _P1[R * _P_DATA3[t4] + (0 + 5) - R * _P_DATA3_(t2,t4)];
      _P2[R * t2 + (0 + 2) - R * t2] += a_prime[t4 * (R*C) + R * (0 + 2) + 1 * (0 + 6)] * _P1[R * _P_DATA3[t4] + (0 + 6) - R * _P_DATA3_(t2,t4)];
      _P2[R * t2 + (0 + 2) - R * t2] += a_prime[t4 * (R*C) + R * (0 + 2) + 1 * (0 + (R-1))] * _P1[R * _P_DATA3[t4] + (0 + (R-1)) - R * _P_DATA3_(t2,t4)];
      _P2[R * t2 + (0 + 3) - R * t2] += a_prime[t4 * (R*C) + R * (0 + 3) + 1 * 0] * _P1[R * _P_DATA3[t4] + 0 - R * _P_DATA3_(t2,t4)];
      _P2[R * t2 + (0 + 3) - R * t2] += a_prime[t4 * (R*C) + R * (0 + 3) + 1 * (0 + 1)] * _P1[R * _P_DATA3[t4] + (0 + 1) - R * _P_DATA3_(t2,t4)];
      _P2[R * t2 + (0 + 3) - R * t2] += a_prime[t4 * (R*C) + R * (0 + 3) + 1 * (0 + 2)] * _P1[R * _P_DATA3[t4] + (0 + 2) - R * _P_DATA3_(t2,t4)];
      _P2[R * t2 + (0 + 3) - R * t2] += a_prime[t4 * (R*C) + R * (0 + 3) + 1 * (0 + 3)] * _P1[R * _P_DATA3[t4] + (0 + 3) - R * _P_DATA3_(t2,t4)];
      _P2[R * t2 + (0 + 3) - R * t2] += a_prime[t4 * (R*C) + R * (0 + 3) + 1 * (0 + 4)] * _P1[R * _P_DATA3[t4] + (0 + 4) - R * _P_DATA3_(t2,t4)];
      _P2[R * t2 + (0 + 3) - R * t2] += a_prime[t4 * (R*C) + R * (0 + 3) + 1 * (0 + 5)] * _P1[R * _P_DATA3[t4] + (0 + 5) - R * _P_DATA3_(t2,t4)];
      _P2[R * t2 + (0 + 3) - R * t2] += a_prime[t4 * (R*C) + R * (0 + 3) + 1 * (0 + 6)] * _P1[R * _P_DATA3[t4] + (0 + 6) - R * _P_DATA3_(t2,t4)];
      _P2[R * t2 + (0 + 3) - R * t2] += a_prime[t4 * (R*C) + R * (0 + 3) + 1 * (0 + (R-1))] * _P1[R * _P_DATA3[t4] + (0 + (R-1)) - R * _P_DATA3_(t2,t4)];
      _P2[R * t2 + (0 + 4) - R * t2] += a_prime[t4 * (R*C) + R * (0 + 4) + 1 * 0] * _P1[R * _P_DATA3[t4] + 0 - R * _P_DATA3_(t2,t4)];
      _P2[R * t2 + (0 + 4) - R * t2] += a_prime[t4 * (R*C) + R * (0 + 4) + 1 * (0 + 1)] * _P1[R * _P_DATA3[t4] + (0 + 1) - R * _P_DATA3_(t2,t4)];
      _P2[R * t2 + (0 + 4) - R * t2] += a_prime[t4 * (R*C) + R * (0 + 4) + 1 * (0 + 2)] * _P1[R * _P_DATA3[t4] + (0 + 2) - R * _P_DATA3_(t2,t4)];
      _P2[R * t2 + (0 + 4) - R * t2] += a_prime[t4 * (R*C) + R * (0 + 4) + 1 * (0 + 3)] * _P1[R * _P_DATA3[t4] + (0 + 3) - R * _P_DATA3_(t2,t4)];
      _P2[R * t2 + (0 + 4) - R * t2] += a_prime[t4 * (R*C) + R * (0 + 4) + 1 * (0 + 4)] * _P1[R * _P_DATA3[t4] + (0 + 4) - R * _P_DATA3_(t2,t4)];
      _P2[R * t2 + (0 + 4) - R * t2] += a_prime[t4 * (R*C) + R * (0 + 4) + 1 * (0 + 5)] * _P1[R * _P_DATA3[t4] + (0 + 5) - R * _P_DATA3_(t2,t4)];
      _P2[R * t2 + (0 + 4) - R * t2] += a_prime[t4 * (R*C) + R * (0 + 4) + 1 * (0 + 6)] * _P1[R * _P_DATA3[t4] + (0 + 6) - R * _P_DATA3_(t2,t4)];
      _P2[R * t2 + (0 + 4) - R * t2] += a_prime[t4 * (R*C) + R * (0 + 4) + 1 * (0 + (R-1))] * _P1[R * _P_DATA3[t4] + (0 + (R-1)) - R * _P_DATA3_(t2,t4)];
      _P2[R * t2 + (0 + 5) - R * t2] += a_prime[t4 * (R*C) + R * (0 + 5) + 1 * 0] * _P1[R * _P_DATA3[t4] + 0 - R * _P_DATA3_(t2,t4)];
      _P2[R * t2 + (0 + 5) - R * t2] += a_prime[t4 * (R*C) + R * (0 + 5) + 1 * (0 + 1)] * _P1[R * _P_DATA3[t4] + (0 + 1) - R * _P_DATA3_(t2,t4)];
      _P2[R * t2 + (0 + 5) - R * t2] += a_prime[t4 * (R*C) + R * (0 + 5) + 1 * (0 + 2)] * _P1[R * _P_DATA3[t4] + (0 + 2) - R * _P_DATA3_(t2,t4)];
      _P2[R * t2 + (0 + 5) - R * t2] += a_prime[t4 * (R*C) + R * (0 + 5) + 1 * (0 + 3)] * _P1[R * _P_DATA3[t4] + (0 + 3) - R * _P_DATA3_(t2,t4)];
      _P2[R * t2 + (0 + 5) - R * t2] += a_prime[t4 * (R*C) + R * (0 + 5) + 1 * (0 + 4)] * _P1[R * _P_DATA3[t4] + (0 + 4) - R * _P_DATA3_(t2,t4)];
      _P2[R * t2 + (0 + 5) - R * t2] += a_prime[t4 * (R*C) + R * (0 + 5) + 1 * (0 + 5)] * _P1[R * _P_DATA3[t4] + (0 + 5) - R * _P_DATA3_(t2,t4)];
      _P2[R * t2 + (0 + 5) - R * t2] += a_prime[t4 * (R*C) + R * (0 + 5) + 1 * (0 + 6)] * _P1[R * _P_DATA3[t4] + (0 + 6) - R * _P_DATA3_(t2,t4)];
      _P2[R * t2 + (0 + 5) - R * t2] += a_prime[t4 * (R*C) + R * (0 + 5) + 1 * (0 + (R-1))] * _P1[R * _P_DATA3[t4] + (0 + (R-1)) - R * _P_DATA3_(t2,t4)];
      _P2[R * t2 + (0 + 6) - R * t2] += a_prime[t4 * (R*C) + R * (0 + 6) + 1 * 0] * _P1[R * _P_DATA3[t4] + 0 - R * _P_DATA3_(t2,t4)];
      _P2[R * t2 + (0 + 6) - R * t2] += a_prime[t4 * (R*C) + R * (0 + 6) + 1 * (0 + 1)] * _P1[R * _P_DATA3[t4] + (0 + 1) - R * _P_DATA3_(t2,t4)];
      _P2[R * t2 + (0 + 6) - R * t2] += a_prime[t4 * (R*C) + R * (0 + 6) + 1 * (0 + 2)] * _P1[R * _P_DATA3[t4] + (0 + 2) - R * _P_DATA3_(t2,t4)];
      _P2[R * t2 + (0 + 6) - R * t2] += a_prime[t4 * (R*C) + R * (0 + 6) + 1 * (0 + 3)] * _P1[R * _P_DATA3[t4] + (0 + 3) - R * _P_DATA3_(t2,t4)];
      _P2[R * t2 + (0 + 6) - R * t2] += a_prime[t4 * (R*C) + R * (0 + 6) + 1 * (0 + 4)] * _P1[R * _P_DATA3[t4] + (0 + 4) - R * _P_DATA3_(t2,t4)];
      _P2[R * t2 + (0 + 6) - R * t2] += a_prime[t4 * (R*C) + R * (0 + 6) + 1 * (0 + 5)] * _P1[R * _P_DATA3[t4] + (0 + 5) - R * _P_DATA3_(t2,t4)];
      _P2[R * t2 + (0 + 6) - R * t2] += a_prime[t4 * (R*C) + R * (0 + 6) + 1 * (0 + 6)] * _P1[R * _P_DATA3[t4] + (0 + 6) - R * _P_DATA3_(t2,t4)];
      _P2[R * t2 + (0 + 6) - R * t2] += a_prime[t4 * (R*C) + R * (0 + 6) + 1 * (0 + (R-1))] * _P1[R * _P_DATA3[t4] + (0 + (R-1)) - R * _P_DATA3_(t2,t4)];
      _P2[R * t2 + (0 + (R-1)) - R * t2] += a_prime[t4 * (R*C) + R * (0 + (R-1)) + 1 * 0] * _P1[R * _P_DATA3[t4] + 0 - R * _P_DATA3_(t2,t4)];
      _P2[R * t2 + (0 + (R-1)) - R * t2] += a_prime[t4 * (R*C) + R * (0 + (R-1)) + 1 * (0 + 1)] * _P1[R * _P_DATA3[t4] + (0 + 1) - R * _P_DATA3_(t2,t4)];
      _P2[R * t2 + (0 + (R-1)) - R * t2] += a_prime[t4 * (R*C) + R * (0 + (R-1)) + 1 * (0 + 2)] * _P1[R * _P_DATA3[t4] + (0 + 2) - R * _P_DATA3_(t2,t4)];
      _P2[R * t2 + (0 + (R-1)) - R * t2] += a_prime[t4 * (R*C) + R * (0 + (R-1)) + 1 * (0 + 3)] * _P1[R * _P_DATA3[t4] + (0 + 3) - R * _P_DATA3_(t2,t4)];
      _P2[R * t2 + (0 + (R-1)) - R * t2] += a_prime[t4 * (R*C) + R * (0 + (R-1)) + 1 * (0 + 4)] * _P1[R * _P_DATA3[t4] + (0 + 4) - R * _P_DATA3_(t2,t4)];
      _P2[R * t2 + (0 + (R-1)) - R * t2] += a_prime[t4 * (R*C) + R * (0 + (R-1)) + 1 * (0 + 5)] * _P1[R * _P_DATA3[t4] + (0 + 5) - R * _P_DATA3_(t2,t4)];
      _P2[R * t2 + (0 + (R-1)) - R * t2] += a_prime[t4 * (R*C) + R * (0 + (R-1)) + 1 * (0 + 6)] * _P1[R * _P_DATA3[t4] + (0 + 6) - R * _P_DATA3_(t2,t4)];
      _P2[R * t2 + (0 + (R-1)) - R * t2] += a_prime[t4 * (R*C) + R * (0 + (R-1)) + 1 * (0 + (R-1))] * _P1[R * _P_DATA3[t4] + (0 + (R-1)) - R * _P_DATA3_(t2,t4)];
      y[R * t2] = _P2[R * t2 - R * t2];
      y[R * t2 + 1] = _P2[R * t2 + 1 - R * t2];
      y[R * t2 + 2] = _P2[R * t2 + 2 - R * t2];
      y[R * t2 + 3] = _P2[R * t2 + 3 - R * t2];
      y[R * t2 + 4] = _P2[R * t2 + 4 - R * t2];
      y[R * t2 + 5] = _P2[R * t2 + 5 - R * t2];
      y[R * t2 + 6] = _P2[R * t2 + 6 - R * t2];
      y[R * t2 + (R-1)] = _P2[R * t2 + (R-1) - R * t2];
    }
  for (t2 = (ND*R); t2 <= (N_R-1); t2 += 1) 
    for (t4 = index_(t2); t4 <= index__(t2) - 1; t4 += 1) 
      y[t2] += a[t4] * x[col[t4]];

  free(_P_DATA1);
  free(_P_DATA2);
  free(_P_DATA3);
  free(a_prime);
  free(_P_DATA5);

    gettimeofday(&t1, NULL);
    elapsed =t1.tv_sec-t0.tv_sec + (t1.tv_usec-t0.tv_usec)/1000000.0;
    fprintf(stderr,"Executor::%.6lf seconds elapsed\n", elapsed);
}

char spmv_comp(int n, float *b, float *b2) {
    int i;
    for (i = 0; i < n; i++) {
        if (fabs((b2[i] - b[i])) >= 0.01) {
            fprintf(stderr, "Values don't match at %d, expected %f obtained %f\n",i,b[i],b2[i]);
            return false;
        }
    }

    return true;
}

int _main(int argc,char **argv)
{
/////////////////////////////////////////////////////////////////////
  struct timeval tv1;
  struct timeval tv2;
  struct timeval tv3;
  struct timeval tv4;
  struct timeval tv5;
  char filename[255];
  struct sparse_matrix A;
  struct sparse_matrix M;
  float *x;
  float *b;
  float *b2;
  int num_threads;
//pthread_attr_t attr;
  fprintf(stderr,"argc=%d,argv[1]='%s'\n", argc, argv[1]);
  strcpy(filename,argv[1]);
  if (strlen(filename) > 0) {
    fprintf(stderr,"main::Loading matrix %s\n",filename);
//fprintf(stderr, "R: %d C: %d\n", R, C);	
  }
  load_sparse_matrix(filename,&A);
//initialize b to 0 and x to 1
  int i;
  int ii;
  x = ((float *)(malloc(sizeof(float ) * A . ncols)));
  b = ((float *)(malloc(sizeof(float ) * A . nrows)));
  b2 = ((float *)(malloc(sizeof(float ) * A . nrows)));

  for (i = 0; i < A . ncols; i++) 
    x[i] = 1.0;
  for (i = 0; i < A . nrows; i++) {
    b[i] = 0;
    b2[i] = 0;
  }

  struct timeval t1,t2;
  double elapsed;

  gettimeofday(&t1, NULL);
  smv(&A,x,b);
  gettimeofday(&t2, NULL);
  elapsed =t2.tv_sec-t1.tv_sec + (t2.tv_usec -t1.tv_usec)/1000000.0;
  fprintf(stderr,"smv::%.6lf seconds elapsed\n", elapsed);

  gettimeofday(&t1, NULL);
  spmv(A.nrows,A.rows,A.vals,b2,x,A.cols);
  gettimeofday(&t2, NULL);
  elapsed =t2.tv_sec-t1.tv_sec + (t2.tv_usec -t1.tv_usec)/1000000.0;
  fprintf(stderr,"spmv::%.6lf seconds elapsed\n", elapsed);

  spmv_comp(A.nrows, b, b2);

  free(x);
  free(b);
  free(b2);
  free(A . rows);
  free(A . cols);
  free(A . vals);
  if (i != A . nrows) 
//printf("i  is %d\n", i) ;
    exit(1);

  return 0;
}
