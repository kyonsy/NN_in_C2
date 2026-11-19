# nn.c / dataset.c / config.h の PyTorch 版
#
#   入力 X ──W_in──→ H[0] ──W_hid[0]──→ H[1] ── … ──→ H[N_HIDDEN-1] ──W_out──→ 出力 Y
#
# 重みファイルは C 版(nn_print / nn_load)と同じ形式なので、そのまま読み書きできる。
import sys
from typing import TextIO

import numpy as np
import torch
from torch import Tensor

# ---- 設定(既定値は config.h と同じ) ----
# N_HIDDEN, N_WIDTH, N_T, EPOCHS, LR は、コマンドライン引数の「名前=値」で上書きできる
#   例: python train_main.py teach_data.csv initial_weight.csv N_WIDTH=32 N_HIDDEN=4
# (C 版の c_build.ps1 の 名前=値 と同じ。渡さなかった値は下の既定値になる)
N_HIDDEN: int = 16  # 隠れ層の数(2以上)
N_WIDTH: int = 16  # 隠れ層1層あたりのノードの数
D_IN: int = 1  # 入力の次元  (教師データの形で決まるので上書きしない)
D_OUT: int = 1  # 出力の次元  (同上)
N_T: int = 41  # 教師データの数(サンプル数)
EPOCHS: int = 10000  # 学習の反復回数
LR: float = 0.05  # 学習率
N_DISPLAY: int = 1000  # 何エポックごとに損失を表示するか


def override_config() -> None:
    # sys.argv の中の「名前=値」で設定を上書きし、sys.argv から取り除く
    # (各スクリプトは、残ったファイル名の引数だけを sys.argv[1], sys.argv[2] で読めばよい)
    global N_HIDDEN, N_WIDTH, N_T, EPOCHS, LR

    rest: list[str] = [sys.argv[0]]
    arg: str
    name: str
    eq: str
    value: str
    for arg in sys.argv[1:]:
        name, eq, value = arg.partition("=")  # "N_T=1000" → "N_T", "=", "1000"
        if eq == "":
            rest.append(arg)  # = がなければファイル名なので残す
        elif name == "N_HIDDEN":
            N_HIDDEN = int(value)
        elif name == "N_WIDTH":
            N_WIDTH = int(value)
        elif name == "N_T":
            N_T = int(value)
        elif name == "EPOCHS":
            EPOCHS = int(value)
        elif name == "LR":
            LR = float(value)
        else:
            sys.exit(f"上書きできない設定です: {arg}")
    sys.argv = rest


# import された時点で上書きしておく
# (train_main.py などが from nn_torch import N_T とする前に、値が決まっている必要があるため)
override_config()

# PyTorch の既定は float(単精度)なので、C 版と同じ double(倍精度)に変えておく
torch.set_default_dtype(torch.float64)

# テンソルを置く場所を CPU にする(C 版と同じく CPU で計算する)
torch.set_default_device("cpu")


class NN(torch.nn.Module):
    # torch.nn.Linear(din, dout) は、重み W(dout×din) とバイアス b(dout) を持つ層で、
    # 呼び出すと H @ W^T + b を計算する(C 版の layer_forward の tanh をかける前まで)
    W_in: torch.nn.Linear  # 入力層 → 隠れ層0
    W_hid: torch.nn.ModuleList  # 隠れ層l → 隠れ層l+1 (N_HIDDEN-1 個)
    W_out: torch.nn.Linear  # 最後の隠れ層 → 出力層

    def __init__(self) -> None:
        super().__init__()
        self.W_in = torch.nn.Linear(D_IN, N_WIDTH)
        self.W_hid = torch.nn.ModuleList()
        for _ in range(N_HIDDEN - 1):
            self.W_hid.append(torch.nn.Linear(N_WIDTH, N_WIDTH))
        self.W_out = torch.nn.Linear(N_WIDTH, D_OUT)

    def forward(self, X: Tensor) -> Tensor:
        # nn_forward (逆伝播は PyTorch が自動で計算するので、途中結果を取っておく必要はない)
        H: Tensor = torch.tanh(self.W_in(X))  # 入力層 → 隠れ層0
        W: torch.nn.Module
        for W in self.W_hid:
            H = torch.tanh(W(H))  # 隠れ層l → 隠れ層l+1
        Y: Tensor = self.W_out(H)  # 最後の隠れ層 → 出力層 (tanh なし)
        return Y

    def load(self, path: str) -> None:
        # nn_load: ファイルの値を先頭から順に W_in, b_in, W_hid[0], b_hid[0], …, W_out, b_out に入れる
        #
        # 1. ファイルの数値を全部、1列に並べて読む
        #    (行ごとに値の数が違うので、カンマも改行も区切りとして扱う)
        f: TextIO
        with open(path) as f:
            text: str = f.read().replace(",", " ")
        words: list[str] = text.split()
        numbers: list[float] = [float(w) for w in words]
        values: Tensor = torch.tensor(numbers)

        # 値の数がネットワークの形(N_HIDDEN, N_WIDTH)と合っているか確かめる
        n_params: int = sum(p.numel() for p in self.parameters())
        if values.numel() != n_params:
            sys.exit(f"{path}: 値が {values.numel()} 個あるが、"
                     f"N_HIDDEN={N_HIDDEN}, N_WIDTH={N_WIDTH} のネットワークは {n_params} 個")

        # 2. 1列の値を、self.parameters() の順に各パラメータの形に切り分けて入れる
        #    self.parameters() は W_in, b_in, W_hid[0], b_hid[0], …, W_out, b_out の順に返すので、
        #    ちょうどファイルの並び順と同じになる
        torch.nn.utils.vector_to_parameters(values, self.parameters())

    def print_weights(self) -> None:
        # nn_print: 入力層側から順に、各層の W と b を標準出力に書き出す
        layers: list[torch.nn.Module] = [self.W_in, *self.W_hid, self.W_out]
        layer: torch.nn.Module
        W: Tensor
        b: Tensor
        for layer in layers:
            W, b = layer.parameters()  # Linear のパラメータは W, b の順に2つ
            layer_print(W, b)


def layer_print(W: Tensor, b: Tensor) -> None:
    # layer_print: W を dout 行、b を 1 行で、カンマ区切りで書き出す
    # :.17g は C の %.17g と同じ(読み戻したときに元の値と完全に一致する桁数)
    W_rows: list[list[float]] = W.tolist()
    b_row: list[float] = b.tolist()

    row: list[float]
    for row in W_rows:
        print(",".join(f"{v:.17g}" for v in row))
    print(",".join(f"{v:.17g}" for v in b_row))


def load_dataset(path: str) -> tuple[Tensor, Tensor]:
    # dataset_load: 1行に x_1,...,x_D_IN,t_1,...,t_D_OUT が並んだファイルから N_T 行読み込む
    data: Tensor = torch.tensor(np.loadtxt(path, delimiter=",", ndmin=2, max_rows=N_T))
    if data.shape[0] != N_T:
        sys.exit(f"{path}: {N_T} 行あるはずが {data.shape[0]} 行しかない")
    X: Tensor = data[:, :D_IN]  # 前の D_IN 列が入力 x
    T: Tensor = data[:, D_IN:]  # 残りの D_OUT 列が教師 t
    return X, T
