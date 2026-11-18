#Requires -Version 7
# エポック数を変えながら学習時間を測る
#   幅 16、深さ 16、教師データ 1000 個に固定し、
#   EPOCHS を 1000 から 2 倍ずつ増やして、学習時間が 60 秒以上になったら止める
#   使い方: .\c_epoch_time.ps1

$fixed = @(
    "N_WIDTH=16"    # 隠れ層1層あたりのノードの数
    "N_HIDDEN=16"   # 隠れ層の数
    "N_T=1000"      # 教師データの数
)

$epochs  = 2000     # 最初のエポック数
$limit   = 60       # この秒数以上になったら止める
$results = @()      # 結果(エポック数と学習時間)をためておく

# 何回も使うファイル名
$teach_data  = "teach_data.csv"       # 教師データ
$init_weight = "initial_weight.csv"   # 初期の重み

# 最後に必ず実行ファイルを消す
# (途中でエラーの exit や Ctrl+C で止まったときも、finally の中は実行される)
try {
    while ($true) {
        # この設定でビルドする(EPOCHS は -D で渡すので、変えるたびにビルドし直す)
        .\c_build.ps1 @fixed "EPOCHS=$epochs"

        # 教師データと初期の重みを作る
        .\make_teach_data.exe > $teach_data
        .\initial_random.exe > $init_weight

        # 学習して時間を測る
        $time = Measure-Command { .\train_main.exe $teach_data $init_weight > c_trained_weight.csv }
        $sec  = $time.TotalSeconds
        Write-Host ("EPOCHS={0}  学習時間: {1:F2} 秒" -f $epochs, $sec)

        $results += [PSCustomObject]@{ EPOCHS = $epochs; 秒 = [math]::Round($sec, 2) }

        if ($sec -ge $limit) { break }
        $epochs += 2000
    }

    # まとめて表示する
    $results | Format-Table -AutoSize
}
finally {
    # ビルドした実行ファイルを消す
    .\c_clean.ps1
}
