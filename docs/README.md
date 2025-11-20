# MiniWeather-style MPI+OpenMP stencil

This project packages a CPU-only 3D stencil (miniWeather-like) solver used for the IE University HPC group assignment. It is engineered for the Magic Castle cluster (Slurm + Alliance/EESSI modules) and includes everything needed to build, run, scale, profile, and report performance.

## Repository structure
- `src/` — Solver source code and `Makefile` (build with `make -C src`).
- `env/` — Module list, load script, and optional Apptainer definition for portable runs.
- `slurm/` — Job scripts for baseline, strong-scaling, weak-scaling, profiling, and a wrapper run helper.
- `data/` — Tiny sample grid for sanity checks.
- `results/` — CSV templates plus a `plots/` stub for generated figures.
- `docs/` — Assignment-facing documentation, paper/proposal/pitch outlines.
- `reproduce.md` — Single place for build and run commands.
- `SYSTEM.md` — Fill in cluster details (node type, module versions) after first run.

## Key features
- MPI domain decomposition along the x-dimension with halo exchange of yz-planes.
- OpenMP acceleration of the inner 3D 7-point stencil updates.
- CLI for grid sizes, steps, and CSV logging destination.
- CSV timing + throughput logging compatible with downstream plotting.
- Slurm scripts for 1-node baseline, multi-node strong/weak scaling, and perf/LIKWID profiling hooks.

## What to customize after cloning
1. Load the right modules via `env/load_modules.sh` or update `env/modules.txt` to match your Magic Castle image.
2. Update `SYSTEM.md` with actual node types and module versions discovered on the cluster.
3. Run the baseline job in `slurm/baseline_single_node.sbatch` to confirm functionality, then execute strong/weak scaling jobs and paste results into the CSV templates under `results/`.
4. Use `results/plots/` for generated scaling curves (see plotting guidance in `docs/short-paper-outline.md`).
