#!/bin/bash
# Simple wrapper to run the solver via srun or mpirun on the allocated node(s).
# Usage: ./slurm/run.sh <ranks> <threads> <nx> <ny> <nz> <steps> <output_csv>
set -euo pipefail

source $(dirname "$0")/../env/load_modules.sh

RANKS=${1:-2}
THREADS=${2:-4}
NX=${3:-256}
NY=${4:-256}
NZ=${5:-256}
STEPS=${6:-100}
OUT=${7:-results/strong_scaling_template.csv}

export OMP_NUM_THREADS=${THREADS}
export OMP_PROC_BIND=true

if command -v srun &>/dev/null; then
  srun --mpi=pmix_v3 -n ${RANKS} ./src/miniweather_hpc --nx ${NX} --ny ${NY} --nz ${NZ} --steps ${STEPS} --output ${OUT}
else
  mpirun -np ${RANKS} ./src/miniweather_hpc --nx ${NX} --ny ${NY} --nz ${NZ} --steps ${STEPS} --output ${OUT}
fi
