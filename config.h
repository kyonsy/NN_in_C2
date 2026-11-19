#ifndef CONFIG_H
#define CONFIG_H

// 全プログラム共通の設定
// ここを変えたら、すべてビルドし直して、教師データ作成・学習からやり直すこと
// (配列の大きさはすべてここの値でコンパイル時に決まる)
//
// #ifndef ～ #endif で囲んだ値は、コンパイルするときに gcc の -D で上書きできる
//   例: .\c_build.ps1 -DN_WIDTH=32 -DN_HIDDEN=4
// -D で渡さなかった値は、ここに書いた既定値になる

// ---- ネットワークの構造 ----
//   入力層(D_IN) → 隠れ層(N_WIDTH) × N_HIDDEN 個 → 出力層(D_OUT)
// 隠れ層の幅はすべて同じ N_WIDTH にする
#ifndef N_HIDDEN
#define N_HIDDEN 16   // 隠れ層の数(2以上)
#endif
#ifndef N_WIDTH
#define N_WIDTH  16   // 隠れ層1層あたりのノードの数
#endif
#define D_IN     1    // 入力の次元  (教師データの形で決まるので -D では変えない)
#define D_OUT    1    // 出力の次元  (同上)

// ---- 教師データ ----
#ifndef N_T
#define N_T     41    // 教師データの数(サンプル数)
#endif
#ifndef T_START
#define T_START 0.0   // 学習範囲の始まり
#endif
#ifndef T_END
#define T_END   4.0   // 学習範囲の終わり
#endif
#define KIZAMI  ((T_END-T_START)/(N_T-1))  // 刻み幅(N_T個で端から端まで並べる)

// ---- 学習 ----
#ifndef EPOCHS
#define EPOCHS 10000  // 学習の反復回数
#endif
#ifndef LR
#define LR     0.05   // 学習率
#endif
#ifndef SEED
#define SEED   0      // 重みの初期化に使う乱数の種
#endif

#define N_DISPLAY 1000

// ---- 解く微分方程式(make_teach_data と pinn_main で共通) ----
//   y'' + 2ζ y' + y = 0,  y(0) = 1,  y'(0) = 0
//   (固有角周波数 ω は 1 に固定している。一般の y'' + 2ζω y' + ω² y = 0 で ω = 1 としたもの)
//   (初期条件も y(0) = 1, y'(0) = 0 に固定している)
#define ZETA   0.5    // 減衰比 ζ (0 < ζ < 1 で振動する)

// ---- PINN ----
#ifndef LAMBDA_IC
#define LAMBDA_IC 1.0 // 損失のうち初期条件の項にかける重み
#endif

#endif
