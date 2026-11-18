# train_main.c の PyTorch 版
# 使い方: python train_main.py 学習データ.csv 初期重み.csv [名前=値 ...] > 学習後の重み.csv
#   例:   python train_main.py teach_data.csv initial_weight.csv N_T=1000 EPOCHS=14000 > trained_weight.csv
#   学習後の重みは標準出力に、途中経過(損失)は標準エラー出力に書き出す
#   名前=値 で上書きできる設定は N_HIDDEN, N_WIDTH, N_T, EPOCHS, LR (既定値は nn_torch.py の先頭)
import sys

import torch
from torch import Tensor

from nn_torch import EPOCHS, LR, N_DISPLAY, N_T, NN, load_dataset


def mse_loss(n: int, Y: Tensor, T: Tensor) -> Tensor:
    # 平均二乗誤差 L = (1/n) Σ_s ||Y[s]-T[s]||^2
    L: Tensor = ((Y - T) ** 2).sum() / n
    return L


def train(net: NN, X: Tensor, T: Tensor, epochs: int, lr: float) -> None:
    # フルバッチの勾配降下法(N_T個のサンプルを毎回まとめて流す)
    #   torch.optim.SGD は、momentum などを指定しなければ p -= lr * ∂L/∂p をするだけ。
    #   毎回全サンプルで勾配を計算しているので、確率的ではない普通の勾配降下になる
    opt: torch.optim.SGD = torch.optim.SGD(net.parameters(), lr=lr)

    ep: int
    for ep in range(1, epochs + 1):
        # 順伝播 (nn_forward)
        Y: Tensor = net(X)

        # 損失
        L: Tensor = mse_loss(N_T, Y, T)

        # 逆伝播 (nn_backward)
        #   L.backward() は各パラメータ p の勾配 ∂L/∂p を p.grad に「足し込む」ので、
        #   前のエポックの勾配が残らないように先に 0 にしておく
        opt.zero_grad()
        L.backward()

        # 更新 (nn_step): p -= lr * ∂L/∂p
        opt.step()

        if ep % N_DISPLAY == 0:
            loss: float = L.item()  # 要素1個の Tensor から float を取り出す
            print(f"epoch {ep} loss {loss:e}", file=sys.stderr)


def main() -> None:
    data_path: str = sys.argv[1]
    init_path: str = sys.argv[2]

    X: Tensor
    T: Tensor
    X, T = load_dataset(data_path)

    net: NN = NN()
    net.load(init_path)

    train(net, X, T, EPOCHS, LR)

    net.print_weights()


if __name__ == "__main__":
    main()
