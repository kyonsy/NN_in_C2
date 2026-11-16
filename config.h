#ifndef CONFIG_H
#define CONFIG_H

// 学習用(train_main.c)と推論用(infer_main.c)で共通の設定
// ネットワークの構造を変えたら、両方をビルドし直して学習からやり直すこと

// ネットワークの構造
#define N_LAYERS 17  // 層の数(入力層は含めない)
#define N_WIDTH 16   // 1層あたりのノードの数
#define D_IN  1      // 入力の次元
#define D_OUT 1      // 出力の次元                      
static int D[N_LAYERS+1] = {
    [0] = D_IN,
    [1 ... N_LAYERS-1] = N_WIDTH,
    [N_LAYERS] = D_OUT
};
#define EPOCHS 500    // 学習の反復回数
#define LR     0.05    // 学習率
#define SEED   0       // 重みの初期化に使う乱数の種

// 学習データの形
#define T_START 0.0 // 学習範囲の始まり
#define T_END   4.0 // 学習範囲の終わり
#define KIZAMI 0.1 // 刻み幅
#define N_T  ((int)((T_END-T_START)/KIZAMI+0.5)+1)  // 教師データの数

#endif
