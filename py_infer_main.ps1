# Python 版(infer_main.py)で推論する
#   使い方: .\py_infer_main.ps1 [名前=値 ...]
#   例:     .\py_infer_main.ps1 N_T=1000
#   N_HIDDEN, N_WIDTH, N_T は、.\py_train_main.ps1 で学習したときと同じ値を渡すこと

# > で保存するファイルを ASCII にする(PowerShell 5.1 の > は UTF-16 になり、C の fscanf で読めないため)
$PSDefaultParameterValues['Out-File:Encoding'] = 'ascii'

python infer_main.py py_trained_weight.csv teach_data.csv @args > py_infer_output.csv
