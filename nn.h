#ifndef NN_H
#define NN_H

#include "config.h"

// ニューラルネットワーク
//
//   入力 X ──W_in──→ H[0] ──W_hid[0]──→ H[1] ── … ──→ H[N_HIDDEN-1] ──W_out──→ 出力 Y
//  (n×D_IN)        (n×N_WIDTH)        (n×N_WIDTH)      (n×N_WIDTH)          (n×D_OUT)
//
// 隠れ層の幅はすべて N_WIDTH なので、隠れ層どうしをつなぐ重みはすべて N_WIDTH×N_WIDTH。
// サンプルは n 個(n <= N_T)をまとめて行列として扱う。1行 = 1サンプル。
//
//   隠れ層:  H[l] = tanh( H[l-1] @ W^T + b )
//   出力層:  Y    =       H[N_HIDDEN-1] @ W_out^T + b_out    (tanh なし)
//
// 大きい(数百KB)ので、ローカル変数ではなく static 変数として置くこと。

// 重みとバイアス一式(勾配も同じ形なので、この型を2つ使う)
typedef struct {
    double W_in[N_WIDTH][D_IN];                  // 入力層 → 隠れ層0
    double b_in[N_WIDTH];
    double W_hid[N_HIDDEN-1][N_WIDTH][N_WIDTH];  // W_hid[l] : 隠れ層l → 隠れ層l+1
    double b_hid[N_HIDDEN-1][N_WIDTH];
    double W_out[D_OUT][N_WIDTH];                // 最後の隠れ層 → 出力層
    double b_out[D_OUT];
} Params;

typedef struct {
    Params p;     // パラメータ
    Params grad;  // 損失関数 L のパラメータに対する勾配(p と同じ形)
    // 途中結果(逆伝播で使うので順伝播のときに取っておく)
    double X[N_T][D_IN];               // 入力
    double H[N_HIDDEN][N_T][N_WIDTH];  // 隠れ層lの出力 H[l] : n × N_WIDTH
    double G[N_HIDDEN][N_T][N_WIDTH];  // 隠れ層lの tanh をかける前の値に対する L の勾配
} NN;

void nn_init(NN *net);
void nn_forward(NN *net,int n,double X[][D_IN],double Y[][D_OUT]);
void nn_backward(NN *net,int n,double dY[][D_OUT]);
void nn_step(NN *net,double lr);
int  nn_save(const NN *net,const char *path);
int  nn_load(NN *net,const char *path);

#endif
