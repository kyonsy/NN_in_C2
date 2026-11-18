#Requires -Version 7
# C 版の全プログラムをビルドする
#   使い方: .\c_build.ps1                         (config.h の既定値でビルド)
#           .\c_build.ps1 N_WIDTH=32 N_HIDDEN=4   (config.h の値を上書きしてビルド)
#
# 引数の「名前=値」は、それぞれ gcc の -D名前=値 として渡す。
# (-DLR=0.01 と直接書くと PowerShell が小数点で分けてしまうので、-D はここで付ける)

$CFLAGS = "-Wall", "-std=c23", "-pedantic", "-O2"
foreach ($a in $args) { $CFLAGS += "-D$a" }

gcc $CFLAGS make_teach_data.c                    -o make_teach_data.exe; if (-not $?) { exit 1 }
gcc $CFLAGS initial_random.c nn.c matrix.c       -o initial_random.exe;  if (-not $?) { exit 1 }
gcc $CFLAGS train_main.c nn.c matrix.c dataset.c -o train_main.exe;      if (-not $?) { exit 1 }
gcc $CFLAGS infer_main.c nn.c matrix.c dataset.c -o infer_main.exe;      if (-not $?) { exit 1 }

Write-Host "ビルド完了: make_teach_data.exe initial_random.exe train_main.exe infer_main.exe"
Write-Host "上書きした設定: $args"
