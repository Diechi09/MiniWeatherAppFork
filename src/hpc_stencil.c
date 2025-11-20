#include <mpi.h>
#include <omp.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <time.h>
#include <unistd.h>

#define DEFAULT_NX 128
#define DEFAULT_NY 128
#define DEFAULT_NZ 128
#define DEFAULT_STEPS 200
#define DEFAULT_OUTPUT "results/strong_scaling_template.csv"

#define IDX(i, j, k, ny, nz) \
    (((size_t)(i) * (size_t)(ny) + (size_t)(j)) * (size_t)(nz) + (size_t)(k))

typedef struct {
    int nx;
    int ny;
    int nz;
    int steps;
    char output_path[512];
} run_config;

static void print_usage(const char *prog) {
    if (!prog) prog = "miniweather_hpc";
    fprintf(stderr,
            "Usage: %s [--nx N] [--ny N] [--nz N] [--steps S] [--output path]\n"
            "  --nx / --ny / --nz   Grid dimensions (default %d x %d x %d)\n"
            "  --steps             Number of stencil iterations (default %d)\n"
            "  --output            CSV file to append timing results (default %s)\n",
            prog, DEFAULT_NX, DEFAULT_NY, DEFAULT_NZ, DEFAULT_STEPS, DEFAULT_OUTPUT);
}

static void parse_args(int argc, char **argv, run_config *cfg) {
    cfg->nx = DEFAULT_NX;
    cfg->ny = DEFAULT_NY;
    cfg->nz = DEFAULT_NZ;
    cfg->steps = DEFAULT_STEPS;
    snprintf(cfg->output_path, sizeof(cfg->output_path), "%s", DEFAULT_OUTPUT);

    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "--nx") == 0 && i + 1 < argc) {
            cfg->nx = atoi(argv[++i]);
        } else if (strcmp(argv[i], "--ny") == 0 && i + 1 < argc) {
            cfg->ny = atoi(argv[++i]);
        } else if (strcmp(argv[i], "--nz") == 0 && i + 1 < argc) {
            cfg->nz = atoi(argv[++i]);
        } else if (strcmp(argv[i], "--steps") == 0 && i + 1 < argc) {
            cfg->steps = atoi(argv[++i]);
        } else if (strcmp(argv[i], "--output") == 0 && i + 1 < argc) {
            snprintf(cfg->output_path, sizeof(cfg->output_path), "%s", argv[++i]);
        } else if (strcmp(argv[i], "--help") == 0 || strcmp(argv[i], "-h") == 0) {
            print_usage(argv[0]);
            MPI_Abort(MPI_COMM_WORLD, 1);
        }
    }
}

static inline double initial_condition(int gx, int gy, int gz) {
    return 1.0 + 0.01 * gx + 0.02 * gy + 0.03 * gz;
}

static void initialize_field(double *grid, int local_nx, int ny, int nz, int global_x0) {
    #pragma omp parallel for collapse(3)
    for (int i = 1; i <= local_nx; i++) {
        for (int j = 0; j < ny; j++) {
            for (int k = 0; k < nz; k++) {
                int gx = global_x0 + (i - 1);
                grid[IDX(i, j, k, ny, nz)] = initial_condition(gx, j, k);
            }
        }
    }
}

static void exchange_halos(double *grid, int local_nx, int ny, int nz, int left, int right, MPI_Comm comm) {
    size_t plane = (size_t)ny * (size_t)nz;
    MPI_Sendrecv(&grid[IDX(1, 0, 0, ny, nz)], plane, MPI_DOUBLE, left, 0,
                 &grid[IDX(local_nx + 1, 0, 0, ny, nz)], plane, MPI_DOUBLE, right, 0,
                 comm, MPI_STATUS_IGNORE);

    MPI_Sendrecv(&grid[IDX(local_nx, 0, 0, ny, nz)], plane, MPI_DOUBLE, right, 1,
                 &grid[IDX(0, 0, 0, ny, nz)], plane, MPI_DOUBLE, left, 1,
                 comm, MPI_STATUS_IGNORE);
}

static void stencil_step(double *restrict grid, double *restrict next, int local_nx, int ny, int nz) {
    #pragma omp parallel for collapse(3)
    for (int i = 1; i <= local_nx; i++) {
        for (int j = 1; j < ny - 1; j++) {
            for (int k = 1; k < nz - 1; k++) {
                double xm = grid[IDX(i - 1, j, k, ny, nz)];
                double xp = grid[IDX(i + 1, j, k, ny, nz)];
                double ym = grid[IDX(i, j - 1, k, ny, nz)];
                double yp = grid[IDX(i, j + 1, k, ny, nz)];
                double zm = grid[IDX(i, j, k - 1, ny, nz)];
                double zp = grid[IDX(i, j, k + 1, ny, nz)];
                next[IDX(i, j, k, ny, nz)] = (xm + xp + ym + yp + zm + zp) / 6.0;
            }
        }
    }
}

static double compute_checksum(double *grid, int local_nx, int ny, int nz) {
    double local_sum = 0.0;
    #pragma omp parallel for collapse(3) reduction(+:local_sum)
    for (int i = 1; i <= local_nx; i++) {
        for (int j = 0; j < ny; j++) {
            for (int k = 0; k < nz; k++) {
                local_sum += grid[IDX(i, j, k, ny, nz)];
            }
        }
    }
    return local_sum;
}

