# 3-minute pitch outline

## Problem
- Students need a reproducible HPC mini-application to learn distributed-memory scaling and profiling.

## Solution
- Provide a compact MPI+OpenMP 3D stencil with clear CLI, CSV logging, and ready-to-run Slurm scripts.
- Include plots/templates and documentation that map directly to the IE University assignment deliverables.

## Demo flow
1. Show repository structure and `reproduce.md`.
2. Build with `make -C src` after `source env/load_modules.sh`.
3. Run `sbatch slurm/baseline_single_node.sbatch` then scale with `slurm/strong_scaling.sbatch`.
4. Present auto-generated CSV rows and explain how to produce plots from `results/` templates.

## Proof points
- Correctness: stable checksum across ranks and steps.
- Performance: throughput (`updates_per_sec`) and communication fraction (`comm_time_s`) recorded for every run.
- Portability: works with modules or Apptainer definition; Slurm scripts specify binding and MPI layout.

## Call to action
- Run the provided scripts on Magic Castle, fill in `SYSTEM.md`, and complete the scaling tables for the report.
