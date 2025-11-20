#!/bin/bash
#SBATCH --job-name=stencil_perf
#SBATCH --nodes=1
#SBATCH --ntasks-per-node=2
#SBATCH --cpus-per-task=8
#SBATCH --time=00:15:00
#SBATCH --output=logs/perf_%j.out
#SBATCH --error=logs/perf_%j.err

set -euo pipefail
cd $SLURM_SUBMIT_DIR
mkdir -p logs results
source env/load_modules.sh
make -C src

export OMP_NUM_THREADS=${SLURM_CPUS_PER_TASK}
export OMP_PROC_BIND=true

METRICS="cycles,instructions,cache-references,cache-misses"

perf stat -e ${METRICS} -a -- srun --mpi=pmix_v3 ./src/miniweather_hpc \
    --nx 1024 --ny 1024 --nz 256 --steps 300 --output results/profiling_template.csv
