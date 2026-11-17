# 教師データと推論結果を重ねて表示する
#   使い方: gnuplot -persist plot.plt   (.\plot.ps1 から呼ぶ)
#   teach_data.csv   : 1行 = x,t  (正解)
#   infer_output.csv : 1行 = x,y  (NN の出力)

set encoding utf8             # このファイルは UTF-8(日本語のタイトルを正しく表示するため)
set datafile separator ","    # CSV なので区切りはカンマ

set title  "教師データと推論結果"
set xlabel "x"
set ylabel "y"
set grid

plot "teach_data.csv"   using 1:2 with points pointtype 7 title "教師データ t", \
     "infer_output.csv" using 1:2 with lines  linewidth 2  title "推論結果 y"