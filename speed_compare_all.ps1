#Requires -Version 7
# C 版と Python 版の速度比較を全部まとめて実行し、結果の CSV とグラフの画像を保存する
#   使い方: .\speed_compare_all.ps1
#   それぞれの実験の設定は、各 speed_compare_*.ps1 の先頭で変える

# 何回も使うファイル名(それぞれの speed_compare_*.ps1 が保存する結果)
$epochs_csv  = "csv\speed_compare_epochs.csv"
$width_csv   = "csv\speed_compare_width.csv"
$depth_csv   = "csv\speed_compare_depth.csv"
$samples_csv = "csv\speed_compare_samples.csv"
$plt         = "speed_compare.plt"   # グラフの描き方(PNG を標準出力に書く)

# 実験する(結果の CSV と学習後の重みは、それぞれのスクリプトが csv フォルダに保存する)
.\speed_compare_epochs.ps1
.\speed_compare_width.ps1
.\speed_compare_depth.ps1
.\speed_compare_samples.ps1

# グラフを描いて、PNG に保存する
gnuplot -c $plt $epochs_csv  "エポック数"                        > speed_compare_epochs.png
gnuplot -c $plt $width_csv   "隠れ層の幅 (1層あたりのノード数)" > speed_compare_width.png
gnuplot -c $plt $depth_csv   "隠れ層の数"                        > speed_compare_depth.png
gnuplot -c $plt $samples_csv "教師データの数"                    > speed_compare_samples.png
