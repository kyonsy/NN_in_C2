// 学習用の実行ファイル
// 使い方: train.exe 学習データ.csv 重みの保存先
//   学習データ.csv : 1行に x_1,...,x_D_IN,t_1,...,t_D_OUT が並んだファイル(N_T行)
#include <stdio.h>
#include <stdlib.h>
#include "config.h"
#include "nn.h"
#include "dataset.h"

// 大きな配列はスタックからあふれないように static で置く
static NN      net;
static Dataset ds;
static double  Y[N_T][D_OUT];   // ネットワークの出力
static double  dY[N_T][D_OUT];  // 損失の出力に対する勾配


double mse_loss(int n,double Y[][D_OUT],double T[][D_OUT],double dY[][D_OUT])
// 平均二乗誤差 L = (1/n) Σ_s ||Y[s]-T[s]||^2 を返し、
// dY = ∂L/∂Y = 2(Y-T)/n を書き込む
{
    double L=0.0;
    for(int s=0;s<n;s++)
        for(int j=0;j<D_OUT;j++){
            double e=Y[s][j]-T[s][j];
            L+=e*e;
            dY[s][j]=2.0*e/n;
        }
    return L/n;
}

void train(NN *net,Dataset *ds,int epochs,double lr)
// フルバッチの勾配降下法(N_T個のサンプルを毎回まとめて流す)
{
    for(int ep=1;ep<=epochs;ep++){
        nn_forward(net,N_T,ds->X,Y);              // 順伝播
        double L=mse_loss(N_T,Y,ds->T,dY);       // 損失
        nn_backward(net,N_T,dY);                  // 逆伝播
        nn_step(net,lr);                          // 更新

        if(ep%100==0) printf("epoch %d loss %e\n",ep,L);
    }
}

int main(int argc,char **argv)
{
    if(argc!=3){
        fprintf(stderr,"使い方: %s 学習データ.csv 重みの保存先\n",argv[0]);
        return 1;
    }
    const char *data_path=argv[1];
    const char *weights_path=argv[2];

    if(dataset_load(&ds,data_path)!=0){
        fprintf(stderr,"学習データの読み込みに失敗しました: %s\n",data_path);
        return 1;
    }

    srand(SEED);
    nn_init(&net);

    train(&net,&ds,EPOCHS,LR);

    if(nn_save(&net,weights_path)!=0){
        fprintf(stderr,"重みの保存に失敗しました: %s\n",weights_path);
        return 1;
    }
    printf("重みを保存しました: %s\n",weights_path);
    return 0;
}
