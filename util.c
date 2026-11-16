#include "util.h"

#include <stdio.h>
#include <stdlib.h>
#include <math.h>


void hadamard(double *a,double *b,double *c,int n)
// c(n)=a(n) * b(n)
{
    for(int i=1;i<=n;i++) c[i]=a[i]*b[i]; 
}


void matmul(double **A,double **B,double **C,int n,int m,int p)
// C(n,p)=A(n,m) @ B(m,p)
{
      mat_zero(C,1,n,1,p);

    // c_ij = \sum_{k=1}^{m} a_ik * b_kj  
    for(int i=1;i<=n;i++){
            for(int j=1;j<=p;j++){
                for(int k=1;k<=m;k++){
                    C[i][j] += A[i][k]*B[k][j]; 
                }
            }
        }
}

void matvecmul(double *x,double **A,double *y,int n,int m)
// y(n,1) = A(n,m) @ x(m,1)
{
    vec_zero(y,1,n);

    // y_i = \sum_{j=1}^{n} a_ij * x_j  
    for(int i=1;i<=n;i++)
        for(int j=1;j<=m;j++)
             y[i]+=A[i][j]*x[j];

}

void vecmatmul(double *x,double **A,double *y,int n,int m)
// y(1,m) = x(1,n) @ A(n,m)
{
    vec_zero(y,1,m);
    // y_i = \sum_{j=1}^{n} x_j * a_ji
    for(int i=1;i<=m;i++)
        for(int j=1;j<=n;j++)
            y[i]+=x[j]*A[j][i];

}

void vecvecmul(double *x,double *y,double **A,int n,int m)
// A = x(n,1) @ y(1,m)
{
    for(int i=1;i<=n;i++)
        for(int j=1;j<=m;j++)
            A[i][j]=x[i]*y[j]; 
}

void vecvecmul_add(double *x,double *y,double **A,int n,int m)
// A += x(n,1) @ y(1,m)
{
    for(int i=1;i<=n;i++)
        for(int j=1;j<=m;j++)
            A[i][j]+=x[i]*y[j]; 
}

void vec_add(double *x,double *y,double *z,int nl,int nh)
// z = x + y
{
    for(int i=nl;i<=nh;i++) z[i]=x[i]+y[i];
}

void vec_sub(double *x,double *y,double *z,int nl,int nh)
// z = x - y
{
    for(int i=nl;i<=nh;i++) z[i]=x[i]-y[i];
}

void vec_tanh(double *x,double *y,int nl,int nh)
// 配列の各要素にtanhを適用する
{
    for(int i=nl;i<=nh;i++) y[i]=tanh(x[i]);
}

void mat_rand(double **m,int nrl,int nrh,int ncl,int nch)
// [0,1)から抽出したランダムな数を行列の各要素に入れる
{
    int n = (nrh-nrl+1)*(nch-ncl+1);
    for(int i=0;i<n;i++) m[nrl][ncl+i]=urand();
}

void vec_rand(double *x,int nl,int nh)
// [0,1)から抽出したランダムな数を配列の各要素に入れる
{
    for(int i=nl;i<=nh;i++) x[i]=urand();
}

void vec_zero(double *a,int nl,int nh)
// 配列(ベクトル)を0で満たす
{
    for(int i=nl;i<=nh;i++) a[i]=0;
}

void mat_zero(double **m,int nrl,int nrh,int ncl,int nch)
// 行列を0で満たす
{
    int n = (nrh-nrl+1)*(nch-ncl+1);
    for(int i=0;i<n;i++) m[nrl][ncl+i]=0;
}

double urand(void)
// 一様分布 [0,1)
{
    return (double)(rand() / (RAND_MAX + 1.0));
}

void vec_fprint(FILE *fp,double *x,int nl,int nh)
// ベクトルを1行に空白区切りで書き込む
// %.17g はdoubleを読み戻したときに元の値と完全に一致する桁数
{
    for(int i=nl;i<=nh;i++) fprintf(fp,"%.17g%c",x[i],i<nh ? ' ' : '\n');
}

void mat_fprint(FILE *fp,double **m,int nrl,int nrh,int ncl,int nch)
// 行列を1行ずつ書き込む
{
    for(int i=nrl;i<=nrh;i++) vec_fprint(fp,m[i],ncl,nch);
}

int vec_fscan(FILE *fp,double *x,int nl,int nh)
// ベクトルを読み込む。成功したら0、失敗したら-1を返す
{
    for(int i=nl;i<=nh;i++)
        if(fscanf(fp,"%lf",&x[i])!=1) return -1;
    return 0;
}

int mat_fscan(FILE *fp,double **m,int nrl,int nrh,int ncl,int nch)
// 行列を読み込む。成功したら0、失敗したら-1を返す
{
    for(int i=nrl;i<=nrh;i++)
        if(vec_fscan(fp,m[i],ncl,nch)!=0) return -1;
    return 0;
}

