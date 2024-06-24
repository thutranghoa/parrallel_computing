#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <omp.h>
#include <sys/resource.h>

struct rusage r_usage;

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
    if (argc != 2) {
        printf("Usage: %s <N>\n", argv[0]);
        return 1;
    }

    printf("THIS IS OPENMP IMPLEMENTATION\n");

    int N = atoi(argv[1]);
    int num_threads[] = {2, 4, 6, 8, 12};
    int num_configs = sizeof(num_threads) / sizeof(num_threads[0]);

    for (int i = 0; i < num_configs; i++) {
        double start_time, end_time;
        double init_time, parallel_time, finalization_time;
        int count_primes_sequential = 0, count_primes_parallel = 0;

        // Set the number of threads
        omp_set_num_threads(num_threads[i]);

        // Sequential run (for comparison)
        start_time = omp_get_wtime();
        for (int j = 2; j <= N; j++) {
            if (is_prime(j)) {
                count_primes_sequential++;
            }
        }
        end_time = omp_get_wtime();
        double sequential_time = end_time - start_time;

        // Measure time for initialization
        start_time = omp_get_wtime();
        // Initialization code (if any) can be placed here
        end_time = omp_get_wtime();
        init_time = end_time - start_time;

        // Parallel run
        start_time = omp_get_wtime();
        #pragma omp parallel for reduction(+:count_primes_parallel)
        for (int j = 2; j <= N; j++) {
            if (is_prime(j)) {
                count_primes_parallel++;
            }
        }
        end_time = omp_get_wtime();
        getrusage(RUSAGE_SELF, &r_usage);

        parallel_time = end_time - start_time;

        // Measure time for finalization
        start_time = omp_get_wtime();
        // Finalization code (if any) can be placed here
        end_time = omp_get_wtime();
        finalization_time = end_time - start_time;

        // Calculate total execution time
        double total_time = init_time + parallel_time + finalization_time;

        // Calculate proportion of parallelizable code
        double P = parallel_time / total_time;

        // Print results
        printf("Num threads: %d\n", num_threads[i]);
        printf("Sequential run time : %.5f seconds\n", sequential_time);
        printf("Parallel run time (with %d threads): %.10f seconds\n", num_threads[i], parallel_time);
        printf("Initialization time: %.5f seconds\n", init_time);
        printf("Finalization time: %.5f seconds\n", finalization_time);
        printf("Total execution time: %.5f seconds\n", total_time);
        printf("Proportion of parallelizable code: %.2f\n", P);
        printf("Number of primes (sequential): %d\n", count_primes_sequential);
        printf("Number of primes (parallel): %d\n", count_primes_parallel);

        double speedup = sequential_time / parallel_time;
        double efficiency = speedup / num_threads[i];
        printf("Speedup: %.2f\n", speedup);
        printf("Efficiency: %.2f\n", efficiency);
        printf("Memory usage: %ld kilobytes\n", r_usage.ru_maxrss);
        printf("\n");
    }

    return 0;
}
