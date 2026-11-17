// 教師データ作成用の実行ファイル
// 使い方: make_teach_data.exe > 教師データ.csv
//   config.h の T_START から T_END まで N_T 個の点で t=f(x) を計算し、
//   1行に x,t を並べた形(dataset_load で読める形式)で標準出力に書き出す
#include <stdio.h>
#include <math.h>
#include "config.h"

#define ZETA  0.5  // 減衰比 ζ (0 < ζ < 1 で振動する)
#define OMEGA 1.0  // 固有角周波数 ω


static double f(double x)
// 学習させる関数: 減衰振動
//   y'' + 2ζω y' + ω² y = 0,  y(0)=1, y'(0)=0  の解
//   y(x) = exp(-ζωx) * ( cos(ωd x) + ζ/sqrt(1-ζ²) * sin(ωd x) )
//   ωd = ω sqrt(1-ζ²)  (減衰があるときの実際の角周波数)
{
    double wd=OMEGA*sqrt(1.0-ZETA*ZETA);
    return exp(-ZETA*OMEGA*x)*(cos(wd*x)+ZETA/sqrt(1.0-ZETA*ZETA)*sin(wd*x));
}

int main(void)
{
    for(int i=0;i<N_T;i++){
        // x+=KIZAMI と足していくと誤差が溜まるので、毎回 i から計算する
        double x=T_START+i*KIZAMI;
        printf("%.17g,%.17g\n",x,f(x));
    }
    return 0;
}
