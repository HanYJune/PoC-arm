#!/usr/bin/env python3
import argparse
import re
from pathlib import Path

import matplotlib.pyplot as plt
import numpy as np
from matplotlib.colors import ListedColormap


def parse_log(path: Path):
    re_len = re.compile(r"Linked list length:\s*(-?\d+)")
    re_stride = re.compile(r"Stride between loads:\s*(-?\d+)")
    re_counts = re.compile(r"(\d+)\s*/\s*(\d+)\s*transient\s*,\s*(\d+)\s*/\s*(\d+)\s*architectural")

    cur_len = None
    cur_stride = None
    rows = []

    for raw in path.read_text(errors="ignore").splitlines():
        line = raw.strip()
        m = re_len.search(line)
        if m:
            cur_len = int(m.group(1))
            continue
        m = re_stride.search(line)
        if m:
            cur_stride = int(m.group(1))
            continue
        m = re_counts.search(line)
        if m and cur_len is not None and cur_stride is not None:
            trans_num = int(m.group(1))
            trans_den = int(m.group(2))
            arch_num = int(m.group(3))
            arch_den = int(m.group(4))
            rows.append((cur_len, cur_stride, trans_num, trans_den, arch_num, arch_den))

    if not rows:
        raise ValueError(f"No records parsed from: {path}")
    return rows


def build_matrix(rows, metric="transient", x_min=None, x_max=None, y_min=None, y_max=None):
    lengths = sorted({r[0] for r in rows})
    strides = sorted({r[1] for r in rows})

    if x_min is not None:
        lengths = [x for x in lengths if x >= x_min]
    if x_max is not None:
        lengths = [x for x in lengths if x <= x_max]
    if y_min is not None:
        strides = [y for y in strides if y >= y_min]
    if y_max is not None:
        strides = [y for y in strides if y <= y_max]

    if not lengths or not strides:
        raise ValueError("No data left after applying axis filters.")

    lx = {v: i for i, v in enumerate(lengths)}
    sy = {v: i for i, v in enumerate(strides)}

    mat = np.full((len(strides), len(lengths)), np.nan, dtype=float)
    den = None

    for ll, st, tn, td, an, ad in rows:
        if ll not in lx or st not in sy:
            continue
        if metric == "transient":
            val, den = tn, td
        else:
            val, den = an, ad
        mat[sy[st], lx[ll]] = float(val)

    return lengths, strides, mat, den


def make_cmap():
    # Value 0 is yellow; positive values use Blues.
    blues = plt.get_cmap("Blues")
    colors = ["#ececab"] + [blues(i) for i in np.linspace(0.18, 1.0, 255)]
    cmap = ListedColormap(colors)
    cmap.set_bad("#ececab")
    return cmap


def edges(vals):
    if len(vals) == 1:
        return vals[0] - 0.5, vals[0] + 0.5
    step = np.median(np.diff(vals))
    return vals[0] - step / 2.0, vals[-1] + step / 2.0


def main():
    p = argparse.ArgumentParser(description="Plot SLAP heatmap from log file")
    p.add_argument("input_pos", nargs="?", help="Input log path")
    p.add_argument("output_pos", nargs="?", help="Output image path")
    p.add_argument("--input", dest="input_opt", default=None)
    p.add_argument("--output", dest="output_opt", default=None)
    p.add_argument("--metric", choices=["transient", "architectural"], default="transient")
    p.add_argument("--vmax", type=float, default=10.0)
    p.add_argument("--x-min", type=int, default=None)
    p.add_argument("--x-max", type=int, default=None)
    p.add_argument("--y-min", type=int, default=None)
    p.add_argument("--y-max", type=int, default=None)
    p.add_argument("--caption", action="store_true", help="Add caption text below the plot")
    args = p.parse_args()

    input_path = args.input_pos or args.input_opt or "/home/hyj/pixel/slap-artifacts/out/x4.log"
    if args.output_pos:
        output_path = args.output_pos
    elif args.output_opt:
        output_path = args.output_opt
    else:
        output_path = str(Path(input_path).with_name(f"{Path(input_path).stem}_heatmap.png"))

    rows = parse_log(Path(input_path))
    lengths, strides, mat, den = build_matrix(
        rows,
        metric=args.metric,
        x_min=args.x_min,
        x_max=args.x_max,
        y_min=args.y_min,
        y_max=args.y_max,
    )

    x0, x1 = edges(lengths)
    y0, y1 = edges(strides)

    fig_h = 7.2 if args.caption else 6.2
    fig, ax = plt.subplots(figsize=(7.4, fig_h))
    if args.caption:
        fig.subplots_adjust(bottom=0.26, right=0.88)
    else:
        fig.subplots_adjust(right=0.88)

    plot_mat = np.nan_to_num(mat, nan=0.0)
    im = ax.imshow(
        plot_mat,
        origin="lower",
        aspect="auto",
        interpolation="nearest",
        extent=[x0, x1, y0, y1],
        cmap=make_cmap(),
        vmin=0,
        vmax=args.vmax,
    )

    ax.set_xlabel("Training Length")
    ax.set_ylabel("Stride (B)")

    # Match figure style with dense y ticks.
    if y0 <= -320 and y1 >= 320:
        ax.set_yticks(np.arange(-320, 321, 32))

    cbar = fig.colorbar(im, ax=ax, fraction=0.03, pad=0.05)
    cbar.set_label(f"LAP Activations / {den or int(args.vmax)}")

    if args.caption:
        fig.text(
            0.02,
            0.06,
            "Figure: Heatmap showing the effects of linked-list length and stride on LAP activation.\n"
            "Values of zero are highlighted in yellow.",
            fontsize=11,
            family="serif",
        )

    fig.savefig(output_path, dpi=180)
    print(f"Saved: {output_path}")


if __name__ == "__main__":
    main()
