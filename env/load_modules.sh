#!/bin/bash
# Source this script on Magic Castle to prepare the build/run environment.
# If modules are not available, fall back to system defaults.

set -euo pipefail

if command -v module &>/dev/null; then
  module purge
  module load EESSI/2023.12
  module load foss/2023b
  module load perf/6.6
  # LIKWID is optional; skip if absent
  module avail likwid &>/dev/null && module load likwid/5.2.2 || true
else
  echo "module command not found; relying on system compiler and MPI"
fi

mpicc --version || echo "Warning: mpicc not found in current environment"
