#!/usr/bin/env python3
import re
import sys
from pathlib import Path

import matplotlib.pyplot as plt


def parse_log(path: Path):
    section = None
    muls = None

    data = {
        "pac_flushed": {},  # Last node flushed, predicted address cached
        "pac_cached": {},   # Last node cached, predicted address cached
        "paf_flushed": {},  # Last node flushed, predicted address flushed
        "paf_cached": {},   # Last node cached, predicted address flushed
    }

    sec_map = {
        "Last node flushed, predicted address cached:": "pac_flushed",
        "Last node cached, predicted address cached:": "pac_cached",
        "Last node flushed, predicted address flushed:": "paf_flushed",
        "Last node cached, predicted address flushed:": "paf_cached",
    }

    re_muls = re.compile(r"MULs in speculation window:\s*(\d+)")
    re_counts = re.compile(r"(\d+)\s*/\s*(\d+)\s*transient\s*,\s*(\d+)\s*/\s*(\d+)\s*architectural")

    for raw in path.read_text(errors="ignore").splitlines():
        line = raw.strip()

        if line in sec_map:
            section = sec_map[line]
            muls = None
            continue

        m = re_muls.search(line)
        if m:
            muls = int(m.group(1))
            continue

        m = re_counts.search(line)
        if m and section is not None and muls is not None:
            transient = int(m.group(1))
            data[section][muls] = transient

    return data


def sorted_xy(d):
    xs = sorted(d.keys())
    ys = [d[x] for x in xs]
    return xs, ys


def main():
    # Usage: script [log_file]
    log_path = Path(sys.argv[1]) if len(sys.argv) > 1 else Path("/home/hyj/pixel/slap-artifacts/out/speculation-window/a720.log")
    out_path = log_path.with_name(f"{log_path.stem}_pred_addr_cached.png")

    data = parse_log(log_path)

    if not data["pac_flushed"] and not data["pac_cached"]:
        raise SystemExit(f"No 'predicted address cached' sections found in {log_path}")

    fig, ax = plt.subplots(figsize=(4.8, 2.8))

    if data["pac_flushed"]:
        x1, y1 = sorted_xy(data["pac_flushed"])
        ax.plot(x1, y1, color="#1f77b4", linewidth=2.2, linestyle="-", label="Last node flushed")

    if data["pac_cached"]:
        x2, y2 = sorted_xy(data["pac_cached"])
        ax.plot(x2, y2, color="#ff7f0e", linewidth=2.2, linestyle=(0, (1, 2)), label="Last node cached")

    ax.set_title("Predicted Address Cached", fontsize=18)
    ax.set_xlabel("MULs", fontsize=20)
    ax.set_ylabel("Activations", fontsize=20)
    ax.tick_params(axis="both", labelsize=17)
    ax.set_xlim(-10, 310)
    ax.set_ylim(-50, 1000)

    ax.legend(loc="upper right", fontsize=10, frameon=False)

    fig.tight_layout()
    fig.savefig(out_path, dpi=200)
    print(f"Saved: {out_path}")


if __name__ == "__main__":
    main()
