#ifndef CONFIG_H
#define CONFIG_H

// 全プログラム共通の設定
// ここを変えたら、すべてビルドし直して、教師データ作成・学習からやり直すこと
// (配列の大きさはすべてここの値でコンパイル時に決まる)

// ---- ネットワークの構造 ----
#define N_LAYERS 17   // 層の数(入力層は含めない)
#define N_WIDTH  16   // 中間層1層あたりのノードの数
#define D_IN     1    // 入力の次元
#define D_OUT    1    // 出力の次元

// 層の次元の最大値。重みなどの配列は、すべての層をこの大きさで確保する
#define MAX2(a,b) ((a)>(b) ? (a) : (b))
#define DMAX MAX2(N_WIDTH,MAX2(D_IN,D_OUT))

// ---- 教師データ ----
#define N_T     41    // 教師データの数(サンプル数)
#define T_START 0.0   // 学習範囲の始まり
#define T_END   4.0   // 学習範囲の終わり
#define KIZAMI  ((T_END-T_START)/(N_T-1))  // 刻み幅(N_T個で端から端まで並べる)

// ---- 学習 ----
#define EPOCHS 10000  // 学習の反復回数
#define LR     0.05   // 学習率
#define SEED   0      // 重みの初期化に使う乱数の種

#endif
