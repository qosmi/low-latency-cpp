from pathlib import Path
import argparse
import matplotlib.pyplot as plt
import numpy as np


INPUT_FILE = Path("reconstruction.csv")
OUTPUT_FILE = Path("reconstruction.png")


def load_reconstruction(filename: str) -> np.ndarray:
    return np.loadtxt(
        filename,
        delimiter=","
    )


def print_statistics(image: np.ndarray) -> None:
    print(f"Image shape: {image.shape}")
    print(f"Minimum:     {image.min():.3f}")
    print(f"Maximum:     {image.max():.3f}")
    print(f"Mean:        {image.mean():.3f}")
    print(f"Total:       {image.sum():.3f}")


def save_visualization(
    image: np.ndarray,
    filename: str
) -> None:
    plt.figure()

    plt.imshow(
        image,
        origin="lower",
        interpolation="nearest"
    )

    plt.colorbar(
        label="Accumulated calibrated energy"
    )

    plt.xlabel("Channel / X")
    plt.ylabel("Detector / Y")
    plt.title("Synthetic PET-like reconstruction")

    plt.tight_layout()

    plt.savefig(
        filename,
        dpi=150
    )

    plt.close()


def parse_arguments() -> argparse.Namespace:
    parser = argparse.ArgumentParser(
        description="Analyze a PET-DAQ reconstruction."
    )

    parser.add_argument(
        "input",
        nargs="?",
        default="reconstruction.csv",
        help="Input reconstruction CSV."
    )

    parser.add_argument(
        "--output",
        default="reconstruction.png",
        help="Output visualization PNG."
    )

    return parser.parse_args()


def main() -> None:
    args = parse_arguments()

    image = load_reconstruction(
        args.input
    )

    print_statistics(image)

    save_visualization(
        image,
        args.output
    )

    print(
        f"Wrote visualization: "
        f"{OUTPUT_FILE}"
    )


if __name__ == "__main__":
    main()