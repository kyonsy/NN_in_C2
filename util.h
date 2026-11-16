#ifndef UTIL_H
#define UTIL_H

#include <stdio.h>

void hadamard(double *a,double *b,double *c,int n);
void matmul(double **A,double **B,double **C,int n,int m,int p);
void matvecmul(double *x,double **A,double *y,int n,int m);
void vecmatmul(double *x,double **A,double *y,int n,int m);
void vecvecmul(double *x,double *y,double **A,int n,int m);
void vecvecmul_add(double *x,double *y,double **A,int n,int m);
void vec_tanh(double *x,double *y,int nl,int nh);
void vec_add(double *x,double *y,double *z,int nl,int nh);
void vec_sub(double *x,double *y,double *z,int nl,int nh);
void mat_rand(double **m,int nrl,int nrh,int ncl,int nch);
void vec_rand(double *x,int nl,int nh);
void vec_zero(double *a,int nl,int nh);
void mat_zero(double **m,int nrl,int nrh,int ncl,int nch);
double urand(void);
void vec_fprint(FILE *fp,double *x,int nl,int nh);
void mat_fprint(FILE *fp,double **m,int nrl,int nrh,int ncl,int nch);
int vec_fscan(FILE *fp,double *x,int nl,int nh);
int mat_fscan(FILE *fp,double **m,int nrl,int nrh,int ncl,int nch);

#endif
