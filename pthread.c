#include <stdio.h>
#include <pthread.h>
#include <math.h>
#include <time.h>

#define MAX_NUM 100000
#define NUM_TESTS 5

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

// Structure for thread data
typedef struct {
    int start;
    int end;
    int count;
} ThreadData;

// Function to be run by each thread
void* check_numbers(void* arg) {
    ThreadData* data = (ThreadData*)arg;
    for (int i = data->start; i < data->end; i++) {
        if (is_prime(i)) {
            data->count++;
        }
    }
    return NULL;
}

int main() {
    // Calculate sequential execution time
    printf("THIS IS PTHREAD IMPLEMENTATION\n");

    clock_t start_time = clock();
    int count_primes_sequential = 0;
    for (int i = 0; i < MAX_NUM-1; i++) {
        if (is_prime(i)) 
            count_primes_sequential++;
    }
    clock_t end_time = clock();
    double sequential_time = (double)(end_time - start_time) / CLOCKS_PER_SEC;

    // Number of threads for each test
    int num_threads[NUM_TESTS] = {2, 4, 6, 8, 12};

    // Calculate parallel execution time and speedup ratio for each test
    for (int test = 0; test < NUM_TESTS; test++) {
        pthread_t threads[num_threads[test]];
        ThreadData thread_data[num_threads[test]];

        // Measure time for initialization
        start_time = clock();
        // Initialization code can be placed here (if any)
        end_time = clock();
        double init_time = (double)(end_time - start_time) / CLOCKS_PER_SEC;

        // Divide numbers between threads
        int numbers_per_thread = (MAX_NUM-1) / num_threads[test];
        for (int i = 0; i < num_threads[test]; i++) {
            thread_data[i].start = i * numbers_per_thread;
            thread_data[i].end = (i+1) * numbers_per_thread;
            thread_data[i].count = 0;  // Initialize count to 0 for each thread
            if (i == num_threads[test]-1) {
                thread_data[i].end = MAX_NUM-1;  // Last thread gets remaining numbers
            }
        }

        // Measure time for the parallel section
        start_time = clock();
        for (int i = 0; i < num_threads[test]; i++) {
            pthread_create(&threads[i], NULL, check_numbers, &thread_data[i]);
        }
        int count_primes_parallel = 0;
        for (int i = 0; i < num_threads[test]; i++) {
            pthread_join(threads[i], NULL);
            count_primes_parallel += thread_data[i].count;  // Add up counts from all threads
        }
        end_time = clock();
        double parallel_time = (double)(end_time - start_time) / CLOCKS_PER_SEC;

        // Measure time for finalization
        start_time = clock();
        // Finalization code can be placed here (if any)
        end_time = clock();
        double finalization_time = (double)(end_time - start_time) / CLOCKS_PER_SEC;

        // Calculate total execution time
        double total_time = init_time + parallel_time + finalization_time;

        // Calculate proportion of parallelizable code
        double P = parallel_time / total_time;

        // Print results
        printf("Num threads: %d\n", num_threads[test]);
        printf("Sequential run time: %.5f seconds\n", sequential_time);
        printf("Parallel run time (with %d threads): %.10f seconds\n", num_threads[test], parallel_time);
        printf("Initialization time: %.5f seconds\n", init_time);
        printf("Finalization time: %.5f seconds\n", finalization_time);
        printf("Total execution time: %.5f seconds\n", total_time);
        printf("Proportion of parallelizable code: %.2f\n", P);
        printf("Number of primes (sequential): %d\n", count_primes_sequential);
        printf("Number of primes (parallel): %d\n", count_primes_parallel);

        double speedup_ratio = sequential_time / parallel_time;
        printf("Speedup ratio: %.2f\n", speedup_ratio);
        printf("\n-----------------------\n");
    }

    return 0;
}
