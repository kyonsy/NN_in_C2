#ifndef MATRIX_H
#define MATRIX_H

#include "config.h"

// 行列の掛け算
//
// このプログラムの行列は、すべて「列数が DMAX の2次元配列」に入れる。
//   例: double W[DMAX][DMAX];     (重み)
//       double H[N_T][DMAX];      (n個のサンプルをまとめた行列。1行 = 1サンプル)
// 実際に使う行数・列数は引数で渡し、余った部分は使わない。
// 添え字は0から始まる。

// C(n,p) = A(n,m) @ B(m,p)
void mat_mul_AB(int n,int m,int p,double A[][DMAX],double B[][DMAX],double C[][DMAX]);

// C(n,p) = A(n,m) @ B(p,m)^T    (Bを転置して掛ける)
void mat_mul_ABt(int n,int m,int p,double A[][DMAX],double B[][DMAX],double C[][DMAX]);

// C(n,p) = A(m,n)^T @ B(m,p)    (Aを転置して掛ける)
void mat_mul_AtB(int n,int m,int p,double A[][DMAX],double B[][DMAX],double C[][DMAX]);

#endif
