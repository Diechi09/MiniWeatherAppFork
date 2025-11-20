# EuroHPC-style proposal outline

## 1. Scientific/technical objectives
- Demonstrate scalability of a lightweight weather-like stencil on EuroHPC-class hardware.
- Explore communication/computation overlap potential using MPI + OpenMP.
- Produce actionable tuning guidance for student teams targeting Alliance/EESSI images.

## 2. Computational methodology
- 3D stencil kernel with explicit halo exchange; one-dimensional domain decomposition along x.
- Performance model: surface/volume ratio for halos, expected bandwidth-bound behavior.
- Parallelization: MPI for distributed slabs, OpenMP for intra-node threading; affinity guidance via `OMP_PROC_BIND=true`.

## 3. Resources requested
- Nodes: fill in from `SYSTEM.md` after characterizing CPU type and memory bandwidth.
- Test plan: baseline (1 node, 4–8 ranks), strong scaling (up to >=2 nodes), weak scaling (fixed cells/rank), profiling (perf/LIKWID).
- Storage: minimal; CSV outputs and plots under `results/`.

## 4. Work plan and milestones
- M1: Environment validation using `env/load_modules.sh` and `make -C src`.
- M2: Baseline correctness (checksum stability), verify CSV logging.
- M3: Scaling campaigns via `slurm/strong_scaling.sbatch` and `slurm/weak_scaling.sbatch`.
- M4: Profiling passes using `slurm/profile_perf.sbatch`; integrate metrics into analysis.

## 5. Impact and dissemination
- Teaching artifact for IE University HPC coursework; reproducibility via `reproduce.md`.
- Public repository with scripts + outlines enabling rapid onboarding for future cohorts.
