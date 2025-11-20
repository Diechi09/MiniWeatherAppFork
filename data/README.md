# Data directory

This directory contains small demonstration inputs that travel with the repository so the application can be sanity-checked without external downloads.

* `sample_grid.txt` — a 4x4 scalar field slice that mirrors the kind of initial condition the 3D solver generates internally. You can feed it to post-processing scripts or compare against a debug dump from a 4x4x4 run.
* Additional experiment results generated on the cluster should be stored under `results/` instead of this folder.
