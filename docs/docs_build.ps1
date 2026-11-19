#Requires -Version 7
# docs/pinn_equations.tex を PDF にする
#   使い方: .\docs\docs_build.ps1   (プロジェクトのフォルダから)
#           .\docs_build.ps1        (docs フォルダの中から)
#   どちらから実行しても、このファイルがある docs フォルダの中でコンパイルする
#
# LaTeX(lualatex)が入っていなければ、先に MiKTeX を winget で入れて設定する。
#   1. MiKTeX をインストール (winget)
#   2. 足りないパッケージを自動で取ってくる設定にする
#   3. パッケージを最新にする
# 2回目からは lualatex が見つかるので、コンパイルだけする。
# 初回のコンパイルはフォントの一覧づくりとパッケージのダウンロードで数分かかる。

$tex = "pinn_equations.tex"

function Update-PathFromRegistry {
    # インストールで増えた PATH を、このターミナルにも読み込み直す
    $machine = [Environment]::GetEnvironmentVariable("Path", "Machine")
    $user    = [Environment]::GetEnvironmentVariable("Path", "User")
    $env:Path = "$machine;$user"
}

if (-not (Get-Command lualatex -ErrorAction SilentlyContinue)) {
    Write-Host "lualatex が見つからないので MiKTeX をインストールする"
    winget install --id MiKTeX.MiKTeX -e
    if ($LASTEXITCODE -ne 0) { Write-Host "MiKTeX のインストールに失敗した"; exit 1 }

    Update-PathFromRegistry

    # PATH にまだ無いときは、MiKTeX のよくある場所を探して足す
    if (-not (Get-Command lualatex -ErrorAction SilentlyContinue)) {
        $candidates = @(
            "$env:LOCALAPPDATA\Programs\MiKTeX\miktex\bin\x64"   # ユーザーごとのインストール
            "$env:ProgramFiles\MiKTeX\miktex\bin\x64"            # 全ユーザーのインストール
        )
        foreach ($dir in $candidates) {
            if (Test-Path "$dir\lualatex.exe") { $env:Path = "$dir;$env:Path"; break }
        }
    }
    if (-not (Get-Command lualatex -ErrorAction SilentlyContinue)) {
        Write-Host "インストールしたが lualatex が見つからない。ターミナルを開き直してもう一度実行すること"
        exit 1
    }

    # 足りないパッケージ(日本語用の luatexja など)を、聞かずに自動で入れる
    initexmf --set-config-value "[MPM]AutoInstall=1"

    # インストール直後はパッケージが古いことがあるので最新にする
    miktex packages update-package-database
    miktex packages update
}

# docs フォルダ(このファイルがあるフォルダ)の中でコンパイルする(.aux や .log もそこにできる)
Push-Location $PSScriptRoot
try {
    lualatex -interaction=nonstopmode -halt-on-error $tex
    if ($LASTEXITCODE -ne 0) { Write-Host "コンパイルに失敗した。docs\pinn_equations.log を見ること"; exit 1 }
    Write-Host "完成: docs\pinn_equations.pdf"
}
finally {
    Pop-Location
}
