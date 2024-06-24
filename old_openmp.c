#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <omp.h>

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
    printf("THIS IS OPENMP IMPLEMENTATION\n")

    int N = atoi(argv[1]);
    int num_threads[] = {2, 4, 6, 8, 12};
    int num_configs = sizeof(num_threads) / sizeof(num_threads[0]);

    for (int i = 0; i < num_configs; i++) {
        double start_time, end_time, sequential_time, parallel_time;
        int count_primes_sequential = 0, count_primes_parallel = 0;

        // Set the number of threads
        omp_set_num_threads(num_threads[i]);

        // Sequential run
        start_time = omp_get_wtime();
        for (int j = 2; j <= N; j++) {
            if (is_prime(j)) {
                count_primes_sequential++;
            }
        }
        end_time = omp_get_wtime();
        sequential_time = end_time - start_time;

        // Parallel run
        start_time = omp_get_wtime();
        #pragma omp parallel for reduction(+:count_primes_parallel)
        for (int j = 2; j <= N; j++) {
            if (is_prime(j)) {
                count_primes_parallel++;
            }
        }
        end_time = omp_get_wtime();
        parallel_time = end_time - start_time;

        // Print results
        // printf("Num threads: %d, Sequential time: %.2f, Parallel time: %.2f\n",
        //        num_threads[i], sequential_time, parallel_time);

        printf("Sequential run time : %.5f seconds\n", sequential_time);
        printf("Number of prime in [2, %d]: %d\n", N, count_primes_sequential);

        double speedup = sequential_time / parallel_time;
        double efficiency = speedup / num_threads[i];
        printf("Number of processors: %d\n", num_threads[i]);
        printf("Parallel run time (with %d threads): %.10f seconds\n", num_threads[i], parallel_time);
        printf ("Number of primes (parallel): %d\n", count_primes_parallel);
        printf("Speedup: %.2f\n", speedup);
        printf("Efficiency: %.2f\n", efficiency);
        printf("\n");
    }

    return 0;
}