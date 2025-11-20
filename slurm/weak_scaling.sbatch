#!/bin/bash
#SBATCH --job-name=stencil_weak
#SBATCH --nodes=2
#SBATCH --ntasks-per-node=4
#SBATCH --cpus-per-task=2
#SBATCH --time=00:20:00
#SBATCH --output=logs/weak_%j.out
#SBATCH --error=logs/weak_%j.err

set -euo pipefail
cd $SLURM_SUBMIT_DIR
mkdir -p logs results
source env/load_modules.sh
make -C src

export OMP_NUM_THREADS=${SLURM_CPUS_PER_TASK}
export OMP_PROC_BIND=true

LOCAL_NX=256
LOCAL_NY=1024
LOCAL_NZ=256
STEPS=300

for TASKS_PER_NODE in 2 4; do
  TOTAL_TASKS=$((TASKS_PER_NODE * SLURM_NNODES))
  GLOBAL_NX=$((LOCAL_NX * TOTAL_TASKS))
  srun --mpi=pmix_v3 --ntasks=${TOTAL_TASKS} --ntasks-per-node=${TASKS_PER_NODE} \
       ./src/miniweather_hpc --nx ${GLOBAL_NX} --ny ${LOCAL_NY} --nz ${LOCAL_NZ} --steps ${STEPS} \
       --output results/weak_scaling_template.csv
  sleep 2
done
