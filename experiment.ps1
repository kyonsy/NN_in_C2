# ある設定で NN を1回動かす(ビルド → 教師データ作成 → 初期化 → 学習 → 推論)
#   使い方: 下の $settings を書き換えてから .\experiment.ps1
#
# $settings に書いた値は config.h の既定値を上書きする(build.ps1 が gcc の -D で渡す)。
# 書かなかった値は config.h の既定値になる。

$settings = @(
    "N_HIDDEN=16"   # 隠れ層の数(2以上)
    "N_WIDTH=16"    # 隠れ層1層あたりのノードの数
    "N_T=10000"        # 教師データの数
    "T_START=0.0"   # 学習範囲の始まり
    "T_END=4.0"     # 学習範囲の終わり
    "EPOCHS=10000"  # 学習の反復回数
    "LR=0.05"       # 学習率
    "SEED=0"        # 重みの初期化に使う乱数の種
)

# この設定でビルドする
.\build.ps1 @settings

# 教師データを作る
.\make_teach_data.ps1

# 初期の重みを作る
.\initial_random.ps1

# 学習する(Measure-Command で { } の中の実行にかかった時間を測る)
$time = Measure-Command { .\train_main.ps1 }
Write-Host ("学習時間: {0:F2} 秒" -f $time.TotalSeconds)

# 推論する
.\infer_main.ps1