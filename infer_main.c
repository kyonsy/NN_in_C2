// 推論用の実行ファイル
// 使い方: infer.exe 重みファイル 入力.csv
//   入力.csv : 1行に x_1,...,x_D_IN が並んだファイル(教師データの列は入れない)
//   ファイルの終わりまで推論し、出力 y_1,...,y_D_OUT を1行ずつ表示する
//   入力は最大 N_T 行ずつまとめて行列としてネットワークに流す
#include <stdio.h>
#include "config.h"
#include "nn.h"

// 大きな配列はスタックからあふれないように static で置く
static NN     net;
static double X[N_T][D_IN];
static double Y[N_T][D_OUT];


int read_rows(FILE *fp,double X[][D_IN],int max_rows)
// 入力を最大 max_rows 行読む。読めた行数を返す(ファイルの終わりなら0)
// 行の途中でデータが切れていたら -1 を返す
{
    int n;
    for(n=0;n<max_rows;n++){
        int r=fscanf(fp,"%lf,",&X[n][0]);
        if(r==EOF) break;            // ファイルの終わり
        if(r!=1) return -1;
        for(int j=1;j<D_IN;j++)
            if(fscanf(fp,"%lf,",&X[n][j])!=1) return -1;
    }
    return n;
}

int main(int argc,char **argv)
{
    if(argc!=3){
        fprintf(stderr,"使い方: %s 重みファイル 入力.csv\n",argv[0]);
        return 1;
    }
    const char *weights_path=argv[1];
    const char *input_path=argv[2];

    if(nn_load(&net,weights_path)!=0){
        fprintf(stderr,"重みの読み込みに失敗しました: %s\n",weights_path);
        return 1;
    }

    FILE *fp=fopen(input_path,"r");
    if(fp==NULL){
        fprintf(stderr,"入力ファイルを開けません: %s\n",input_path);
        return 1;
    }

    int ret=0;
    for(;;){
        int n=read_rows(fp,X,N_T);
        if(n==0) break;
        if(n<0){
            fprintf(stderr,"入力の形式が正しくありません\n");
            ret=1;
            break;
        }

        nn_forward(&net,n,X,Y);
        for(int s=0;s<n;s++)
            for(int j=0;j<D_OUT;j++)
                printf("%.17g%c",Y[s][j],j<D_OUT-1 ? ',' : '\n');
    }

    fclose(fp);
    return ret;
}
