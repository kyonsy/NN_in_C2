#ifndef DATASET_H
#define DATASET_H

#include "config.h"

// 教師データ N_T 個をまとめたもの(1行 = 1サンプル)
typedef struct {
    double X[N_T][D_IN];   // 入力
    double T[N_T][D_OUT];  // 正解(教師データ)
} Dataset;

int dataset_load(Dataset *ds,const char *path);

#endif
