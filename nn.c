#include "nn.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "matrix.h"


// ============================================================
//  1つの層の計算
//  (入力層→隠れ層、隠れ層→隠れ層、隠れ層→出力層 の3種類で大きさだけが違うので、
//   大きさ din, dout を引数で受け取る共通の関数にしておく)
// ============================================================

static double urand(void)
// 一様分布 [0,1)
{
    return rand()/(RAND_MAX+1.0);
}

static void layer_init(int dout,int din,double W[dout][din],double b[dout])
// 重みを Xavier(Glorot)の一様分布で初期化し、バイアスは0にする
//
//   W の各要素 ~ 一様分布 [-a, a)   a = sqrt(6 / (din + dout))
//
// こうすると、どの層でも出力のばらつきが同じくらいに保たれ、
// tanh が ±1 に張り付いて(飽和して)勾配が0になるのを防げる。
{
    double a=sqrt(6.0/(din+dout));
    for(int i=0;i<dout;i++)
        for(int j=0;j<din;j++)
            W[i][j]=(2.0*urand()-1.0)*a;
    for(int i=0;i<dout;i++)
        b[i]=0.0;
}

static void layer_forward(int n,int din,int dout,
                          double Hin[n][din],double W[dout][din],double b[dout],
                          double Hout[n][dout],int use_tanh)
// Hout = Hin @ W^T + b   (use_tanh が真なら、さらに tanh をかける)
//  (n×dout) (n×din)(din×dout)
{
    mat_mul_ABt(n,din,dout,Hin,W,Hout);

    // どの行(サンプル)にも同じ b を足す
    for(int s=0;s<n;s++)
        for(int j=0;j<dout;j++)
            Hout[s][j]+=b[j];

    if(use_tanh){
        for(int s=0;s<n;s++)
            for(int j=0;j<dout;j++)
                Hout[s][j]=tanh(Hout[s][j]);
    }
}

static void layer_backward(int n,int din,int dout,
                           double Hin[n][din],double W[dout][din],double Gout[n][dout],
                           double dW[dout][din],double db[dout],double Gin[n][din])
// layer_forward の逆伝播
//   Gout : L の「この層の tanh をかける前の値」に対する勾配 (n×dout)
// を受け取り、次を計算する
//   dW, db : L の W, b に対する勾配
//   Gin    : L の「1つ前の層の tanh をかける前の値」に対する勾配 (n×din)
//            (1つ前が入力層のときは計算しなくてよいので、NULL を渡す)
{
    // dW = Gout^T @ Hin       (dout×n) @ (n×din)
    mat_mul_AtB(dout,n,din,Gout,Hin,dW);

    // db = Gout を縦(サンプル方向)に足したもの
    for(int j=0;j<dout;j++){
        db[j]=0.0;
        for(int s=0;s<n;s++) db[j]+=Gout[s][j];
    }

    if(Gin!=NULL){
        // Gin = Gout @ W          (n×dout) @ (dout×din)
        mat_mul_AB(n,dout,din,Gout,W,Gin);

        // tanh の微分をかける: tanh'(z) = 1 - tanh(z)^2 = 1 - Hin^2
        for(int s=0;s<n;s++)
            for(int j=0;j<din;j++)
                Gin[s][j]*=1.0-Hin[s][j]*Hin[s][j];
    }
}

static void layer_step(int dout,int din,double W[dout][din],double b[dout],
                       double dW[dout][din],double db[dout],double lr)
// 勾配降下法で1歩進める: W -= lr*dW, b -= lr*db
{
    for(int i=0;i<dout;i++){
        for(int j=0;j<din;j++)
            W[i][j]-=lr*dW[i][j];
        b[i]-=lr*db[i];
    }
}

