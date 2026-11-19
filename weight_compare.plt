# C 版と Python 版の学習後の重みの平均二乗誤差のグラフを PNG で標準出力に書き出す
#   使い方: gnuplot -c weight_compare.plt 結果.csv 横軸の見出し > 画像.png   (weight_compare_all.ps1 から呼ぶ)
#   結果.csv : 1行 = 変えた設定の値,重みの数,平均二乗誤差  (weight_compare_*.ps1 の出力)

set encoding utf8
set terminal pngcairo size 800,500
set datafile separator ","

file   = ARG1
xlabel = (ARGC >= 2) ? ARG2 : ""

set title  "C 版と Python 版の学習後の重みの差"
set xlabel xlabel
set ylabel "平均二乗誤差"
set logscale y 10             # 誤差は桁で変わるので対数目盛(0 の点は描かれない)
set format y "10^{%L}"
set autoscale xfix
set offsets graph 0.05, graph 0.05, 0, 0
set grid xtics ytics mytics
set key off

plot file using 1:3 with linespoints linewidth 2 pointtype 7 pointsize 1.2
