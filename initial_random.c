// 初期の重みを作る実行ファイル
// 使い方: initial_random.exe > 初期重み.csv
//   config.h の SEED を乱数の種にして重みを初期化し(nn_init)、
//   CSV 形式で標準出力に書き出す。train_main.exe はこれを読み込んで学習を始める
#include <stdlib.h>
#include "config.h"
#include "nn.h"

// 大きいのでスタックからあふれないように static で置く
static NN net;


int main(void)
{
    srand(SEED);
    nn_init(&net);
    nn_print(&net);
    return 0;
}
