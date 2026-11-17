// 推論用の実行ファイル
// 使い方: infer_main.exe 重み.csv 教師データ.csv > 出力.csv
//   教師データ.csv : 学習と同じ形式(1行に x_1,...,x_D_IN,t_1,...,t_D_OUT、N_T行)
//                    推論には x の列だけを使い、t の列は読むだけで使わない
//   N_T 個の入力をまとめて推論し、出力 y_1,...,y_D_OUT を1行ずつ標準出力に書き出す
#include <stdio.h>
#include "config.h"
#include "nn.h"
#include "dataset.h"

// 大きな配列はスタックからあふれないように static で置く
static NN      net;
static Dataset ds;
static double  Y[N_T][D_OUT];


int main(int argc,char **argv)
{
    const char *weights_path=argv[1];
    const char *data_path=argv[2];

    nn_load(&net,weights_path);
    dataset_load(&ds,data_path);

    nn_forward(&net,N_T,ds.X,Y);

    for(int s=0;s<N_T;s++)
        for(int j=0;j<D_OUT;j++)
            printf("%.17g%c",Y[s][j],j<D_OUT-1 ? ',' : '\n');

    return 0;
}
