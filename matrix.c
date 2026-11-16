#include "matrix.h"


void mat_mul_AB(int n,int m,int p,double A[][DMAX],double B[][DMAX],double C[][DMAX])
// C(n,p) = A(n,m) @ B(m,p)
// c_ij = Σ_k a_ik * b_kj
{
    for(int i=0;i<n;i++){
        for(int j=0;j<p;j++) C[i][j]=0.0;
        // i -> k -> j の順にすると、B と C を行方向に順番に読めるので速い
        for(int k=0;k<m;k++)
            for(int j=0;j<p;j++)
                C[i][j]+=A[i][k]*B[k][j];
    }
}

void mat_mul_ABt(int n,int m,int p,double A[][DMAX],double B[][DMAX],double C[][DMAX])
// C(n,p) = A(n,m) @ B(p,m)^T
// c_ij = Σ_k a_ik * b_jk
{
    for(int i=0;i<n;i++)
        for(int j=0;j<p;j++){
            double s=0.0;
            for(int k=0;k<m;k++) s+=A[i][k]*B[j][k];
            C[i][j]=s;
        }
}

void mat_mul_AtB(int n,int m,int p,double A[][DMAX],double B[][DMAX],double C[][DMAX])
// C(n,p) = A(m,n)^T @ B(m,p)
// c_ij = Σ_k a_ki * b_kj
{
    for(int i=0;i<n;i++)
        for(int j=0;j<p;j++) C[i][j]=0.0;

    for(int k=0;k<m;k++)
        for(int i=0;i<n;i++)
            for(int j=0;j<p;j++)
                C[i][j]+=A[k][i]*B[k][j];
}
