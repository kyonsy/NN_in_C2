# Measure-Command と Stopwatch の誤差の2乗平均のグラフを PNG で標準出力に書き出す
#   使い方: gnuplot -c time_test.plt 結果.csv > 画像.png   (time_test.ps1 から呼ぶ)
#   結果.csv : 1行 = 寝かせた時間 [ms],Measure-Command の誤差の2乗平均,Stopwatch の誤差の2乗平均

set encoding utf8
set terminal pngcairo size 800,500
set datafile separator ","

file = ARG1

set title  "時間計測の誤差の比較 (各10回の誤差の2乗平均)"
set xlabel "寝かせた時間 [ms]"
set ylabel "誤差の2乗平均 [ms^2]"
set logscale x 10             # 寝かせた時間は 10〜1000 ms と幅が広いので対数目盛
set logscale y 10             # 2つの方法の差が大きくても両方見えるように対数目盛
set offsets graph 0.05, graph 0.05, 0, 0
set grid xtics ytics mxtics mytics
set key left top

plot file using 1:2 with linespoints linewidth 2 pointtype 7 pointsize 1.2 title "Measure-Command", \
     file using 1:3 with linespoints linewidth 2 pointtype 5 pointsize 1.2 title "Stopwatch"
