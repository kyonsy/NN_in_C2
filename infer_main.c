// 推論用の実行ファイル
// 使い方: infer.exe 重みファイル 入力.csv
//   入力.csv : 1行に x_1,...,x_din が並んだファイル(教師データの列は入れない)
//   ファイルの終わりまで1行ずつ推論し、出力 y_1,...,y_dout を1行ずつ表示する
#include <stdio.h>
#include <stdlib.h>
#include "config.h"
#include "nn.h"
#include "nrutil.h"


int main(int argc,char **argv)
{
    if(argc!=3){
        fprintf(stderr,"使い方: %s 重みファイル 入力.csv\n",argv[0]);
        return 1;
    }
    const char *weights_path=argv[1];
    const char *input_path=argv[2];
    int din=D[0], dout=D[N_LAYERS];

    NN net;
    nn_init(&net,N_LAYERS,D);
    if(nn_import_weights(&net,weights_path)!=0){
        fprintf(stderr,"重みの読み込みに失敗しました: %s\n",weights_path);
        nn_free(&net);
        return 1;
    }

    FILE *fp=fopen(input_path,"r");
    if(fp==NULL){
        fprintf(stderr,"入力ファイルを開けません: %s\n",input_path);
        nn_free(&net);
        return 1;
    }

    double *x=dvector(1,din);
    double *y=dvector(1,dout);
    int ret=0;

    for(int count=1;;count++){
        // 1サンプル分(din個)読む。最初の値でファイルが終わっていたら正常終了
        int r=fscanf(fp,"%lf,",&x[1]);
        if(r==EOF) break;
        int ok=(r==1);
        for(int j=2;j<=din && ok;j++)
            ok=(fscanf(fp,"%lf,",&x[j])==1);
        if(!ok){
            fprintf(stderr,"入力の%d個目のサンプルを読めませんでした\n",count);
            ret=1;
            break;
        }

        nn_forward(&net,x,y);
        for(int j=1;j<=dout;j++) printf("%.17g%c",y[j],j<dout ? ',' : '\n');
    }

    fclose(fp);
    free_dvector(x,1,din);
    free_dvector(y,1,dout);
    nn_free(&net);
    return ret;
}
