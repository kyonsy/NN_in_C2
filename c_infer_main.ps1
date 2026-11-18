# > で保存するファイルを ASCII にする(PowerShell 5.1 の > は UTF-16 になり、C の fscanf で読めないため)
$PSDefaultParameterValues['Out-File:Encoding'] = 'ascii'

.\infer_main.exe c_trained_weight.csv teach_data.csv > c_infer_output.csv
