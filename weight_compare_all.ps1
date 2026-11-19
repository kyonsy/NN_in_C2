#Requires -Version 7
# C 版と Python 版の学習後の重みの比較を全部まとめて実行し、結果の CSV とグラフの画像を保存する
#   使い方: .\weight_compare_all.ps1
#   学習はしない。先に .\speed_compare_all.ps1 を実行して、学習後の重みを csv フォルダに残しておくこと

# 何回も使うファイル名
$epochs_csv  = "csv\weight_compare_epochs.csv"
$width_csv   = "csv\weight_compare_width.csv"
$depth_csv   = "csv\weight_compare_depth.csv"
$samples_csv = "csv\weight_compare_samples.csv"
$plt         = "weight_compare.plt"   # グラフの描き方(PNG を標準出力に書く)

# 重みを比べて、結果を CSV に保存する
.\weight_compare_epochs.ps1  > $epochs_csv
.\weight_compare_width.ps1   > $width_csv
.\weight_compare_depth.ps1   > $depth_csv
.\weight_compare_samples.ps1 > $samples_csv

# グラフを描いて、PNG に保存する
gnuplot -c $plt $epochs_csv  "エポック数"                        > weight_compare_epochs.png
gnuplot -c $plt $width_csv   "隠れ層の幅 (1層あたりのノード数)" > weight_compare_width.png
gnuplot -c $plt $depth_csv   "隠れ層の数"                        > weight_compare_depth.png
gnuplot -c $plt $samples_csv "教師データの数"                    > weight_compare_samples.png
