#ifndef CONFIG_H
#define CONFIG_H

// 全プログラム共通の設定
// ここを変えたら、すべてビルドし直して、教師データ作成・学習からやり直すこと
// (配列の大きさはすべてここの値でコンパイル時に決まる)

// ---- ネットワークの構造 ----
//   入力層(D_IN) → 隠れ層(N_WIDTH) × N_HIDDEN 個 → 出力層(D_OUT)
// 隠れ層の幅はすべて同じ N_WIDTH にする
#define N_HIDDEN 16   // 隠れ層の数(2以上)
#define N_WIDTH  16   // 隠れ層1層あたりのノードの数
#define D_IN     1    // 入力の次元
#define D_OUT    1    // 出力の次元

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
