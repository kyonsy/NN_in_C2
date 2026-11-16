#ifndef NRUTIL_H
#define NRUTIL_H

void nrerror(const char error_text[]);
double *dvector(int nl,int nh);
double **dmatrix(int nrl,int nrh,int ncl,int nch);
void free_dvector(double *v,int nl,int nh);
void free_dmatrix(double **m,int nrl,int nrh,int ncl,int nch);

#endif
