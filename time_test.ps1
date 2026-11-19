#Requires -Version 7
# Measure-Command と Stopwatch のどちらが時間を正確に測れるかを比べる
#   決まった時間だけ Start-Sleep し、それぞれの方法で測った時間と寝かせた時間の差(誤差)を求める
#   寝かせる時間ごとに 10 回ずつ測って誤差の2乗平均を出し、CSV とグラフに保存する
#   使い方: .\time_test.ps1
#   結果  : csv\time_test.csv (1行 = 寝かせた時間 [ms],Measure-Command の誤差の2乗平均,Stopwatch の誤差の2乗平均)
#           time_test.png    (time_test.plt で描いたグラフ)

$sleep_times = @(10, 50, 100, 500, 1000)  # 寝かせる時間 [ms]
$n_trials    = 10                         # 1つの寝かせる時間あたりの測定回数

$csv = "csv\time_test.csv"   # 結果の CSV
$plt = "time_test.plt"       # グラフの描き方(PNG を標準出力に書く)
$png = "time_test.png"       # グラフの画像

# CSV ファイルはすべて csv フォルダに置く(なければ作る)
New-Item -ItemType Directory -Force csv | Out-Null

$lines = @()   # CSV の行をためておく
foreach ($sleep_time in $sleep_times) {
    $sum_sq_mc = 0.0   # Measure-Command の誤差の2乗の合計
    $sum_sq_sw = 0.0   # Stopwatch の誤差の2乗の合計

    for ($i = 0; $i -lt $n_trials; $i++) {
        # Measure-Command で測る
        $mc = (Measure-Command {
            Start-Sleep -Milliseconds $sleep_time
        }).TotalMilliseconds

        # Stopwatch で測る
        $sw = [System.Diagnostics.Stopwatch]::StartNew()
        Start-Sleep -Milliseconds $sleep_time
        $sw.Stop()
        $sw_ms = $sw.Elapsed.TotalMilliseconds

        $sum_sq_mc += ($mc    - $sleep_time) * ($mc    - $sleep_time)
        $sum_sq_sw += ($sw_ms - $sleep_time) * ($sw_ms - $sleep_time)
    }

    $mse_mc = $sum_sq_mc / $n_trials   # 誤差の2乗平均 [ms^2]
    $mse_sw = $sum_sq_sw / $n_trials

    Write-Host ("{0,5} ms  Measure-Command: {1,10:F4} ms^2   Stopwatch: {2,10:F4} ms^2" -f $sleep_time, $mse_mc, $mse_sw)
    $lines += "{0},{1},{2}" -f $sleep_time, $mse_mc, $mse_sw
}

# 結果を CSV に保存する
$lines | Set-Content -Encoding utf8 $csv

# グラフを描いて、PNG に保存する
gnuplot -c $plt $csv > $png
Write-Host "グラフを $png に保存しました"
