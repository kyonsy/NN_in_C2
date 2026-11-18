# 教師データと推論結果のグラフを表示する
#   使い方: .\plot.ps1   (先に .\c_run.ps1 などで teach_data.csv と c_infer_output.csv / py_infer_output.csv を作っておく)
#   -persist : gnuplot が終わってもグラフのウィンドウを開いたままにする

gnuplot -persist plot.plt