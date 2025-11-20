# Reproduce the MPI+OpenMP stencil runs

These commands assume Magic Castle with Slurm and EESSI modules. Adjust `--ntasks-per-node` / `--nodes` to fit allocations.

## 1) Prepare environment
```
cd $(git rev-parse --show-toplevel)
source env/load_modules.sh
```

## 2) Build
```
make -C src
```

## 3) Quick functional test (interactive)
```
mpirun -np 2 ./src/miniweather_hpc --nx 48 --ny 48 --nz 32 --steps 20 --output results/strong_scaling_template.csv
```

## 4) Slurm runs
- Baseline 1 node: `sbatch slurm/baseline_single_node.sbatch`
- Strong scaling (>=2 nodes): `sbatch slurm/strong_scaling.sbatch`
- Weak scaling (>=2 nodes): `sbatch slurm/weak_scaling.sbatch`
- Profiling (perf): `sbatch slurm/profile_perf.sbatch`

## 5) Collect results
- Append rows are written automatically to the CSV specified by `--output` (defaults to `results/strong_scaling_template.csv`).
- Fill `results/weak_scaling_template.csv` and `results/profiling_template.csv` after running the corresponding Slurm jobs.
- Generate plots from the CSVs with `python results/plot_scaling.py` (PNGs land in `results/plots/`).

## 6) Document system details
Update `SYSTEM.md` with the node type, CPU model, interconnect, module versions, and any affinity flags used.
