#ifndef NN_H
#define NN_H

#include "config.h"

// 各層の次元。D[0]は入力層、D[N_LAYERS]は出力層
//   D = { D_IN, N_WIDTH, N_WIDTH, ..., N_WIDTH, D_OUT }
extern const int D[N_LAYERS+1];

// ニューラルネットワーク
//
// 層の番号 k は 1..N_LAYERS を使う(k=0 は入力層なので、重みは持たない)。
// サンプルは n 個(n <= N_T)をまとめて行列として扱う。1行 = 1サンプル。
//
//   第k層の計算:  H[k] = tanh( H[k-1] @ W[k]^T + b[k] )   (最後の層だけ tanh なし)
//                 (n×D[k])    (n×D[k-1]) (D[k-1]×D[k])
//
// 配列はすべて DMAX の大きさで確保してあり、実際に使うのは D[k] までの部分。
// 大きい(数百KB)ので、ローカル変数ではなく static 変数として置くこと。
typedef struct {
    // パラメータ
    double W[N_LAYERS+1][DMAX][DMAX];   // 重み       W[k] : D[k] × D[k-1]
    double b[N_LAYERS+1][DMAX];         // バイアス   b[k] : D[k]
    // 勾配(損失関数 L の W, b に対する勾配)
    double dW[N_LAYERS+1][DMAX][DMAX];  // ∂L/∂W[k]  : D[k] × D[k-1]
    double db[N_LAYERS+1][DMAX];        // ∂L/∂b[k]  : D[k]
    // 途中結果
    double H[N_LAYERS+1][N_T][DMAX];    // 第k層の出力 H[k] : n × D[k]  (H[0]は入力そのもの)
    double G[N_LAYERS+1][N_T][DMAX];    // 第k層の tanh をかける前の値に対する L の勾配 : n × D[k]
} NN;

void nn_init(NN *net);
void nn_forward(NN *net,int n,double X[][D_IN],double Y[][D_OUT]);
void nn_backward(NN *net,int n,double dY[][D_OUT]);
void nn_step(NN *net,double lr);
int  nn_save(const NN *net,const char *path);
int  nn_load(NN *net,const char *path);

#endif
