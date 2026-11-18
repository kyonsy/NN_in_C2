# Python 版(train_main.py)で学習する
#   使い方: .\py_train_main.ps1 [名前=値 ...]
#   例:     .\py_train_main.ps1 N_T=1000 EPOCHS=14000
#   名前=値 は train_main.py にそのまま渡す(上書きできるのは N_HIDDEN, N_WIDTH, N_T, EPOCHS, LR)
#   渡さなかった値は nn_torch.py の先頭の既定値(config.h と同じ)になる

# > で保存するファイルを ASCII にする(PowerShell 5.1 の > は UTF-16 になり、C の fscanf で読めないため)
$PSDefaultParameterValues['Out-File:Encoding'] = 'ascii'

python train_main.py teach_data.csv initial_weight.csv @args > py_trained_weight.csv
