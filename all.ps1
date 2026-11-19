#Requires -Version 7
# 速度比較と重みの比較を続けて実行し、それぞれにかかった時間を最後に表示する
#   使い方: .\all.ps1
#   1. speed_compare_all.ps1  (学習して、学習時間と学習後の重みを保存し、グラフを描く)
#   2. weight_compare_all.ps1 (保存された学習後の重みを比べて、グラフを描く)
#   weight_compare_all.ps1 は 1 で保存された重みを読むので、この順に実行する

# 途中の表示(ビルドのメッセージや学習中の損失)も画面に出したいので、
# Measure-Command ではなく Stopwatch で時間を測る(Measure-Command は { } の中の出力を捨ててしまう)
$sw = [System.Diagnostics.Stopwatch]::StartNew()
.\speed_compare_all.ps1
$speed_ms = $sw.Elapsed.TotalMilliseconds

$sw.Restart()
.\weight_compare_all.ps1
$weight_ms = $sw.Elapsed.TotalMilliseconds

# それぞれにかかった時間を表示する
Write-Host ""
Write-Host ("speed_compare_all.ps1  : {0:F0} ミリ秒" -f $speed_ms)
Write-Host ("weight_compare_all.ps1 : {0:F0} ミリ秒" -f $weight_ms)
Write-Host ("合計                   : {0:F0} ミリ秒" -f ($speed_ms + $weight_ms))
