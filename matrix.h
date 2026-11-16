#ifndef MATRIX_H
#define MATRIX_H

// 行列の掛け算
//
// 引数の double A[n][m] は「n行m列の行列」という意味。
// 行列の大きさ n, m, p を先に渡すことで、どんな大きさの固定長配列でも受け取れる。
// (配列をコピーするのではなく、先頭の場所を渡しているだけ。動的メモリは使わない)
//
// 配列の行数が n より多くてもよい(先頭の n 行だけ使う)。
//   例: double H[N_T][N_WIDTH] を n 行の行列として渡す
// 添え字は0から始まる。

// C(n,p) = A(n,m) @ B(m,p)
void mat_mul_AB(int n,int m,int p,double A[n][m],double B[m][p],double C[n][p]);

// C(n,p) = A(n,m) @ B(p,m)^T    (Bを転置して掛ける)
void mat_mul_ABt(int n,int m,int p,double A[n][m],double B[p][m],double C[n][p]);

// C(n,p) = A(m,n)^T @ B(m,p)    (Aを転置して掛ける)
void mat_mul_AtB(int n,int m,int p,double A[m][n],double B[m][p],double C[n][p]);

#endif
