#include "train.h"
#include <stdio.h>
#include "nn.h"
#include "dataset.h"
#include "nrutil.h"

double mse_loss(double *y,double *t,double *dy,int m)
// L=||y-t||^2を返し、dy = ∂L/∂y = 2(y-t)を書き込む
{
    double L=0.0;
    for(int i=1;i<=m;i++){
        double e=y[i]-t[i];
        L+=e*e;
        dy[i]=2.0*e;
    }
    return L;
}

void train(NN *net,const Dataset *ds,int epochs,double lr)
// フルバッチの勾配降下法
{
    int N=ds->N, dout=ds->dout;
    double *y=dvector(1,dout);
    double *dy=dvector(1,dout);

    for(int ep=1;ep<=epochs;ep++){
        double L=0.0;
        nn_zero_grad(net);
        for(int i=1;i<=N;i++){
            nn_forward(net,ds->X[i],y);
            L += mse_loss(y,ds->T[i],dy,dout);
            nn_backward(net,dy);
        }
        nn_step(net,lr/N);

        if(ep%100 == 0) printf("epoch %d loss %e\n",ep,L/N);
    }
    free_dvector(y,1,dout);
    free_dvector(dy,1,dout);
}
