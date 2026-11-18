# 教師データ作成から推論までの流れを実行する
#   使い方: .\c_run.ps1   (先に .\c_build.ps1 でビルドしておく)
#
# 流れ:
#   1. 教師データを作る    make_teach_data.exe                                → teach_data.csv
#   2. 初期の重みを作る    initial_random.exe                                 → initial_weight.csv
#   3. 学習する            train_main.exe teach_data.csv initial_weight.csv   → c_trained_weight.csv
#   4. 推論する            infer_main.exe c_trained_weight.csv teach_data.csv   → c_infer_output.csv

# > で保存するファイルを ASCII にする(PowerShell 5.1 の > は UTF-16 になり、C の fscanf で読めないため)
$PSDefaultParameterValues['Out-File:Encoding'] = 'ascii'


# ---- 1. 教師データを作る (1行 = x,t) ----
.\make_teach_data.exe > teach_data.csv

# ---- 2. 初期の重みを作る (config.h の SEED で乱数を初期化) ----
.\initial_random.exe > initial_weight.csv

# ---- 3. 学習する (損失の途中経過は標準エラー出力なので画面に出る) ----
.\train_main.exe teach_data.csv initial_weight.csv > c_trained_weight.csv

# ---- 4. 推論する (教師データの x を入力にする。出力は1行 = y) ----
.\infer_main.exe c_trained_weight.csv teach_data.csv > c_infer_output.csv

# ---- 5. グラフを描く
.\plot.ps1