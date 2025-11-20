#!/usr/bin/env python3
"""Plot strong/weak scaling curves from CSV outputs.

Usage:
  python results/plot_scaling.py --strong results/strong_scaling_template.csv \
      --weak results/weak_scaling_template.csv

Outputs PNGs in results/plots/. Comment lines (starting with '#') are ignored.
"""

import argparse
import csv
from pathlib import Path
from typing import List

import matplotlib.pyplot as plt


def load_rows(path: Path) -> List[dict]:
    rows = []
    with path.open() as f:
        reader = csv.DictReader(row for row in f if not row.lstrip().startswith("#"))
        for row in reader:
            rows.append(row)
    return rows


def plot_strong(rows: List[dict], outdir: Path):
    if not rows:
        print("No strong scaling data found; skipping plot.")
        return
    tasks = [int(r["ranks"]) for r in rows]
    times = [float(r["total_time_s"]) for r in rows]
    baseline = times[0]
    speedup = [baseline / t for t in times]
    plt.figure()
    plt.plot(tasks, speedup, marker="o", label="Speedup")
    plt.xlabel("MPI ranks")
    plt.ylabel("Speedup vs smallest run")
    plt.title("Strong scaling")
    plt.grid(True)
    plt.savefig(outdir / "strong_scaling.png", dpi=150)
    plt.close()


def plot_weak(rows: List[dict], outdir: Path):
    if not rows:
        print("No weak scaling data found; skipping plot.")
        return
    nodes = [int(r["nodes"]) for r in rows]
    throughput = [float(r["updates_per_sec"]) for r in rows]
    plt.figure()
    plt.plot(nodes, throughput, marker="s", label="Throughput (updates/s)")
    plt.xlabel("Nodes")
    plt.ylabel("Throughput")
    plt.title("Weak scaling")
    plt.grid(True)
    plt.savefig(outdir / "weak_scaling.png", dpi=150)
    plt.close()


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--strong", type=Path, default=Path("results/strong_scaling_template.csv"))
    parser.add_argument("--weak", type=Path, default=Path("results/weak_scaling_template.csv"))
    parser.add_argument("--outdir", type=Path, default=Path("results/plots"))
    args = parser.parse_args()

    args.outdir.mkdir(parents=True, exist_ok=True)

    strong_rows = load_rows(args.strong) if args.strong.exists() else []
    weak_rows = load_rows(args.weak) if args.weak.exists() else []

    plot_strong(strong_rows, args.outdir)
    plot_weak(weak_rows, args.outdir)

    print(f"Plots (if data present) written to {args.outdir}")


if __name__ == "__main__":
    main()
