# infer_main.c の PyTorch 版
# 使い方: python infer_main.py 重み.csv 教師データ.csv [名前=値 ...] > 出力.csv
#   例:   python infer_main.py trained_weight.csv teach_data.csv N_T=1000 > infer_output.csv
#   教師データの x の列だけを入力にして推論し、1行に x,y を並べて標準出力に書き出す
#   名前=値 は train_main.py と同じ(N_HIDDEN, N_WIDTH, N_T は学習のときと同じ値にすること)
import sys

import torch
from torch import Tensor

from nn_torch import N_T, NN, load_dataset


def main() -> None:
    weights_path: str = sys.argv[1]
    data_path: str = sys.argv[2]

    net: NN = NN()
    net.load(weights_path)

    X: Tensor
    X, _ = load_dataset(data_path)

    # 推論だけで逆伝播はしないので、自動微分の記録を止めておく
    with torch.no_grad():
        Y: Tensor = net(X)

    # 1行に x_1,...,x_D_IN,y_1,...,y_D_OUT を並べて書き出す
    s: int
    for s in range(N_T):
        x_strs: list[str] = [f"{float(x):.17g}" for x in X[s]]
        y_strs: list[str] = [f"{float(y):.17g}" for y in Y[s]]
        print(",".join(x_strs + y_strs))


if __name__ == "__main__":
    main()
