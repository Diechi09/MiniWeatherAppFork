#!/bin/bash
#SBATCH --job-name=stencil_baseline
#SBATCH --nodes=1
#SBATCH --ntasks-per-node=4
#SBATCH --cpus-per-task=4
#SBATCH --time=00:10:00
#SBATCH --output=logs/baseline_%j.out
#SBATCH --error=logs/baseline_%j.err

set -euo pipefail

cd $SLURM_SUBMIT_DIR
mkdir -p logs results
source env/load_modules.sh
make -C src

export OMP_NUM_THREADS=${SLURM_CPUS_PER_TASK}
export OMP_PROC_BIND=true

NZ=256

srun --mpi=pmix_v3 ./src/miniweather_hpc --nx 512 --ny 512 --nz ${NZ} --steps 200 --output results/strong_scaling_template.csv
