#ifndef NN_H
#define NN_H

typedef struct {
    // パラメータ
    double **W;   // 重み W[1..d[k]][1..d[k-1]]
    double *b;    // バイアス b[1..d[k]]
    // 勾配
    double **dW;  // 損失関数のWに対する勾配
    double *db;   // 損失関数のbに対する勾配
} Layer;

typedef struct {
    int n;        // 層の数(入力層は含めない)
    int *d;       // 各層の次元(d[0]は入力層の次元)
    int dmax;     // 層の次元の最大値
    double **h;   // 各層の出力結果 h[0..n](h[0]は入力)
    Layer *layer; // 各層 layer[1..n]
    double *work; // 逆伝播計算用のメモリ
} NN;

void nn_init(NN *net,int n,int *d);
void nn_free(NN *net);
void nn_forward(NN *net,double *x,double *y);
void nn_backward(NN *net,double *dy);
void nn_step(NN *net,double lr);
void nn_zero_grad(NN *net);
int nn_export_weights(const NN *net,const char *path);
int nn_import_weights(NN *net,const char *path);

#endif