static void layer_print(int dout,int din,const double W[dout][din],const double b[dout])
// W を dout 行、b を 1 行で、カンマ区切りで標準出力に書き出す
// %.17g は double を読み戻したときに元の値と完全に一致する桁数
{
    for(int i=0;i<dout;i++)
        for(int j=0;j<din;j++)
            printf("%.17g%c",W[i][j],j<din-1 ? ',' : '\n');
    for(int i=0;i<dout;i++)
        printf("%.17g%c",b[i],i<dout-1 ? ',' : '\n');
}

static void layer_fscan(FILE *fp,int dout,int din,double W[dout][din],double b[dout])
// layer_print で書いたものを読み込む
// "%lf," : 数値を読んで、後ろにカンマがあれば読み飛ばす(行末の改行は次の %lf が読み飛ばす)
{
    for(int i=0;i<dout;i++)
        for(int j=0;j<din;j++)
            fscanf(fp,"%lf,",&W[i][j]);
    for(int i=0;i<dout;i++)
        fscanf(fp,"%lf,",&b[i]);
}


// ============================================================
//  ネットワーク全体の計算
//  どれも「入力層→隠れ層」「隠れ層→隠れ層 を繰り返す」「隠れ層→出力層」の3段階
// ============================================================

void nn_init(NN *net)
// 重みとバイアスを初期化する(乱数の種は呼ぶ前に srand で決めておく)
{
    Params *p=&net->p;

    layer_init(N_WIDTH,D_IN,p->W_in,p->b_in);
    for(int l=0;l<N_HIDDEN-1;l++)
        layer_init(N_WIDTH,N_WIDTH,p->W_hid[l],p->b_hid[l]);
    layer_init(D_OUT,N_WIDTH,p->W_out,p->b_out);
}

void nn_forward(NN *net,int n,double X[][D_IN],double Y[][D_OUT])
// n個のサンプル X(n×D_IN) をまとめて順伝播し、出力を Y(n×D_OUT) に書き込む
{
    Params *p=&net->p;

    // 逆伝播で使うので入力を取っておく
    for(int s=0;s<n;s++)
        for(int j=0;j<D_IN;j++)
            net->X[s][j]=X[s][j];

    // 入力層 → 隠れ層0
    layer_forward(n,D_IN,N_WIDTH,net->X,p->W_in,p->b_in,net->H[0],1);

    // 隠れ層l → 隠れ層l+1
    for(int l=0;l<N_HIDDEN-1;l++)
        layer_forward(n,N_WIDTH,N_WIDTH,net->H[l],p->W_hid[l],p->b_hid[l],net->H[l+1],1);

    // 最後の隠れ層 → 出力層 (tanh なし)
    layer_forward(n,N_WIDTH,D_OUT,net->H[N_HIDDEN-1],p->W_out,p->b_out,Y,0);
}

void nn_backward(NN *net,int n,double dY[][D_OUT])
// 逆伝播して勾配 net->grad を計算する(前回の値は上書きされる)
// 直前に同じ n で nn_forward を呼んでおくこと
// dY(n×D_OUT) : 損失関数 L の出力 Y に対する勾配 ∂L/∂Y
//               (出力層は tanh がないので、これがそのまま出力層の Gout になる)
{
    Params *p=&net->p, *g=&net->grad;

    // 出力層 → 最後の隠れ層
    layer_backward(n,N_WIDTH,D_OUT,net->H[N_HIDDEN-1],p->W_out,dY,
                   g->W_out,g->b_out,net->G[N_HIDDEN-1]);

    // 隠れ層l+1 → 隠れ層l
    for(int l=N_HIDDEN-2;l>=0;l--)
        layer_backward(n,N_WIDTH,N_WIDTH,net->H[l],p->W_hid[l],net->G[l+1],
                       g->W_hid[l],g->b_hid[l],net->G[l]);

    // 隠れ層0 → 入力層 (入力層の勾配は不要なので NULL)
    layer_backward(n,D_IN,N_WIDTH,net->X,p->W_in,net->G[0],
                   g->W_in,g->b_in,NULL);
}

