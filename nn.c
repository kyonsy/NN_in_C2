#include "nn.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "matrix.h"

// 各層の次元 (GCCの範囲指定 [1 ... N_LAYERS-1] を使って中間層をまとめて N_WIDTH にする)
const int D[N_LAYERS+1] = {
    [0]                = D_IN,
    [1 ... N_LAYERS-1] = N_WIDTH,
    [N_LAYERS]         = D_OUT
};


static double urand(void)
// 一様分布 [0,1)
{
    return rand()/(RAND_MAX+1.0);
}

void nn_init(NN *net)
// 重みを Xavier(Glorot)の一様分布で初期化し、バイアスは0にする
// (乱数の種は呼ぶ前に srand で決めておく)
//
//   W[k] の各要素 ~ 一様分布 [-a, a)   a = sqrt(6 / (D[k-1] + D[k]))
//
// こうすると、どの層でも出力のばらつきが同じくらいに保たれ、
// tanh が ±1 に張り付いて(飽和して)勾配が0になるのを防げる。
// [0,1) の乱数だと値が全部正で大きすぎるため、層を重ねると飽和して学習が進まない。
{
    for(int k=1;k<=N_LAYERS;k++){
        double a=sqrt(6.0/(D[k-1]+D[k]));
        for(int i=0;i<D[k];i++)
            for(int j=0;j<D[k-1];j++)
                net->W[k][i][j]=(2.0*urand()-1.0)*a;
        for(int i=0;i<D[k];i++)
            net->b[k][i]=0.0;
    }
}

void nn_forward(NN *net,int n,double X[][D_IN],double Y[][D_OUT])
// n個のサンプル X(n×D_IN) をまとめて順伝播し、出力を Y(n×D_OUT) に書き込む
{
    // H[0] = X
    for(int s=0;s<n;s++)
        for(int j=0;j<D_IN;j++)
            net->H[0][s][j]=X[s][j];

    for(int k=1;k<=N_LAYERS;k++){
        // H[k] = H[k-1] @ W[k]^T
        mat_mul_ABt(n,D[k-1],D[k],net->H[k-1],net->W[k],net->H[k]);

        // H[k] += b[k]   (どの行(サンプル)にも同じ b を足す)
        for(int s=0;s<n;s++)
            for(int j=0;j<D[k];j++)
                net->H[k][s][j]+=net->b[k][j];

        // 最後の層以外は tanh をかける
        if(k<N_LAYERS){
            for(int s=0;s<n;s++)
                for(int j=0;j<D[k];j++)
                    net->H[k][s][j]=tanh(net->H[k][s][j]);
        }
    }

    // Y = H[N_LAYERS]
    for(int s=0;s<n;s++)
        for(int j=0;j<D_OUT;j++)
            Y[s][j]=net->H[N_LAYERS][s][j];
}

void nn_backward(NN *net,int n,double dY[][D_OUT])
// 逆伝播して勾配 dW, db を計算する(前回の値は上書きされる)
// 直前に同じ n で nn_forward を呼んでおくこと
// dY(n×D_OUT) : 損失関数 L の出力 Y に対する勾配 ∂L/∂Y
{
    // 最後の層は tanh がないので G[N_LAYERS] = ∂L/∂Y
    for(int s=0;s<n;s++)
        for(int j=0;j<D_OUT;j++)
            net->G[N_LAYERS][s][j]=dY[s][j];

    for(int k=N_LAYERS;k>=1;k--){
        // dW[k] = G[k]^T @ H[k-1]       (D[k]×n) @ (n×D[k-1])
        mat_mul_AtB(D[k],n,D[k-1],net->G[k],net->H[k-1],net->dW[k]);

        // db[k] = G[k] を縦(サンプル方向)に足したもの
        for(int j=0;j<D[k];j++){
            net->db[k][j]=0.0;
            for(int s=0;s<n;s++) net->db[k][j]+=net->G[k][s][j];
        }

        // 1つ前の層へ勾配を伝える(k=1 の前は入力層なので不要)
        if(k>1){
            // G[k-1] = G[k] @ W[k]            (n×D[k]) @ (D[k]×D[k-1])
            mat_mul_AB(n,D[k],D[k-1],net->G[k],net->W[k],net->G[k-1]);

            // tanh の微分をかける: tanh'(z) = 1 - tanh(z)^2 = 1 - H^2
            for(int s=0;s<n;s++)
                for(int j=0;j<D[k-1];j++){
                    double h=net->H[k-1][s][j];
                    net->G[k-1][s][j]*=1.0-h*h;
                }
        }
    }
}

void nn_step(NN *net,double lr)
// 勾配降下法で1歩進める: W -= lr*dW, b -= lr*db
{
    for(int k=1;k<=N_LAYERS;k++){
        for(int i=0;i<D[k];i++){
            for(int j=0;j<D[k-1];j++)
                net->W[k][i][j]-=lr*net->dW[k][i][j];
            net->b[k][i]-=lr*net->db[k][i];
        }
    }
}

int nn_save(const NN *net,const char *path)
// 重みとバイアスをテキストファイルに書き出す。成功したら0、失敗したら-1を返す
// ファイルの形式:
//   1行目  : N_LAYERS
//   2行目  : D[0] D[1] ... D[N_LAYERS]
//   その後 : 第1層から順に W(D[k]行) と b(1行)
// %.17g は double を読み戻したときに元の値と完全に一致する桁数
{
    FILE *fp=fopen(path,"w");
    if(fp==NULL) return -1;

    fprintf(fp,"%d\n",N_LAYERS);
    for(int k=0;k<=N_LAYERS;k++) fprintf(fp,"%d%c",D[k],k<N_LAYERS ? ' ' : '\n');

    for(int k=1;k<=N_LAYERS;k++){
        for(int i=0;i<D[k];i++)
            for(int j=0;j<D[k-1];j++)
                fprintf(fp,"%.17g%c",net->W[k][i][j],j<D[k-1]-1 ? ' ' : '\n');
        for(int i=0;i<D[k];i++)
            fprintf(fp,"%.17g%c",net->b[k][i],i<D[k]-1 ? ' ' : '\n');
    }

    fclose(fp);
    return 0;
}

int nn_load(NN *net,const char *path)
// nn_save で書き出したファイルから重みとバイアスを読み込む
// ファイルの層の構造が config.h と一致している必要がある
// 成功したら0、失敗したら-1を返す
{
    FILE *fp=fopen(path,"r");
    if(fp==NULL) return -1;

    // 層の構造が一致しているか確認する
    int fn,fd;
    if(fscanf(fp,"%d",&fn)!=1 || fn!=N_LAYERS){
        fprintf(stderr,"nn_load: 層の数が一致しません\n");
        goto fail;
    }
    for(int k=0;k<=N_LAYERS;k++){
        if(fscanf(fp,"%d",&fd)!=1 || fd!=D[k]){
            fprintf(stderr,"nn_load: D[%d]が一致しません\n",k);
            goto fail;
        }
    }

    for(int k=1;k<=N_LAYERS;k++){
        int ok=1;
        for(int i=0;i<D[k];i++)
            for(int j=0;j<D[k-1];j++)
                if(fscanf(fp,"%lf",&net->W[k][i][j])!=1) ok=0;
        for(int i=0;i<D[k];i++)
            if(fscanf(fp,"%lf",&net->b[k][i])!=1) ok=0;
        if(!ok){
            fprintf(stderr,"nn_load: 第%d層の読み込みに失敗しました\n",k);
            goto fail;
        }
    }

    fclose(fp);
    return 0;

fail:
    fclose(fp);
    return -1;
}
