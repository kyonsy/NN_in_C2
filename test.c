#include "test.h"

#include <stdio.h>
#include <stdlib.h>
#include "nrutil.h"
#include "util.h"
#include <time.h>

#define ROOP 1

int test1()
// matmulの速度測定
// ROOP=1,n=1000で測定
// 最適化なし(-O0)のとき(5回平均)
// i -> k -> j の順のとき2.75秒
// i -> j -> k の順のとき3.25秒
// 処理時間 1.18倍
// 最適化あり(-O2)のとき(5回平均)
// i -> k -> j の順のとき0.422秒
// i -> j -> k の順のとき0.828秒
// 処理速度 1.96倍
{
    srand(0);

    int n = 1000;
    double **a = dmatrix(1,n,1,n);
    mat_rand(a,1,n,1,n);
    double **b = dmatrix(1,n,1,n);
    clock_t start=clock();


    for(int i=0;i<ROOP;i++){
        matmul(a,a,b,n,n,n);
    }

    clock_t end=clock();
    double sec = (double)(end-start)/CLOCKS_PER_SEC;
    printf("%.3f sec\n",sec);

    free_dmatrix(a,1,n,1,n);
    free_dmatrix(b,1,n,1,n);
    return 0;
}

