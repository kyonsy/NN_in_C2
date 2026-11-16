#include "dataset.h"
#include <stdio.h>


int dataset_load(Dataset *ds,const char *path)
// 1行に x_1,...,x_D_IN,t_1,...,t_D_OUT が並んだファイルから N_T 行読み込む
// 成功したら0、失敗したら-1を返す
{
    FILE *fp=fopen(path,"r");
    if(fp==NULL) return -1;

    for(int s=0;s<N_T;s++){
        for(int j=0;j<D_IN;j++)
            if(fscanf(fp,"%lf,",&ds->X[s][j])!=1) goto fail;
        for(int j=0;j<D_OUT;j++)
            if(fscanf(fp,"%lf,",&ds->T[s][j])!=1) goto fail;
    }
    fclose(fp);
    return 0;

fail:
    fprintf(stderr,"dataset_load: データが N_T=%d 行より少ないか、形式が違います\n",N_T);
    fclose(fp);
    return -1;
}
