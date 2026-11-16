// 教師データ作成用の実行ファイル
// 使い方: make_data.exe 出力先.csv
//   config.h の T_START から T_END まで N_T 個の点で t=cos(πx) を計算し、
//   1行に x,t を並べたファイル(dataset_load で読める形式)に書き出す
#include <stdio.h>
#include <math.h>
#include "config.h"

#define PI 3.14159265358979323846


int main(int argc,char **argv)
{
    if(argc!=2){
        fprintf(stderr,"使い方: %s 出力先.csv\n",argv[0]);
        return 1;
    }
    const char *path=argv[1];

    FILE *fp=fopen(path,"w");
    if(fp==NULL){
        fprintf(stderr,"ファイルを開けません: %s\n",path);
        return 1;
    }

    for(int i=0;i<N_T;i++){
        // x+=KIZAMI と足していくと誤差が溜まるので、毎回 i から計算する
        double x=T_START+i*KIZAMI;
        double t=cos(PI*x);
        fprintf(fp,"%.15f,%.15f\n",x,t);
    }

    fclose(fp);
    return 0;
}
