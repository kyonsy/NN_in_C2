#Requires -Version 7
# C 版と Python 版の学習後の重みの平均二乗誤差が、エポック数 によってどう変わるかを調べる
#   学習はせず、speed_compare_epochs.ps1 が csv フォルダに残した学習後の重みを読んで比べるだけ
#   (先に speed_compare_epochs.ps1 を実行しておくこと。重みのファイルがない設定は飛ばす)
#   使い方: .\weight_compare_epochs.ps1 > csv\weight_compare_epochs.csv
#
# 結果は CSV で標準出力に書き出す。1行 = EPOCHS,重みの数,平均二乗誤差
#   平均二乗誤差 = (1/n) Σ_i (c_i - py_i)^2   (n = 重みの数)
#
# 下の $fixed と $list は speed_compare_epochs.ps1 と同じにすること
# (ファイル名の印 幅_深さ_教師データの数_エポック数 がこの設定から決まるので、違うとファイルが見つからない)

$fixed = @(
    "N_WIDTH=16"    # 隠れ層1層あたりのノードの数
    "N_HIDDEN=16"   # 隠れ層の数
    "N_T=1000"      # 教師データの数
)

$list = @(1000, 2000, 4000, 8000, 10000)   # 試すエポック数

. .\run_tag.ps1      # Get-RunTag(ファイル名の印を作る)
. .\weight_mse.ps1   # Get-WeightCompareLine(重みを読んで平均二乗誤差を求める)

foreach ($value in $list) {
    Get-WeightCompareLine $value ($fixed + "EPOCHS=$value")
}