static void ensure_output_dir(const char *path) {
    char buffer[512];
    snprintf(buffer, sizeof(buffer), "%s", path);
    char *last_slash = strrchr(buffer, '/');
    if (last_slash) {
        *last_slash = '\0';
        if (strlen(buffer) > 0) {
            struct stat st = {0};
            if (stat(buffer, &st) != 0) {
                mkdir(buffer, 0755);
            }
        }
    }
}

int main(int argc, char **argv) {
    MPI_Init(&argc, &argv);

    int rank, world;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &world);

    run_config cfg;
    parse_args(argc, argv, &cfg);

    if (cfg.nx < world) {
        if (rank == 0) {
            fprintf(stderr, "Grid x-dimension (%d) must be >= MPI size (%d) for 1D decomposition.\n", cfg.nx, world);
        }
        MPI_Abort(MPI_COMM_WORLD, 2);
    }

    int base = cfg.nx / world;
    int rem = cfg.nx % world;
    int local_nx = base + (rank < rem ? 1 : 0);
    int global_x0 = rank * base + (rank < rem ? rank : rem);

    size_t slab_elems = (size_t)(local_nx + 2) * (size_t)cfg.ny * (size_t)cfg.nz;
    double *grid = (double *)calloc(slab_elems, sizeof(double));
    double *next = (double *)calloc(slab_elems, sizeof(double));
    if (!grid || !next) {
        fprintf(stderr, "Rank %d failed to allocate grid of %zu doubles.\n", rank, slab_elems);
        MPI_Abort(MPI_COMM_WORLD, 3);
    }

    initialize_field(grid, local_nx, cfg.ny, cfg.nz, global_x0);

    int left = (rank == 0) ? MPI_PROC_NULL : rank - 1;
    int right = (rank == world - 1) ? MPI_PROC_NULL : rank + 1;

    double comm_time = 0.0;
    double comp_time = 0.0;

    MPI_Barrier(MPI_COMM_WORLD);
    double t0 = MPI_Wtime();

    for (int step = 0; step < cfg.steps; step++) {
        double cstart = MPI_Wtime();
        exchange_halos(grid, local_nx, cfg.ny, cfg.nz, left, right, MPI_COMM_WORLD);
        comm_time += MPI_Wtime() - cstart;

        double kstart = MPI_Wtime();
        stencil_step(grid, next, local_nx, cfg.ny, cfg.nz);
        double *tmp = grid;
        grid = next;
        next = tmp;
        comp_time += MPI_Wtime() - kstart;
    }

    MPI_Barrier(MPI_COMM_WORLD);
    double total_time = MPI_Wtime() - t0;

    double local_checksum = compute_checksum(grid, local_nx, cfg.ny, cfg.nz);

    double global_checksum = 0.0;
    double max_comm = 0.0, max_comp = 0.0, max_total = 0.0;

    MPI_Reduce(&local_checksum, &global_checksum, 1, MPI_DOUBLE, MPI_SUM, 0, MPI_COMM_WORLD);
    MPI_Reduce(&comm_time, &max_comm, 1, MPI_DOUBLE, MPI_MAX, 0, MPI_COMM_WORLD);
    MPI_Reduce(&comp_time, &max_comp, 1, MPI_DOUBLE, MPI_MAX, 0, MPI_COMM_WORLD);
    MPI_Reduce(&total_time, &max_total, 1, MPI_DOUBLE, MPI_MAX, 0, MPI_COMM_WORLD);

    if (rank == 0) {
        ensure_output_dir(cfg.output_path);
        int file_exists = (access(cfg.output_path, F_OK) == 0);
        FILE *out = fopen(cfg.output_path, "a");
        if (!out) {
            fprintf(stderr, "Could not open %s for writing.\n", cfg.output_path);
            MPI_Abort(MPI_COMM_WORLD, 4);
        }

        if (!file_exists) {
            fprintf(out,
                    "nodes,ranks,threads_per_rank,nx,ny,nz,steps,total_time_s,compute_time_s,comm_time_s,updates_per_sec,checksum,timestamp\n");
        }

        const char *nodes_env = getenv("SLURM_NNODES");
        int nodes = nodes_env ? atoi(nodes_env) : 1;
        int threads = omp_get_max_threads();
        double updates = (double)cfg.nx * (double)cfg.ny * (double)cfg.nz * (double)cfg.steps;
        double ups = updates / max_total;

        time_t now = time(NULL);
        char time_buf[64];
        strftime(time_buf, sizeof(time_buf), "%Y-%m-%dT%H:%M:%S%z", localtime(&now));

        fprintf(out, "%d,%d,%d,%d,%d,%d,%d,%.6f,%.6f,%.6f,%.2f,%.4e,%s\n",
                nodes, world, threads, cfg.nx, cfg.ny, cfg.nz, cfg.steps,
                max_total, max_comp, max_comm, ups, global_checksum, time_buf);

        fclose(out);
    }

    free(grid);
    free(next);

    MPI_Finalize();
    return 0;
}
