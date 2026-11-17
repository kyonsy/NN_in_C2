#include "dataset.h"
#include <stdio.h>


void dataset_load(Dataset *ds,const char *path)
// 1行に x_1,...,x_D_IN,t_1,...,t_D_OUT が並んだファイルから N_T 行読み込む
{
    FILE *fp=fopen(path,"r");

    for(int s=0;s<N_T;s++){
        for(int j=0;j<D_IN;j++)  fscanf(fp,"%lf,",&ds->X[s][j]);
        for(int j=0;j<D_OUT;j++) fscanf(fp,"%lf,",&ds->T[s][j]);
    }

    fclose(fp);
}
