#!/usr/bin/env python3
import argparse
import csv
from collections import defaultdict

#python3 /home/hyj/pixel/tiktag/gadget/plot_backpac.py /home/hyj/pixel/tiktag/gadget/backpacktest.txt \ --output /home/hyj/pixel/tiktag/gadget/backpac_latency.png
def load_rows(path):
    rows = []
    with open(path, "r", newline="") as f:
        reader = csv.reader(f)
        for row in reader:
            if len(row) < 4:
                continue
            try:
                cpu = int(row[0])
                passed = int(row[1])
                nop = int(row[2])
                latency = int(row[3])
            except ValueError:
                continue
            rows.append((cpu, passed, nop, latency))
    return rows


def aggregate(rows):
    sums = defaultdict(int)
    counts = defaultdict(int)
    for _cpu, passed, nop, latency in rows:
        key = (passed, nop)
        sums[key] += latency
        counts[key] += 1
    averages = defaultdict(list)
    for (passed, nop), total in sums.items():
        averages[passed].append((nop, total / counts[(passed, nop)]))
    for passed in averages:
        averages[passed].sort(key=lambda x: x[0])
    return averages


def main():
    parser = argparse.ArgumentParser(
        description="Plot latency vs orr_count for PAC aut success/fail."
    )
    parser.add_argument("input", help="CSV file with cpu,pass,nop,latency rows")
    parser.add_argument(
        "--output",
        default="backpac_latency.png",
        help="Output image path (default: backpac_latency.png)",
    )
    args = parser.parse_args()

    rows = load_rows(args.input)
    if not rows:
        raise SystemExit("No valid rows found in input.")

    averages = aggregate(rows)

    try:
        import matplotlib.pyplot as plt
    except Exception as exc:
        raise SystemExit(f"matplotlib is required: {exc}")

    plt.figure(figsize=(9, 5))
    if 1 in averages:
        xs, ys = zip(*averages[1])
        plt.plot(xs, ys, label="pass=1 (aut success)")
    if 0 in averages:
        xs, ys = zip(*averages[0])
        plt.plot(xs, ys, label="pass=0 (aut fail)")

    plt.xlabel("nop_count")
    plt.ylabel("avg latency")
    plt.title("Latency vs orr_count (AUT success/fail)")
    plt.legend()
    plt.grid(True, alpha=0.3)
    plt.tight_layout()
    plt.savefig(args.output, dpi=150)


if __name__ == "__main__":
    main()
