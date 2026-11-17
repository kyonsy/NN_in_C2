# 全プログラムをビルドする
#   使い方: .\build.ps1

$CFLAGS = "-Wall", "-std=c23", "-pedantic","-O2"

gcc $CFLAGS make_teach_data.c                    -o make_teach_data.exe; if (-not $?) { exit 1 }
gcc $CFLAGS initial_random.c nn.c matrix.c       -o initial_random.exe;  if (-not $?) { exit 1 }
gcc $CFLAGS train_main.c nn.c matrix.c dataset.c -o train_main.exe;      if (-not $?) { exit 1 }
gcc $CFLAGS infer_main.c nn.c matrix.c dataset.c -o infer_main.exe;      if (-not $?) { exit 1 }

Write-Host "ビルド完了: make_teach_data.exe initial_random.exe train_main.exe infer_main.exe"
