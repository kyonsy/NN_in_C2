#Requires -Version 7
# C 版と Python 版の学習時間を、深さ(隠れ層の数)を変えながら比べる
#   幅 16、教師データ 1000 個、エポック数 2000 に固定し、
#   N_HIDDEN を $depth_list の値に変えながら、C 版と Python 版で同じ学習をして時間を測る
#   使い方: .\speed_compare_depth.ps1 > speed_compare_depth.csv
#
# 結果は CSV で標準出力に書き出す。1行 = N_HIDDEN,C の秒数,Python の秒数,Python/C
#   (ほかの CSV と同じく見出しの行はなし。ビルドのメッセージや学習中の損失は画面に出るだけで、
#    標準出力には入らない)
#
# 教師データと初期の重みは C 版で作り、C 版と Python 版の両方で同じものを使う。
# Python 版の時間には、python の起動と torch の import にかかる時間(数秒)も含まれる。

$fixed = @(
    "N_WIDTH=16"    # 隠れ層1層あたりのノードの数
    "N_T=1000"      # 教師データの数
    "EPOCHS=2000"   # 学習の反復回数
)

$depth_list = @(2, 4, 8, 16, 32)   # 試す深さ(2以上)

# 何回も使うファイル名
$teach_data  = "teach_data.csv"       # 教師データ
$init_weight = "initial_weight.csv"   # 初期の重み

# 最後に必ず実行ファイルを消す
# (途中でエラーの exit や Ctrl+C で止まったときも、finally の中は実行される)
try {
    # 教師データは深さによらないので、最初に1回だけ作る
    .\c_build.ps1 @fixed
    if (-not $?) { exit 1 }
    .\make_teach_data.exe > $teach_data

    foreach ($depth in $depth_list) {
        $settings = $fixed + "N_HIDDEN=$depth"

        # C 版: 設定はコンパイル時に決まるので、ビルドし直す
        .\c_build.ps1 @settings
        if (-not $?) { exit 1 }

        # 深さが変わると重みの数が変わるので、初期の重みは毎回作り直す
        .\initial_random.exe > $init_weight

        # C 版で学習する
        $c_sec = (Measure-Command { .\train_main.exe $teach_data $init_weight > c_trained_weight.csv }).TotalSeconds

        # Python 版: 同じ設定をコマンドライン引数の 名前=値 で渡して学習する
        $py_sec = (Measure-Command { python train_main.py $teach_data $init_weight @settings > py_trained_weight.csv }).TotalSeconds

        # 1行分を標準出力に書き出す(N_HIDDEN,C の秒数,Python の秒数,Python/C)
        "{0},{1:F2},{2:F2},{3:F2}" -f $depth, $c_sec, $py_sec, ($py_sec / $c_sec)
    }
}
finally {
    # ビルドした実行ファイルを消す
    .\c_clean.ps1
}
