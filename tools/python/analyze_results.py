#!/usr/bin/env python3

from pathlib import Path
import numpy as np


def load_reconstruction(filename: str) -> np.ndarray:
    return np.loadtxt(filename, delimiter=",")


def main() -> None:
    filename = Path("reconstruction.csv")

    image = load_reconstruction(str(filename))

    print(f"Image shape: {image.shape}")
    print(f"Minimum:     {image.min():.3f}")
    print(f"Maximum:     {image.max():.3f}")
    print(f"Mean:        {image.mean():.3f}")
    print(f"Total:       {image.sum():.3f}")


if __name__ == "__main__":
    main()