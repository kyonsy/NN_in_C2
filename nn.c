#include "nn.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "nrutil.h"
#include "util.h"


void nn_init(NN *net,int n,int *d){
    net->n=n;
    net->d=d;

    net->dmax=0;
    for(int i=0;i<=n;i++) if(d[i]>net->dmax) net->dmax=d[i];
    net->work=dvector(1,net->dmax);

    net->h=(double**)malloc((size_t)((n+1)*sizeof(double*)));
    // 添え字が1スタートになるように n+1 個確保する(0番目は未使用)
    net->layer=(Layer*)malloc((size_t)((n+1)*sizeof(Layer)));

    double **h=net->h;
    Layer *L=net->layer;

    h[0]=dvector(1,d[0]);
    for(int i=1;i<=n;i++){
        h[i]=dvector(1,d[i]);
        L[i].W=dmatrix(1,d[i],1,d[i-1]);
        L[i].b=dvector(1,d[i]);
        L[i].dW=dmatrix(1,d[i],1,d[i-1]);
        L[i].db=dvector(1,d[i]);

        mat_rand(L[i].W,1,d[i],1,d[i-1]);
        vec_rand(L[i].b,1,d[i]);
        mat_zero(L[i].dW,1,d[i],1,d[i-1]);
        vec_zero(L[i].db,1,d[i]);
    }
}

void nn_free(NN *net){
    int n=net->n, *d=net->d;
    double **h=net->h;
    Layer *L=net->layer;

    free_dvector(net->work,1,net->dmax);
    free_dvector(h[0],1,d[0]);
    for(int i=1;i<=n;i++){
        free_dvector(h[i],1,d[i]);
        free_dmatrix(L[i].W,1,d[i],1,d[i-1]);
        free_dvector(L[i].b,1,d[i]);
        free_dmatrix(L[i].dW,1,d[i],1,d[i-1]);
        free_dvector(L[i].db,1,d[i]);
    }
    free(h);
    free(L);
}

void nn_forward(NN *net,double *x,double *y){
    int n=net->n, *d=net->d;
    double **h=net->h;
    Layer *L=net->layer;

    memcpy(h[0]+1,x+1,d[0]*sizeof(double));

    for(int i=1;i<=n-1;i++){
        matvecmul(h[i-1],L[i].W,h[i],d[i],d[i-1]);
        vec_add(h[i],L[i].b,h[i],1,d[i]);
        vec_tanh(h[i],h[i],1,d[i]);
    }
    matvecmul(h[n-1],L[n].W,h[n],d[n],d[n-1]);
    vec_add(h[n],L[n].b,h[n],1,d[n]);

    memcpy(y+1,h[n]+1,d[n]*sizeof(double));
}

void nn_backward(NN *net,double *dy){
    int n=net->n, *d=net->d;
    double **h=net->h, *work=net->work;
    Layer *L=net->layer;

    memcpy(h[n]+1,dy+1,d[n]*sizeof(double));
    vecvecmul_add(h[n],h[n-1],L[n].dW,d[n],d[n-1]);
    vec_add(L[n].db,h[n],L[n].db,1,d[n]);

    for(int k=n-1;k>=1;k--){
        hadamard(h[k],h[k],h[k],d[k]);
        for(int j=1;j<=d[k];j++) h[k][j]=1.0-h[k][j];
        vecmatmul(h[k+1],L[k+1].W,work,d[k+1],d[k]);
        hadamard(work,h[k],h[k],d[k]);
        vecvecmul_add(h[k],h[k-1],L[k].dW,d[k],d[k-1]);
        vec_add(L[k].db,h[k],L[k].db,1,d[k]);
    }
}

void nn_step(NN *net,double lr){
    int n=net->n, *d=net->d;
    Layer *L=net->layer;

    for(int k=1;k<=n;k++){
        for(int i=1;i<=d[k];i++){
            for(int j=1;j<=d[k-1];j++)
                L[k].W[i][j] -= lr*L[k].dW[i][j];
            L[k].b[i] -= lr*L[k].db[i];
        }
    }
}

void nn_zero_grad(NN *net){
    int n=net->n, *d=net->d;
    Layer *L=net->layer;

    for(int k=1;k<=n;k++){
        mat_zero(L[k].dW,1,d[k],1,d[k-1]);
        vec_zero(L[k].db,1,d[k]);
    }
}

int nn_export_weights(const NN *net,const char *path)
// 重みとバイアスをテキストファイルに書き出す。成功したら0、失敗したら-1を返す
// ファイルの形式:
//   1行目     : n
//   2行目     : d[0] d[1] ... d[n]
//   その後    : 第1層から順に W(d[k]行) と b(1行)
{
    int n=net->n, *d=net->d;
    Layer *L=net->layer;

    FILE *fp=fopen(path,"w");
    if(fp==NULL) return -1;

    fprintf(fp,"%d\n",n);
    for(int k=0;k<=n;k++) fprintf(fp,"%d%c",d[k],k<n ? ' ' : '\n');
    for(int k=1;k<=n;k++){
        mat_fprint(fp,L[k].W,1,d[k],1,d[k-1]);
        vec_fprint(fp,L[k].b,1,d[k]);
    }

    fclose(fp);
    return 0;
}

int nn_import_weights(NN *net,const char *path)
// nn_export_weightsで書き出したファイルから重みとバイアスを読み込む
// netはnn_init済みで、ファイルと同じ層の構造である必要がある
// 成功したら0、失敗したら-1を返す
{
    int n=net->n, *d=net->d;
    Layer *L=net->layer;

    FILE *fp=fopen(path,"r");
    if(fp==NULL) return -1;

    // 層の構造がnetと一致しているか確認する
    int fn, fd;
    if(fscanf(fp,"%d",&fn)!=1 || fn!=n){
        fprintf(stderr,"nn_import_weights: 層の数が一致しません\n");
        fclose(fp);
        return -1;
    }
    for(int k=0;k<=n;k++){
        if(fscanf(fp,"%d",&fd)!=1 || fd!=d[k]){
            fprintf(stderr,"nn_import_weights: d[%d]が一致しません\n",k);
            fclose(fp);
            return -1;
        }
    }

    for(int k=1;k<=n;k++){
        if(mat_fscan(fp,L[k].W,1,d[k],1,d[k-1])!=0 || vec_fscan(fp,L[k].b,1,d[k])!=0){
            fprintf(stderr,"nn_import_weights: 第%d層の読み込みに失敗しました\n",k);
            fclose(fp);
            return -1;
        }
    }

    fclose(fp);
    return 0;
}
