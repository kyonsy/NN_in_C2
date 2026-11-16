#include "dataset.h"
#include <stdio.h>
#include "nrutil.h"


void dataset_init(Dataset *ds,int N,int din,int dout)
// N個のサンプルを入れる領域を確保する
{
    ds->N=N;
    ds->din=din;
    ds->dout=dout;
    ds->X=dmatrix(1,N,1,din);
    ds->T=dmatrix(1,N,1,dout);
}

void dataset_free(Dataset *ds)
{
    free_dmatrix(ds->X,1,ds->N,1,ds->din);
    free_dmatrix(ds->T,1,ds->N,1,ds->dout);
}

int dataset_load(Dataset *ds,const char *path,int N,int din,int dout)
// 1行に x_1,...,x_din,t_1,...,t_dout が並んだファイルを読み込む
// 成功したら0を返す(dsは確保済み)。失敗したら-1を返す(dsは確保されない)
{
    FILE *fp=fopen(path,"r");
    if(fp==NULL) return -1;

    dataset_init(ds,N,din,dout);
    for(int i=1;i<=N;i++){
        for(int j=1;j<=din;j++)
            if(fscanf(fp,"%lf,",&ds->X[i][j])!=1) goto fail;
        for(int j=1;j<=dout;j++)
            if(fscanf(fp,"%lf,",&ds->T[i][j])!=1) goto fail;
    }
    fclose(fp);
    return 0;

fail:
    dataset_free(ds);
    fclose(fp);
    return -1;
}
