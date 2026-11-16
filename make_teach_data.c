#include "make_teach_data.h"
#include <stdio.h>
#include <math.h>
#include "config.h"


int main(int argc, char *argv[])
// config.h の T_START から T_END まで KIZAMI 刻みで t=cos(πx) を計算し、
// 1行に x,t を並べたファイル(dataset_load で読める形式)に書き出す
// 成功したら0を返す。失敗したら-1を返す
{
    const char* path = argv[1]; 
    FILE *fp=fopen(path,"w");
    if(fp==NULL) return -1;

    for(int i=0;i<N_T;i++){
        // x+=KIZAMI と足していくと誤差が溜まるので、毎回 i から計算する
        double x=T_START+i*KIZAMI;
        double t=cos(M_PI*x);
        fprintf(fp,"%.15f,%.15f\n",x,t);
    }

    fclose(fp);
    return 0;
}
