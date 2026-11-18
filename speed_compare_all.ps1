#Requires -Version 7
# C 版と Python 版の速度比較を全部まとめて実行し、結果の CSV とグラフの画像を保存する
#   使い方: .\speed_compare_all.ps1   (全部で十数分かかる)
#   それぞれの実験の設定は、各 speed_compare_*.ps1 の先頭で変える

# 何回も使うファイル名
$epochs_csv = "speed_compare_epochs.csv"
$width_csv  = "speed_compare_width.csv"
$depth_csv  = "speed_compare_depth.csv"
$plt        = "speed_compare.plt"   # グラフの描き方(PNG を標準出力に書く)

# 実験して、結果を CSV に保存する
.\speed_compare_epochs.ps1 > $epochs_csv
.\speed_compare_width.ps1  > $width_csv
.\speed_compare_depth.ps1  > $depth_csv

# グラフを描いて、PNG に保存する
gnuplot -c $plt $epochs_csv "エポック数"                        > speed_compare_epochs.png
gnuplot -c $plt $width_csv  "隠れ層の幅 (1層あたりのノード数)" > speed_compare_width.png
gnuplot -c $plt $depth_csv  "隠れ層の数"                        > speed_compare_depth.png
