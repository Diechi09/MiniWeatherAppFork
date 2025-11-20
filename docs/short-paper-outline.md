# Short paper outline (IE University HPC assignment)

## 1. Introduction
- Context: motivation for simple 3D stencil as a miniWeather analogue.
- Goals: demonstrate MPI+OpenMP scaling on Magic Castle; quantify communication/computation balance.

## 2. Methods
- Code architecture: 1D domain decomposition (x-direction), halo exchange via `MPI_Sendrecv`, OpenMP-parallel loops.
- Experiment design: baseline single-node test, strong scaling (fixed global grid), weak scaling (per-rank workload fixed), profiling runs.
- Hardware/software stack: cluster node specs (fill from `SYSTEM.md`), modules used (from `env/modules.txt`), compiler flags.

## 3. Results
- Tables: populate `results/strong_scaling_template.csv` and `results/weak_scaling_template.csv` with measured timings.
- Plots: speedup and parallel efficiency for strong scaling; throughput vs nodes for weak scaling; place figures in `results/plots/`.
- Discussion: identify communication costs using `comm_time_s` column; highlight NUMA/placement settings if relevant.

## 4. Profiling and analysis
- `perf stat` or LIKWID metrics from `results/profiling_template.csv` (cycles, instructions, cache misses, bandwidth).
- Roofline-style commentary: compare measured FLOP/byte vs hardware peak.

## 5. Conclusions and future work
- Summary of scalability, bottlenecks, and portability observations.
- Next steps: hybrid decomposition, vectorization tuning, larger problem sizes.

## Appendix
- Command log: reference `reproduce.md` and the exact Slurm scripts used.
- Validation: checksum stability across runs and grid sizes.
