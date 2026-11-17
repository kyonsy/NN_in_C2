# > で保存するファイルを ASCII にする(PowerShell 5.1 の > は UTF-16 になり、C の fscanf で読めないため)
$PSDefaultParameterValues['Out-File:Encoding'] = 'ascii'

.\train_main.exe .\teach_data.csv .\initial_weight.csv > trained_weight.csv
