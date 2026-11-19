#Requires -Version 7
# ある設定で NN を1回動かす(ビルド → 教師データ作成 → 初期化 → 学習 → 推論)
#   使い方: 下の $settings を書き換えてから .\c_experiment.ps1
#
# $settings に書いた値は config.h の既定値を上書きする(c_build.ps1 が gcc の -D で渡す)。
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

# 何回も使うファイル名
$teach_data  = "csv\teach_data.csv"         # 教師データ

# 初期の重み・学習後の重み・推論の出力は、名前の後ろに 幅_深さ_教師データの数_エポック数 を付けて、
# 上書きしないようにする(例: trained_weight_16_16_10000_10000_c.csv)
. .\run_tag.ps1
$tag         = Get-RunTag $settings
$init_weight = "csv\initial_weight_$tag.csv"     # 初期の重み
$c_weight    = "csv\trained_weight_${tag}_c.csv" # 学習後の重み
$c_infer     = "csv\infer_output_${tag}_c.csv"   # 推論の出力

# 最後に必ず実行ファイルを消す
# (途中でエラーの exit や Ctrl+C で止まったときも、finally の中は実行される)
try {
    # この設定でビルドする
    .\c_build.ps1 @settings

    # 教師データを作る
    .\make_teach_data.exe > $teach_data

    # 初期の重みを作る
    .\initial_random.exe > $init_weight

    # 学習する(Measure-Command で { } の中の実行にかかった時間を測る)
    $time = Measure-Command { .\train_main.exe $teach_data $init_weight > $c_weight }
    Write-Host ("学習時間: {0:F0} ミリ秒" -f $time.TotalMilliseconds)

    # 推論する
    .\infer_main.exe $c_weight $teach_data > $c_infer
}
finally {
    # ビルドした実行ファイルを消す
    .\c_clean.ps1
}
