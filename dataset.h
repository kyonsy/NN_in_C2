#ifndef DATASET_H
#define DATASET_H

typedef struct {
    int N;        // サンプル数
    int din;      // 入力の次元
    int dout;     // 出力(教師データ)の次元
    double **X;   // 入力 X[1..N][1..din]
    double **T;   // 教師データ T[1..N][1..dout]
} Dataset;

void dataset_init(Dataset *ds,int N,int din,int dout);
void dataset_free(Dataset *ds);
int dataset_load(Dataset *ds,const char *path,int N,int din,int dout);

#endif
