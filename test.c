#include <stdio.h>
#include <pthread.h>
#include <math.h>
#include <time.h>

#define MAX_NUM 1000000
#define NUM_TESTS 5

int numbers[MAX_NUM-1];
int num_primes = 0;
pthread_mutex_t mutex;

// Function to check if a number is prime
int is_prime(int index) {
    int n = numbers[index];
    int i;
    if (n < 2) return 0;
    for (i = 2; i <= sqrt(n); i++) {
        if (n % i == 0) return 0;
    }
    return 1;
}

// Structure for thread data
typedef struct {
    int start;
    int end;
} ThreadData;

// Function to be run by each thread
void* check_numbers(void* arg) {
    ThreadData* data = (ThreadData*)arg;
    for (int i = data->start; i < data->end; i++) {
        if (is_prime(i)) {
            pthread_mutex_lock(&mutex);
            num_primes++;
            pthread_mutex_unlock(&mutex);
        }
    }
    return NULL;
}

int main() {
    // Initialize numbers
    for (int i = 0; i < MAX_NUM-1; i++) {
        numbers[i] = i+2;
    }

    // Initialize mutex
    pthread_mutex_init(&mutex, NULL);

    // Number of threads for each test
    int num_threads[NUM_TESTS] = {2, 4, 6, 8, 12};

    // Calculate parallel execution time and speedup ratio for each test
    for (int test = 0; test < NUM_TESTS; test++) {
        pthread_t threads[num_threads[test]];
        ThreadData thread_data[num_threads[test]];

        // Divide numbers between threads
        int numbers_per_thread = (MAX_NUM-1) / num_threads[test];
        for (int i = 0; i < num_threads[test]; i++) {
            thread_data[i].start = i * numbers_per_thread;
            thread_data[i].end = (i+1) * numbers_per_thread;
            if (i == num_threads[test]-1) {
                thread_data[i].end = MAX_NUM-1;  // Last thread gets remaining numbers
            }
        }

        // Reset num_primes
        num_primes = 0;

        // Create threads and calculate parallel execution time
        clock_t start_time = clock();
        for (int i = 0; i < num_threads[test]; i++) {
            pthread_create(&threads[i], NULL, check_numbers, &thread_data[i]);
        }
        for (int i = 0; i < num_threads[test]; i++) {
            pthread_join(threads[i], NULL);
        }
        clock_t end_time = clock();
        double parallel_time = (double)(end_time - start_time) / CLOCKS_PER_SEC;

        // Print results
        printf ("Number of primes: %d\n", num_primes);
        printf("Num threads %d \n", num_threads[test]);
        printf("Parallel time: %.2f\n", parallel_time);
        printf("\n -----------------------\n");
    }

    // Destroy mutex
    pthread_mutex_destroy(&mutex);

    return 0;
}