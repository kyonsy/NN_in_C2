#Requires -Version 7
# PINN を1回動かす(ビルド → 正解データ作成 → 初期化 → 学習 → 推論)
#   使い方: 下の $settings を書き換えてから .\pinn_experiment.ps1
#
# $settings に書いた値は config.h の既定値を上書きする(c_build.ps1 が gcc の -D で渡す)。
# 書かなかった値は config.h の既定値になる。
#
# PINN は教師データを使わずに学習する。make_teach_data.exe で作る厳密解は、
# 推論の入力 x と、結果と比べるための正解としてだけ使う。

$settings = @(
    "N_HIDDEN=16"    # 隠れ層の数(2以上)
    "N_WIDTH=16"     # 隠れ層1層あたりのノードの数
    "N_T=41"         # 選点(微分方程式の残差を計算する点)の数
    "T_START=0.0"    # 範囲の始まり(初期条件をここで課すので 0 のままにする)
    "T_END=4.0"      # 範囲の終わり
    "EPOCHS=10000"   # 学習の反復回数
    "LR=0.05"        # 学習率
    "SEED=0"         # 重みの初期化に使う乱数の種
    "LAMBDA_IC=1.0"  # 損失のうち初期条件の項にかける重み
)

# 何回も使うファイル名
$exact_data  = "csv\teach_data.csv"          # 厳密解(比べるための正解)

# 初期の重み・学習後の重み・推論の出力は、名前の後ろに 幅_深さ_選点の数_エポック数 を付けて、
# 上書きしないようにする(例: pinn_trained_weight_16_16_41_10000_c.csv)
. .\run_tag.ps1
$tag         = Get-RunTag $settings
$init_weight = "csv\initial_weight_$tag.csv"          # 初期の重み
$pinn_weight = "csv\pinn_trained_weight_${tag}_c.csv" # 学習後の重み
$pinn_infer  = "csv\pinn_infer_output_${tag}_c.csv"   # 推論の出力

# 最後に必ず実行ファイルを消す
# (途中でエラーの exit や Ctrl+C で止まったときも、finally の中は実行される)
try {
    # この設定でビルドする
    .\c_build.ps1 @settings

    # 厳密解を作る
    .\make_teach_data.exe > $exact_data

    # 初期の重みを作る
    .\initial_random.exe > $init_weight

    # 学習する(Measure-Command で { } の中の実行にかかった時間を測る)
    $time = Measure-Command { .\pinn_main.exe $init_weight > $pinn_weight }
    Write-Host ("学習時間: {0:F0} ミリ秒" -f $time.TotalMilliseconds)

    # 推論する(1行に x,y)
    .\infer_main.exe $pinn_weight $exact_data > $pinn_infer
}
finally {
    # ビルドした実行ファイルを消す
    .\c_clean.ps1
}
