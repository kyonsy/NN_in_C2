# c_build.ps1 で作った実行ファイルを消す
#   使い方: .\c_clean.ps1

Remove-Item make_teach_data.exe, initial_random.exe, train_main.exe, infer_main.exe -ErrorAction SilentlyContinue

Write-Host "削除完了: make_teach_data.exe initial_random.exe train_main.exe infer_main.exe"