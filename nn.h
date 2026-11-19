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

// PINN 用の途中結果(nn_forward_d2 / nn_backward_d2 だけが使う)
// 入力 x による1階微分 ' と2階微分 '' も一緒に流すための配列。
//   z = tanh をかける前の値、 a = tanh(z)
// a, x, z に対する勾配は NN の H, X, G をそのまま使い、ここには ' と '' の分だけを置く。
// 普通の学習(train_main)では使わないので、NN とは別にして PINN のときだけ確保する。
// これも大きいので static 変数として置くこと。
typedef struct {
    double X1[N_T][D_IN];               // x'  = 1
    double X2[N_T][D_IN];               // x'' = 0
    double H1[N_HIDDEN][N_T][N_WIDTH];  // a'
    double H2[N_HIDDEN][N_T][N_WIDTH];  // a''
    double Z1[N_HIDDEN][N_T][N_WIDTH];  // z'
    double Z2[N_HIDDEN][N_T][N_WIDTH];  // z''
    double G1[N_HIDDEN][N_T][N_WIDTH];  // L の z'  に対する勾配
    double G2[N_HIDDEN][N_T][N_WIDTH];  // L の z'' に対する勾配
} NNd2;

void nn_init(NN *net);
void nn_forward(NN *net,int n,double X[][D_IN],double Y[][D_OUT]);
void nn_backward(NN *net,int n,double dY[][D_OUT]);
void nn_forward_d2(NN *net,NNd2 *d,int n,double X[][D_IN],
                   double Y[][D_OUT],double Y1[][D_OUT],double Y2[][D_OUT]);
void nn_backward_d2(NN *net,NNd2 *d,int n,
                    double dY[][D_OUT],double dY1[][D_OUT],double dY2[][D_OUT]);
void nn_step(NN *net,double lr);
void nn_print(const NN *net);
void nn_load(NN *net,const char *path);

#endif
