import re
from pathlib import Path

import matplotlib.pyplot as plt


def parse_access_times(path: Path):
    data = []
    current_orr = None
    for line in path.read_text().splitlines():
        orr_match = re.search(r"ORR count:\s*(\d+)", line)
        access_match = re.search(r"min transient access:\s*(\d+)", line)
        if orr_match:
            current_orr = int(orr_match.group(1))
        elif access_match and current_orr is not None:
            data.append((current_orr, int(access_match.group(1))))
            current_orr = None
    return data


def plot_file(path: Path):
    data = parse_access_times(path)
    if not data:
        print(f"[skip] No data found in {path.name}")
        return

    x_vals, y_vals = zip(*data)
    plt.figure(figsize=(8, 4))
    plt.plot(x_vals, y_vals, marker="o", linewidth=1, markersize=2)
    plt.xlabel("ORR count")
    plt.ylabel("Access time")
    plt.title(path.name)
    plt.grid(True, linestyle="--", linewidth=0.5)
    plt.tight_layout()

    output = path.with_suffix(".png")
    plt.savefig(output)
    plt.close()
    print(f"[ok] Saved {output}")


def main():
    figure_dir = Path(__file__).resolve().parent
    txt_files = sorted(figure_dir.glob("*.txt"))
    if not txt_files:
        print("No *.txt files found in figure directory.")
        return
    for file_path in txt_files:
        plot_file(file_path)


if __name__ == "__main__":
    main()
