// PINN(Physics-Informed Neural Network)の学習用の実行ファイル
// 使い方: pinn_main.exe 初期重み.csv > 学習後の重み.csv
//   初期重み.csv : initial_random.exe で作った重み(前に学習した重みを渡せば続きから学習する)
//   学習後の重みは標準出力に、途中経過(損失)は標準エラー出力に書き出す
//   重みの形式は train_main.exe と同じなので、推論は infer_main.exe でできる
//
// 教師データ(正解の y)は使わず、微分方程式と初期条件だけで学習する
//   y'' + 2ζ y' + y = 0,   y(0) = 1,   y'(0) = 0   (固有角周波数 ω は 1)
//
// 損失関数
//   L = (1/n) Σ_s r_s^2  +  LAMBDA_IC * ( (y(0)-1)^2 + y'(0)^2 )
//   r_s = y''(x_s) + 2ζ y'(x_s) + y(x_s)   (微分方程式の残差)
//
// 残差を計算する点(選点) x_s は T_START から T_END まで等間隔に N_T 個。
// 初期条件は先頭の点 x_0 = T_START で課すので、T_START は 0 にしておくこと。
#include <stdio.h>
#include "config.h"
#include "nn.h"

// 大きな配列はスタックからあふれないように static で置く
static NN     net;
static NNd2   d2;                                         // 微分の途中結果(PINN 用)
static double X[N_T][D_IN];                                // 選点
static double Y[N_T][D_OUT],  Y1[N_T][D_OUT],  Y2[N_T][D_OUT];   // y, y', y''
static double dY[N_T][D_OUT], dY1[N_T][D_OUT], dY2[N_T][D_OUT];  // 損失のそれぞれに対する勾配


double pinn_loss(int n,double *L_ode,double *L_ic)
// 損失 L を返し、その y, y', y'' に対する勾配を dY, dY1, dY2 に書き込む
// 内訳(微分方程式の項、初期条件の項)を *L_ode, *L_ic に書き込む
{
    // 微分方程式の項: (1/n) Σ r^2
    //   ∂/∂y'' = 2r/n,  ∂/∂y' = 2ζ * 2r/n,  ∂/∂y = 2r/n
    double Lo=0.0;
    for(int s=0;s<n;s++){
        double r=Y2[s][0]+2.0*ZETA*Y1[s][0]+Y[s][0];
        Lo+=r*r;
        dY2[s][0]=2.0*r/n;
        dY1[s][0]=2.0*ZETA*2.0*r/n;
        dY [s][0]=2.0*r/n;
    }
    Lo/=n;

    // 初期条件の項 (先頭の点 s=0 だけに足す)
    double e0=Y [0][0]-1.0;   // y(0)  - 1
    double e1=Y1[0][0];       // y'(0) - 0
    double Li=e0*e0+e1*e1;
    dY [0][0]+=LAMBDA_IC*2.0*e0;
    dY1[0][0]+=LAMBDA_IC*2.0*e1;

    *L_ode=Lo;
    *L_ic=Li;
    return Lo+LAMBDA_IC*Li;
}

void train(NN *net,NNd2 *d,int epochs,double lr)
// フルバッチの勾配降下法(N_T個の選点を毎回まとめて流す)
{
    for(int ep=1;ep<=epochs;ep++){
        double L_ode,L_ic;

        nn_forward_d2(net,d,N_T,X,Y,Y1,Y2);        // 順伝播 (y, y', y'')
        double L=pinn_loss(N_T,&L_ode,&L_ic);      // 損失
        nn_backward_d2(net,d,N_T,dY,dY1,dY2);      // 逆伝播
        nn_step(net,lr);                           // 更新

        if(ep%N_DISPLAY==0)
            fprintf(stderr,"epoch %d loss %e (ode %e, ic %e)\n",ep,L,L_ode,L_ic);
    }
}

int main(int argc,char **argv)
{
    const char *init_path=argv[1];

    // 選点を並べる(x+=KIZAMI と足していくと誤差が溜まるので、毎回 i から計算する)
    for(int i=0;i<N_T;i++)
        X[i][0]=T_START+i*KIZAMI;

    nn_load(&net,init_path);

    train(&net,&d2,EPOCHS,LR);

    nn_print(&net);
    return 0;
}
