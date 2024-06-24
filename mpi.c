#include <stdio.h>
#include <math.h>
#include <mpi.h>

#define MAX_NUM 100000 

// Function to check if a number is prime
int is_prime(int n) {
    if (n < 2) return 0;
    if (n == 2) return 1;
    if (n % 2 == 0) return 0;
    for (int i = 3; i <= sqrt(n); i += 2) {
        if (n % i == 0) return 0;
    }
    return 1;
}

int main(int argc, char **argv) {
    MPI_Init(&argc, &argv);

    int world_size;
    MPI_Comm_size(MPI_COMM_WORLD, &world_size);

    int world_rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);

    double start_time, end_time, sequential_time = 0.0, parallel_time;

    // Calculate sequential execution time
    if (world_rank == 0) {
        printf("THIS IS MPI IMPLEMENTATION\n");

        start_time = MPI_Wtime();
        int count_primes_sequential = 0;
        for (int i = 0; i < MAX_NUM; i++) {
            if (is_prime(i)) 
                count_primes_sequential++;
        }
        end_time = MPI_Wtime();
        sequential_time = end_time - start_time;

        printf ("Number of primes (sequential): %d\n", count_primes_sequential);
        printf ("Sequential time: %.5f seconds\n", sequential_time);
    }

    MPI_Barrier(MPI_COMM_WORLD);  // Synchronize all processes before parallel section

    // Calculate parallel execution time
    int local_count = 0;
    start_time = MPI_Wtime();
    for (int i = world_rank; i < MAX_NUM; i += world_size) {
        if (is_prime(i)) {
            local_count++;
        }
    }
    int count_primes_parallel = 0;
    MPI_Reduce(&local_count, &count_primes_parallel, 1, MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD);
    end_time = MPI_Wtime();
    parallel_time = end_time - start_time;

    // Calculate initialization and finalization time (assuming they are negligible in this case)
    double init_time = 0.0, finalization_time = 0.0;
    double total_parallel_time = parallel_time + init_time + finalization_time;
    double parallelizable_portion = parallel_time / total_parallel_time;

    // Print results
    if (world_rank == 0) {
        double speedup = sequential_time / parallel_time;
        double efficiency = speedup / world_size;
        printf("Number of processors: %d\n", world_size);
        printf("Parallel run time (with %d threads): %.10f seconds\n", world_size, parallel_time);
        printf("Number of primes (parallel): %d\n", count_primes_parallel);
        printf("Speedup: %.2f\n", speedup);
        printf("Efficiency: %.2f\n", efficiency);
        printf("Proportion of parallelizable code: %.2f\n", parallelizable_portion);
        printf("\n");
    }

    MPI_Finalize();
    return 0;
}
