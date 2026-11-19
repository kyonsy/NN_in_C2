#Requires -Version 7
# C 版と Python 版の学習後の重みの CSV を読んで、平均二乗誤差を求める関数
#   使い方: ほかのスクリプトの先頭で  . .\weight_mse.ps1  と読み込んでから
#           $r = Get-WeightMse $c_weight $py_weight
#           $r.N   → 重みの数
#           $r.Mse → 平均二乗誤差 = (1/n) Σ_i (c_i - py_i)^2
#   (設定によって重みの数が変わるので、合計ではなく平均にする)

# 重みの CSV(1行にカンマ区切りで数値が並ぶ)を読んで、全部の数値を1列の double の配列にする
function Read-WeightCsv([string]$path) {
    $inv = [System.Globalization.CultureInfo]::InvariantCulture
    $values = [System.Collections.Generic.List[double]]::new()
    foreach ($line in [System.IO.File]::ReadLines((Resolve-Path $path))) {
        foreach ($s in $line.Split(',')) {
            $s = $s.Trim()
            if ($s -ne "") { $values.Add([double]::Parse($s, $inv)) }
        }
    }
    return , $values.ToArray()
}

function Get-WeightMse([string]$c_weight, [string]$py_weight) {
    $c  = Read-WeightCsv $c_weight
    $py = Read-WeightCsv $py_weight
    $n  = $c.Length

    $sum_sq = 0.0   # 二乗誤差の合計
    for ($i = 0; $i -lt $n; $i++) {
        $d = $c[$i] - $py[$i]
        $sum_sq += $d * $d
    }

    return [PSCustomObject]@{ N = $n; Mse = $sum_sq / $n }
}

# speed_compare_*.ps1 が残した学習後の重みを、1つの設定について比べて、CSV の1行を返す
#   $value    : 変えた設定の値(CSV の1列目に書く)
#   $settings : c_build.ps1 に渡したのと同じ "名前=値" の配列(ファイル名の印を作るのに使う)
#   重みのファイルがなければ警告を出して $null を返す
function Get-WeightCompareLine($value, [string[]]$settings) {
    $tag       = Get-RunTag $settings
    $c_weight  = "csv\trained_weight_${tag}_c.csv"  # C 版で学習した後の重み
    $py_weight = "csv\trained_weight_${tag}_py.csv" # Python 版で学習した後の重み

    if (-not (Test-Path $c_weight) -or -not (Test-Path $py_weight)) {
        Write-Warning "$c_weight か $py_weight がないので飛ばします(先に speed_compare_*.ps1 を実行してください)"
        return $null
    }

    $r = Get-WeightMse $c_weight $py_weight
    return [string]::Format([System.Globalization.CultureInfo]::InvariantCulture,
        "{0},{1},{2:E3}", $value, $r.N, $r.Mse)
}
