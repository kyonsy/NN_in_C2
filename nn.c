#include "nn.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "matrix.h"

// 隠れ層どうしをつなぐ重み W_hid が最低1つ要るので、隠れ層は2つ以上
_Static_assert(N_HIDDEN>=2,"N_HIDDEN は2以上にしてください");


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
//            (1つ前が入力層のときは不要なので、NULL を渡す)
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

static void layer_fprint(FILE *fp,int dout,int din,const double W[dout][din],const double b[dout])
// W を dout 行、b を 1 行で書き出す
// %.17g は double を読み戻したときに元の値と完全に一致する桁数
{
    for(int i=0;i<dout;i++)
        for(int j=0;j<din;j++)
            fprintf(fp,"%.17g%c",W[i][j],j<din-1 ? ' ' : '\n');
    for(int i=0;i<dout;i++)
        fprintf(fp,"%.17g%c",b[i],i<dout-1 ? ' ' : '\n');
}

static int layer_fscan(FILE *fp,int dout,int din,double W[dout][din],double b[dout])
// layer_fprint で書いたものを読み込む。成功したら0、失敗したら-1を返す
{
    for(int i=0;i<dout;i++)
        for(int j=0;j<din;j++)
            if(fscanf(fp,"%lf",&W[i][j])!=1) return -1;
    for(int i=0;i<dout;i++)
        if(fscanf(fp,"%lf",&b[i])!=1) return -1;
    return 0;
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

int nn_save(const NN *net,const char *path)
// 重みとバイアスをテキストファイルに書き出す。成功したら0、失敗したら-1を返す
// ファイルの形式:
//   1行目  : N_HIDDEN N_WIDTH D_IN D_OUT
//   その後 : 入力層側から順に、各層の W(複数行) と b(1行)
{
    const Params *p=&net->p;

    FILE *fp=fopen(path,"w");
    if(fp==NULL) return -1;

    fprintf(fp,"%d %d %d %d\n",N_HIDDEN,N_WIDTH,D_IN,D_OUT);
    layer_fprint(fp,N_WIDTH,D_IN,p->W_in,p->b_in);
    for(int l=0;l<N_HIDDEN-1;l++)
        layer_fprint(fp,N_WIDTH,N_WIDTH,p->W_hid[l],p->b_hid[l]);
    layer_fprint(fp,D_OUT,N_WIDTH,p->W_out,p->b_out);

    fclose(fp);
    return 0;
}

int nn_load(NN *net,const char *path)
// nn_save で書き出したファイルから重みとバイアスを読み込む
// ファイルのネットワークの形が config.h と一致している必要がある
// 成功したら0、失敗したら-1を返す
{
    Params *p=&net->p;

    FILE *fp=fopen(path,"r");
    if(fp==NULL) return -1;

    // ネットワークの形が一致しているか確認する
    int hidden,width,din,dout;
    if(fscanf(fp,"%d %d %d %d",&hidden,&width,&din,&dout)!=4
       || hidden!=N_HIDDEN || width!=N_WIDTH || din!=D_IN || dout!=D_OUT){
        fprintf(stderr,"nn_load: ネットワークの形が config.h と一致しません\n");
        fclose(fp);
        return -1;
    }

    int ok=(layer_fscan(fp,N_WIDTH,D_IN,p->W_in,p->b_in)==0);
    for(int l=0;l<N_HIDDEN-1 && ok;l++)
        ok=(layer_fscan(fp,N_WIDTH,N_WIDTH,p->W_hid[l],p->b_hid[l])==0);
    if(ok)
        ok=(layer_fscan(fp,D_OUT,N_WIDTH,p->W_out,p->b_out)==0);

    fclose(fp);
    if(!ok){
        fprintf(stderr,"nn_load: 重みの読み込みに失敗しました\n");
        return -1;
    }
    return 0;
}
