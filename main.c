#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <pthread.h>

#define MAX_THREADS 32

// Global variables shared by threads
long long int a, b;                     // Range of numbers to compute the square root sum
int c, d;                               // Number of threads and method identifier
double global_sqrt_sum = 0;             // Accumulated square root sum across threads
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;  // Mutex for thread synchronization

// Structure to hold thread arguments
struct ThreadArgs {
    long long int start;                 // Start of the range for a thread
    long long int end;                   // End of the range for a thread
    double local_sqrt_sum;               // Local sum for each thread (used in Method 3)
};

// Function executed by each thread to compute the square root sum
void* computeSum(void* arg) {
    struct ThreadArgs* args = (struct ThreadArgs*)arg;

    // Compute the square root sum for the specified range
    for (long long int x = args->start; x <= args->end; x++) {
        double sqrt_x = sqrt(x);

        // Synchronize the update of the global sum based on the selected method
        switch (d) {
            case 1: // Method 1: No synchronization
                global_sqrt_sum += sqrt_x;
                break;

            case 2: // Method 2: Synchronize with a mutex
                pthread_mutex_lock(&mutex);
                global_sqrt_sum += sqrt_x;
                pthread_mutex_unlock(&mutex);
                break;

            case 3: // Method 3: Use local variable and synchronize with a mutex
                pthread_mutex_lock(&mutex);
                args->local_sqrt_sum += sqrt_x;
                pthread_mutex_unlock(&mutex);
                break;

            default:
                break;
        }
    }


    // If using Method 3, add the local sum to the global sum after the thread completes
    if (d == 3) {
        pthread_mutex_lock(&mutex);
        global_sqrt_sum += args->local_sqrt_sum;
        pthread_mutex_unlock(&mutex);
    }

    pthread_exit(NULL);
}

int main(int argc, char* argv[]) {
    // Check for the correct number of command-line arguments
    if (argc != 5) {
        fprintf(stderr, "Usage: %s <a> <b> <c> <d>\n", argv[0]);
        return EXIT_FAILURE;
    }

    // Parse command-line arguments
    a = atoll(argv[1]);
    b = atoll(argv[2]);
    c = atoi(argv[3]);
    d = atoi(argv[4]);

    // Declare arrays to store thread IDs and thread arguments
    pthread_t threads[MAX_THREADS];
    struct ThreadArgs threadArgs[MAX_THREADS];

    // Calculate the range and determine the number of threads and chunk size
    long long int range = b - a + 1;
    int threads_count = (c > 0 && c <= MAX_THREADS) ? c : 1;
    int chunk_size = range / threads_count;

    // Create threads and assign ranges to each thread
    for (int i = 0; i < threads_count; i++) {
        threadArgs[i].start = a + i * chunk_size;
        threadArgs[i].end = (i == threads_count - 1) ? b : threadArgs[i].start + chunk_size - 1;
        threadArgs[i].local_sqrt_sum = 0; // Initialize local_sqrt_sum for Method 3

        pthread_create(&threads[i], NULL, computeSum, (void*)&threadArgs[i]);
    }

    // Wait for all threads to complete
    for (int i = 0; i < threads_count; i++) {
        pthread_join(threads[i], NULL);

    }

    // Print the final result
    printf("Sum: %.5e\n", global_sqrt_sum);

    return EXIT_SUCCESS;
}
