// 学習用の実行ファイル
// 使い方: train.exe 学習データ.csv サンプル数 重みの保存先
//   学習データ.csv : 1行に x_1,...,x_din,t_1,...,t_dout が並んだファイル
#include <stdio.h>
#include <stdlib.h>
#include "config.h"
#include "nn.h"
#include "dataset.h"
#include "train.h"


int main(int argc,char **argv)
{
    if(argc!=4){
        fprintf(stderr,"使い方: %s 学習データ.csv サンプル数 重みの保存先\n",argv[0]);
        return 1;
    }
    const char *data_path=argv[1];
    int N=atoi(argv[2]);
    const char *weights_path=argv[3];

    if(N<=0){
        fprintf(stderr,"サンプル数は1以上を指定してください: %s\n",argv[2]);
        return 1;
    }

    Dataset ds;
    if(dataset_load(&ds,data_path,N,D_IN,D_OUT)!=0){
        fprintf(stderr,"学習データの読み込みに失敗しました: %s\n",data_path);
        return 1;
    }

    int D[N_LAYERS+1];
    SET_D(D);

    srand(SEED);
    NN net;
    nn_init(&net,N_LAYERS,D);

    train(&net,&ds,EPOCHS,LR);

    int ret=0;
    if(nn_export_weights(&net,weights_path)!=0){
        fprintf(stderr,"重みの保存に失敗しました: %s\n",weights_path);
        ret=1;
    }else{
        printf("重みを保存しました: %s\n",weights_path);
    }

    nn_free(&net);
    dataset_free(&ds);
    return ret;
}