void nn_step(NN *net,double lr)
// 勾配降下法で1歩進める
{
    Params *p=&net->p, *g=&net->grad;

    layer_step(N_WIDTH,D_IN,p->W_in,p->b_in,g->W_in,g->b_in,lr);
    for(int l=0;l<N_HIDDEN-1;l++)
        layer_step(N_WIDTH,N_WIDTH,p->W_hid[l],p->b_hid[l],g->W_hid[l],g->b_hid[l],lr);
    layer_step(D_OUT,N_WIDTH,p->W_out,p->b_out,g->W_out,g->b_out,lr);
}

void nn_print(const NN *net)
// 重みとバイアスを CSV 形式で標準出力に書き出す
// 形式: 入力層側から順に、各層の W(複数行) と b(1行)。値はカンマ区切り
{
    const Params *p=&net->p;

    layer_print(N_WIDTH,D_IN,p->W_in,p->b_in);
    for(int l=0;l<N_HIDDEN-1;l++)
        layer_print(N_WIDTH,N_WIDTH,p->W_hid[l],p->b_hid[l]);
    layer_print(D_OUT,N_WIDTH,p->W_out,p->b_out);
}

void nn_load(NN *net,const char *path)
// nn_print で書き出したファイルから重みとバイアスを読み込む
// (保存したときと config.h のネットワークの形が同じであること)
{
    Params *p=&net->p;

    FILE *fp=fopen(path,"r");

    layer_fscan(fp,N_WIDTH,D_IN,p->W_in,p->b_in);
    for(int l=0;l<N_HIDDEN-1;l++)
        layer_fscan(fp,N_WIDTH,N_WIDTH,p->W_hid[l],p->b_hid[l]);
    layer_fscan(fp,D_OUT,N_WIDTH,p->W_out,p->b_out);

    fclose(fp);
}


// ============================================================
//  PINN 用: 出力 y だけでなく、入力 x による微分 y' = dy/dx, y'' = d²y/dx² も計算する
//
//  計算グラフは作らず、連鎖律で y, y', y'' を層ごとに直接計算する。
//  1つの層を z = 前の層 @ W^T + b,  a = tanh(z) とすると
//
//    z   = a_prev   @ W^T + b
//    z'  = a_prev'  @ W^T          (b は x によらないので微分すると消える)
//    z'' = a_prev'' @ W^T
//
//    a   = tanh(z)
//    a'  = s z'                    s = tanh'(z) = 1 - a^2
//    a'' = s z'' - 2 a s z'^2      (s' = ds/dz * z' = -2 a s z' を使う)
//
//  入力層は a_prev = x, a_prev' = 1, a_prev'' = 0 とすれば同じ式で計算できる。
//  x で微分するので D_IN は 1 でなければならない。
// ============================================================

static_assert(D_IN==1,"PINN 用の関数は入力が1次元(D_IN==1)のときだけ使える");

static void layer_forward_d2(int n,int din,int dout,
                             double Hin[n][din],double Hin1[n][din],double Hin2[n][din],
                             double W[dout][din],double b[dout],
                             double Z1[n][dout],double Z2[n][dout],
                             double Hout[n][dout],double Hout1[n][dout],double Hout2[n][dout],
                             int use_tanh)
