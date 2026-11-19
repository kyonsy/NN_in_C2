#Requires -Version 7
# 学習のたびにできる CSV(初期の重み・学習後の重み・推論の出力)の名前に付ける印を作る
#   使い方: ほかのスクリプトの先頭で  . .\run_tag.ps1  と読み込んでから
#           $tag = Get-RunTag $settings      → 例: "16_8_1000_10000"
#           "csv\trained_weight_${tag}_c.csv" のように使う
#
# 印は 幅_深さ_教師データの数_エポック数 (N_WIDTH_N_HIDDEN_N_T_EPOCHS) の順に並べる。
# $settings(c_build.ps1 に渡す "名前=値" の配列)に書いていない値は、config.h の既定値を使う。

function Get-RunTag([string[]]$settings) {
    $names = @("N_WIDTH", "N_HIDDEN", "N_T", "EPOCHS")

    # まず config.h の既定値を読む(#define 名前 値)
    $values = @{}
    $config = Get-Content (Join-Path $PSScriptRoot "config.h")
    foreach ($name in $names) {
        foreach ($line in $config) {
            if ($line -match "^\s*#define\s+$name\s+(\S+)") { $values[$name] = $Matches[1]; break }
        }
    }

    # $settings に書いた値で上書きする
    foreach ($s in $settings) {
        $name, $value = $s -split "=", 2
        if ($names -contains $name) { $values[$name] = $value }
    }

    return ($names | ForEach-Object { $values[$_] }) -join "_"
}
