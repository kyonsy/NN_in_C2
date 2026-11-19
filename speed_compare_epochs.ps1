#Requires -Version 7
# C 版と Python 版の学習時間を、エポック数を変えながら比べる
#   幅 16、深さ 16、教師データ 1000 個に固定し、
#   EPOCHS を $epochs_list の値に変えながら、C 版と Python 版で同じ学習をして時間を測る
#   使い方: .\speed_compare_epochs.ps1
#
# 結果は csv\speed_compare_epochs.csv に保存する(1つの設定が終わるたびに1行ずつ書き足す)。1行 = EPOCHS,C のミリ秒数,Python のミリ秒数,Python/C
#   (ほかの CSV と同じく見出しの行はなし)
#   学習後の重みも、設定ごとに csv フォルダに残す(下の $c_weight と $py_weight)
#
# 教師データと初期の重みは C 版で作り、C 版と Python 版の両方で同じものを使う。
# Python 版の時間には、python の起動と torch の import にかかる時間(数秒)も含まれる。

$fixed = @(
    "N_WIDTH=16"    # 隠れ層1層あたりのノードの数
    "N_HIDDEN=16"   # 隠れ層の数
    "N_T=1000"      # 教師データの数
)

$epochs_list = @(1000, 2000, 4000, 8000, 10000)   # 試すエポック数

# 何回も使うファイル名
$result_csv  = "csv\speed_compare_epochs.csv"   # 結果(学習時間)
$teach_data  = "csv\teach_data.csv"       # 教師データ

# 初期の重みと学習後の重みは、名前の後ろに 幅_深さ_教師データの数_エポック数 を付けて、上書きしないようにする
. .\run_tag.ps1

# 最後に必ず実行ファイルを消す
# (途中でエラーの exit や Ctrl+C で止まったときも、finally の中は実行される)
try {
    # 前の結果を消して、空のファイルから書き始める
    New-Item -ItemType Directory -Force csv | Out-Null
    Set-Content $result_csv $null

    # 教師データは EPOCHS によらないので、最初に1回だけ作る
    .\c_build.ps1 @fixed
    if (-not $?) { exit 1 }
    .\make_teach_data.exe > $teach_data

    foreach ($epochs in $epochs_list) {
        $settings = $fixed + "EPOCHS=$epochs"

        $tag         = Get-RunTag $settings
        $init_weight = "csv\initial_weight_$tag.csv"      # 初期の重み
        $c_weight    = "csv\trained_weight_${tag}_c.csv"  # C 版で学習した後の重み
        $py_weight   = "csv\trained_weight_${tag}_py.csv" # Python 版で学習した後の重み

        # C 版: 設定はコンパイル時に決まるので、ビルドし直してから学習する
        .\c_build.ps1 @settings
        if (-not $?) { exit 1 }

        # 初期の重みは EPOCHS によらず同じ(乱数の種が同じ)だが、名前ごとに残すため毎回書き出す
        .\initial_random.exe > $init_weight

        $c_ms = (Measure-Command { .\train_main.exe $teach_data $init_weight > $c_weight }).TotalMilliseconds

        # Python 版: 同じ設定をコマンドライン引数の 名前=値 で渡して学習する
        $py_ms = (Measure-Command { python train_main.py $teach_data $init_weight @settings > $py_weight }).TotalMilliseconds

        # 1行分を CSV に書き足す(EPOCHS,C のミリ秒数,Python のミリ秒数,Python/C)。途中で止まっても、終わった設定の分は残る
        $line = "{0},{1:F0},{2:F0},{3:F2}" -f $epochs, $c_ms, $py_ms, ($py_ms / $c_ms)
        Add-Content $result_csv $line
    }
}
finally {
    # ビルドした実行ファイルを消す
    .\c_clean.ps1
}