// layer_forward の微分つき版
//   Hin, Hin1, Hin2    : 前の層の a, a', a''
//   Hout, Hout1, Hout2 : この層の a, a', a''  (use_tanh が偽なら z, z', z'')
//   Z1, Z2             : この層の z', z''  (逆伝播で使うので取っておく。use_tanh が偽なら NULL でよい)
{
    // まず z, z', z'' を Hout, Hout1, Hout2 に計算する
    mat_mul_ABt(n,din,dout,Hin ,W,Hout );
    mat_mul_ABt(n,din,dout,Hin1,W,Hout1);
    mat_mul_ABt(n,din,dout,Hin2,W,Hout2);
    for(int s=0;s<n;s++)
        for(int j=0;j<dout;j++)
            Hout[s][j]+=b[j];

    if(use_tanh){
        for(int s=0;s<n;s++)
            for(int j=0;j<dout;j++){
                double z1=Hout1[s][j], z2=Hout2[s][j];
                double a=tanh(Hout[s][j]);
                double sd=1.0-a*a;   // s = tanh'(z)

                Z1[s][j]=z1;
                Z2[s][j]=z2;
                Hout [s][j]=a;
                Hout1[s][j]=sd*z1;
                Hout2[s][j]=sd*z2-2.0*a*sd*z1*z1;
            }
    }
}

static void layer_backward_d2(int n,int din,int dout,
                              double Hin[n][din],double Hin1[n][din],double Hin2[n][din],
                              double Zin1[n][din],double Zin2[n][din],
                              double W[dout][din],
                              double Gout[n][dout],double Gout1[n][dout],double Gout2[n][dout],
                              double dW[dout][din],double db[dout],
                              double Gin[n][din],double Gin1[n][din],double Gin2[n][din])
// layer_backward の微分つき版
//   Gout, Gout1, Gout2 : L の この層の z, z', z'' に対する勾配 (n×dout)
// を受け取り、次を計算する
//   dW, db             : L の W, b に対する勾配
//   Gin, Gin1, Gin2    : L の 1つ前の層の z, z', z'' に対する勾配 (n×din)
//                        (1つ前が入力層のときは不要なので、Gin に NULL を渡す)
//   Hin, Hin1, Hin2    : 1つ前の層の a, a', a''
//   Zin1, Zin2         : 1つ前の層の z', z''  (Gin が NULL なら使わない)
{
    // z = Hin W^T + b,  z' = Hin1 W^T,  z'' = Hin2 W^T  の3つすべてに W が入っているので
    // dW = Gout^T @ Hin + Gout1^T @ Hin1 + Gout2^T @ Hin2
    for(int i=0;i<dout;i++)
        for(int j=0;j<din;j++){
            double sum=0.0;
            for(int s=0;s<n;s++)
                sum+=Gout[s][i]*Hin[s][j]+Gout1[s][i]*Hin1[s][j]+Gout2[s][i]*Hin2[s][j];
            dW[i][j]=sum;
        }

    // b は z にしか入っていない
    for(int i=0;i<dout;i++){
        db[i]=0.0;
        for(int s=0;s<n;s++) db[i]+=Gout[s][i];
    }

    if(Gin!=NULL){
        // まず L の 1つ前の層の a, a', a'' に対する勾配 (それぞれ Gout @ W など)
        mat_mul_AB(n,dout,din,Gout ,W,Gin );
        mat_mul_AB(n,dout,din,Gout1,W,Gin1);
        mat_mul_AB(n,dout,din,Gout2,W,Gin2);

        // tanh の部分を逆にたどって z, z', z'' に対する勾配にする
        //   a   = tanh(z)
        //   a'  = s z'
        //   a'' = s z'' - 2 a s z'^2           (s = 1 - a^2,  ds/dz = -2 a s)
        // をそれぞれ z, z', z'' で偏微分すると
        //   ∂a/∂z   = s
        //   ∂a'/∂z  = -2 a s z'             ∂a'/∂z'  = s
        //   ∂a''/∂z = -2 a s z'' - 2 s (s - 2 a^2) z'^2
        //   ∂a''/∂z' = -4 a s z'            ∂a''/∂z'' = s
        for(int s=0;s<n;s++)
            for(int j=0;j<din;j++){
                double a=Hin[s][j], z1=Zin1[s][j], z2=Zin2[s][j];
                double sd=1.0-a*a;
                double g0=Gin[s][j], g1=Gin1[s][j], g2=Gin2[s][j];

                Gin [s][j]=g0*sd
                          +g1*(-2.0*a*sd*z1)
                          +g2*(-2.0*a*sd*z2-2.0*sd*(sd-2.0*a*a)*z1*z1);
                Gin1[s][j]=g1*sd
                          +g2*(-4.0*a*sd*z1);
                Gin2[s][j]=g2*sd;
            }
    }
}

