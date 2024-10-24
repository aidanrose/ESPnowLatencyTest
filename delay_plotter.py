from matplotlib import pyplot as plt
import csv

import argparse
from pathlib import Path
import numpy as np

def load_csv(path: Path) -> list[tuple[float, int | None]]:
    data = []
    with open(path) as file:
        reader = csv.reader(file)
        for row in reader:
            timestamp = float(row[0])
            delay = float("nan") if row[1] == "No Data" else (float(row[1])/1000)
            data.append((timestamp, delay))
    return data

def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("path", type=str)
    args = parser.parse_args()

    path = Path(args.path)

    data = np.array(load_csv(path), dtype=float)
    data = np.delete(data, data[:, 1] > 5.0, axis=0)

    fig, axes = plt.subplots(1, 2)
    axes[0].hist(data[:, 1], bins=50)
    axes[1].plot(data[:, 0], data[:, 1])
    plt.show()


if __name__ == "__main__":
    main()