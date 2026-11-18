# C 版と Python 版の学習時間のグラフを PNG で標準出力に書き出す
# (speed_compare_epochs/width/depth.ps1 の結果で共通に使う)
#   使い方: gnuplot -c speed_compare.plt 結果.csv 横軸の見出し > 画像.png   (speed_compare_all.ps1 から呼ぶ)
#   例:     gnuplot -c speed_compare.plt speed_compare_width.csv "隠れ層の幅" > speed_compare_width.png
#   結果.csv : 1行 = 変えた設定の値,C の秒数,Python の秒数,Python/C  (speed_compare_*.ps1 の出力)
#   -c のあとに書いた引数は、このファイルの中で ARG1, ARG2 として使える(ARGC は引数の数)

set encoding utf8             # このファイルは UTF-8(日本語の見出しを正しく表示するため)
set terminal pngcairo size 800,500   # PNG で出力する(set output を書かないので標準出力に出る)
set datafile separator ","    # CSV なので区切りはカンマ

file   = ARG1                          # 結果の CSV ファイル
xlabel = (ARGC >= 2) ? ARG2 : ""       # 横軸の見出し(省略したら見出しなし)

set title  "学習時間の比較"
set xlabel xlabel
set ylabel "学習時間 [秒]"
set yrange [0:*]              # 縦軸は 0 から
set offsets graph 0.05, graph 0.05, 0, 0   # 左右に少し余白を空け、端の点と目盛りが枠に重ならないようにする
set grid
set key left top

plot file using 1:2 with linespoints linewidth 2 pointtype 7 pointsize 1.2 title "C", \
     file using 1:3 with linespoints linewidth 2 pointtype 5 pointsize 1.2 title "Python"
