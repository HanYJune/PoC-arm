#!/usr/bin/env python3
import argparse
import re
from pathlib import Path

import matplotlib.pyplot as plt


def parse_raw_loop_log(path: Path):
    re_iters = re.compile(r"Iters\s*=\s*(\d+)")
    re_random = re.compile(r"Loop Random Addr \+ Random Value:\s*([0-9]+(?:\.[0-9]+)?)")
    re_sasv = re.compile(r"Loop Striding Addr \+ Striding Value:\s*([0-9]+(?:\.[0-9]+)?)")
    re_sarv = re.compile(r"Loop Striding Addr \+ Random Value:\s*([0-9]+(?:\.[0-9]+)?)")

    points = {}
    cur_iters = None

    for line in path.read_text(errors="ignore").splitlines():
        m = re_iters.search(line)
        if m:
            cur_iters = int(m.group(1))
            points.setdefault(cur_iters, {})
            continue

        if cur_iters is None:
            continue

        m = re_random.search(line)
        if m:
            points[cur_iters]["random"] = float(m.group(1))
            continue

        m = re_sasv.search(line)
        if m:
            points[cur_iters]["sasv"] = float(m.group(1))
            continue

        m = re_sarv.search(line)
        if m:
            points[cur_iters]["sarv"] = float(m.group(1))
            continue

    # Left panel: Random vs Striding+Striding
    xs_left = sorted(k for k, v in points.items() if "random" in v and "sasv" in v)
    random_left = [points[x]["random"] for x in xs_left]
    sasv = [points[x]["sasv"] for x in xs_left]

    # Right panel: Random vs Striding+Random
    xs_right = sorted(k for k, v in points.items() if "random" in v and "sarv" in v)
    random_right = [points[x]["random"] for x in xs_right]
    sarv = [points[x]["sarv"] for x in xs_right]

    if not xs_left or not xs_right:
        raise ValueError(f"No usable points in {path}")

    return xs_left, random_left, sasv, xs_right, random_right, sarv


def plot_panel(ax, xs, y_random, y_other, title, other_label, show_ylabel):
    ax.plot(xs, y_random, linestyle=(0, (1.2, 2.2)), linewidth=2.0, color="#1f77b4", label="Random")
    ax.plot(xs, y_other, linestyle="-", linewidth=1.5, color="#ff7f0e", label=other_label)

    ax.set_title(title, fontsize=12)
    ax.set_xlabel("Iterations", fontsize=13)
    if show_ylabel:
        ax.set_ylabel("Cycles", fontsize=13)
    else:
        ax.set_yticklabels([])

    ax.tick_params(axis="both", labelsize=11)
    ax.set_xlim(min(xs), max(xs))


def main():
    p = argparse.ArgumentParser(description="Plot raw-loop log into two panels.")
    p.add_argument("log")
    args = p.parse_args()

    log_path = Path(args.log)
    xs_l, yr_l, ysasv, xs_r, yr_r, ysarv = parse_raw_loop_log(log_path)

    core_title = log_path.stem.upper()
    output = log_path.with_name(f"{log_path.stem}_two_panel.png")

    fig, axes = plt.subplots(1, 2, figsize=(7.4, 4.0), sharey=False)

    plot_panel(
        axes[0],
        xs_l,
        yr_l,
        ysasv,
        f"{core_title}: Random vs Striding+Striding",
        "Striding+Striding",
        show_ylabel=True,
    )
    plot_panel(
        axes[1],
        xs_r,
        yr_r,
        ysarv,
        f"{core_title}: Random vs Striding+Random",
        "Striding+Random",
        show_ylabel=False,
    )

    axes[0].legend(loc="upper left", frameon=True, fontsize=10)
    axes[1].legend(loc="upper left", frameon=True, fontsize=10)

    fig.subplots_adjust(bottom=0.14, wspace=0.20)
    fig.savefig(output, dpi=200, bbox_inches="tight")
    print(f"Saved: {output}")


if __name__ == "__main__":
    main()
