#!/bin/bash
#SBATCH --job-name=stencil_strong
#SBATCH --nodes=2
#SBATCH --ntasks-per-node=4
#SBATCH --cpus-per-task=2
#SBATCH --time=00:20:00
#SBATCH --output=logs/strong_%j.out
#SBATCH --error=logs/strong_%j.err

set -euo pipefail
cd $SLURM_SUBMIT_DIR
mkdir -p logs results
source env/load_modules.sh
make -C src

export OMP_NUM_THREADS=${SLURM_CPUS_PER_TASK}
export OMP_PROC_BIND=true

GLOBAL_NX=2048
GLOBAL_NY=2048
GLOBAL_NZ=512
STEPS=400

for TASKS_PER_NODE in 2 4; do
  TOTAL_TASKS=$((TASKS_PER_NODE * SLURM_NNODES))
  srun --mpi=pmix_v3 --ntasks=${TOTAL_TASKS} --ntasks-per-node=${TASKS_PER_NODE} \
       ./src/miniweather_hpc --nx ${GLOBAL_NX} --ny ${GLOBAL_NY} --nz ${GLOBAL_NZ} --steps ${STEPS} \
       --output results/strong_scaling_template.csv
  sleep 2
done