void nn_forward_d2(NN *net,NNd2 *d,int n,double X[][D_IN],
                   double Y[][D_OUT],double Y1[][D_OUT],double Y2[][D_OUT])
// n個の入力 X について、出力 Y = y と、その x による微分 Y1 = y', Y2 = y'' を計算する
// 微分の途中結果は d に、微分でない途中結果は net に取っておく
{
    Params *p=&net->p;

    // 入力 x と、その微分 x' = 1, x'' = 0
    for(int s=0;s<n;s++)
        for(int j=0;j<D_IN;j++){
            net->X [s][j]=X[s][j];
            d->X1[s][j]=1.0;
            d->X2[s][j]=0.0;
        }

    // 入力層 → 隠れ層0
    layer_forward_d2(n,D_IN,N_WIDTH,net->X,d->X1,d->X2,p->W_in,p->b_in,
                     d->Z1[0],d->Z2[0],net->H[0],d->H1[0],d->H2[0],1);

    // 隠れ層l → 隠れ層l+1
    for(int l=0;l<N_HIDDEN-1;l++)
        layer_forward_d2(n,N_WIDTH,N_WIDTH,net->H[l],d->H1[l],d->H2[l],p->W_hid[l],p->b_hid[l],
                         d->Z1[l+1],d->Z2[l+1],net->H[l+1],d->H1[l+1],d->H2[l+1],1);

    // 最後の隠れ層 → 出力層 (tanh なし)
    layer_forward_d2(n,N_WIDTH,D_OUT,net->H[N_HIDDEN-1],d->H1[N_HIDDEN-1],d->H2[N_HIDDEN-1],
                     p->W_out,p->b_out,NULL,NULL,Y,Y1,Y2,0);
}

void nn_backward_d2(NN *net,NNd2 *d,int n,
                    double dY[][D_OUT],double dY1[][D_OUT],double dY2[][D_OUT])
// 逆伝播して勾配 net->grad を計算する(前回の値は上書きされる)
// 直前に同じ net, d, n で nn_forward_d2 を呼んでおくこと
//   dY, dY1, dY2 : 損失関数 L の y, y', y'' に対する勾配 ∂L/∂y, ∂L/∂y', ∂L/∂y''
{
    Params *p=&net->p, *g=&net->grad;
    const int last=N_HIDDEN-1;

    // 出力層 → 最後の隠れ層
    layer_backward_d2(n,N_WIDTH,D_OUT,net->H[last],d->H1[last],d->H2[last],
                      d->Z1[last],d->Z2[last],p->W_out,dY,dY1,dY2,
                      g->W_out,g->b_out,net->G[last],d->G1[last],d->G2[last]);

    // 隠れ層l+1 → 隠れ層l
    for(int l=N_HIDDEN-2;l>=0;l--)
        layer_backward_d2(n,N_WIDTH,N_WIDTH,net->H[l],d->H1[l],d->H2[l],
                          d->Z1[l],d->Z2[l],p->W_hid[l],net->G[l+1],d->G1[l+1],d->G2[l+1],
                          g->W_hid[l],g->b_hid[l],net->G[l],d->G1[l],d->G2[l]);

    // 隠れ層0 → 入力層 (入力層の勾配は不要なので NULL)
    layer_backward_d2(n,D_IN,N_WIDTH,net->X,d->X1,d->X2,NULL,NULL,
                      p->W_in,net->G[0],d->G1[0],d->G2[0],
                      g->W_in,g->b_in,NULL,NULL,NULL);
}
