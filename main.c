// import libraries
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <math.h>

// Global variables shared by threads
double global_sqrt_sum = 0; // Accumulated square root sum across threads

long long int a;  // Range of numbers to compute the square root sum
long long int b;
int c;
int d;

// Mutex for thread synchronization
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;


void calculateSqrtSum(long long int a, long long int b);

// Function executed by each thread to compute the square root sum - Method 1
void *computeSum1(void *arg)
{
    long long int x;
    int threadID = *((int *)arg);
    // Split the range into equal parts
    long long int start = a + (b - a) * threadID / c;
    long long int end = a + (b - a) * (threadID + 1) / c - 1;

    // Compute the square root sum for the specified range
    for (x = start; x <= end; x++)
    {
        // Method 1: No synchronization
        global_sqrt_sum += sqrt(x);
    }
    pthread_exit(NULL);
}

// Function executed by each thread to compute the square root sum - Method 2
void *computeSum2(void *arg)
{
    long long int x;
    int threadID = *((int *)arg);
    // Split the range into equal parts
    long long int start = a + (b - a) * threadID / c;
    long long int end = a + (b - a) * (threadID + 1) / c - 1;

    // Compute the square root sum for the specified range
    for (x = start; x <= end; x++)
    {
        // Method 2: Synchronize with a mutex
        pthread_mutex_lock(&mutex); // Lock the mutex
        global_sqrt_sum += sqrt(x);
        pthread_mutex_unlock(&mutex); // Unlock the mutex
    }
    pthread_exit(NULL);
}

// Function executed by each thread to compute the square root sum - Method 3
void *computeSum3(void *arg)
{
    long long int x;
    double local_sqrt_sum = 0;
    int threadID = *((int *)arg);
    // Split the range into equal parts
    long long int start = a + (b - a) * threadID / c;
    long long int end = a + (b - a) * (threadID + 1) / c - 1;

    // Compute the square root sum for the specified range
    for (x = start; x <= end; x++)
    {
        // Method 3: Use local variable and synchronize with a mutex
        local_sqrt_sum += sqrt(x);
    }

    // If using Method 3, add the local sum to the global sum after the thread completes
    pthread_mutex_lock(&mutex); // Lock the mutex
    global_sqrt_sum += local_sqrt_sum;
    pthread_mutex_unlock(&mutex);   // Unlock the mutex

    pthread_exit(NULL);
}

void main(int argc, char *argv[])
{
    // Check for the correct number of command-line arguments
    if (argc != 5) {
        fprintf(stderr, "Usage: %s <a> <b> <c> <d>\n", argv[0]);
        exit(1);
    }

    // Parse command-line arguments
    a = atoll(argv[1]);
    b = atoll(argv[2]);
    c = atoi(argv[3]);
    d = atoi(argv[4]);


    // Declare arrays to store thread IDs and thread arguments
    pthread_t threads[c];
    int threadsIDs[c];

    // Determine which method to use
    if (d == 1)
    {
        // Create threads method 1
        int i;
        for (i = 0; i < c; i++)
        {
            threadsIDs[i] = i;
            pthread_create(&threads[i], NULL, computeSum1, &threadsIDs[i]);
        }
    }
    else if (d == 2)
    {
        // Create threads method 2
        int i;
        for (i = 0; i < c; i++)
        {
            threadsIDs[i] = i;
            pthread_create(&threads[i], NULL, computeSum2, &threadsIDs[i]);
        }
    }
    else if (d == 3)
    {
        // Create threads method 3
        int i;
        for (i = 0; i < c; i++)
        {
            threadsIDs[i] = i;
            pthread_create(&threads[i], NULL, computeSum3, &threadsIDs[i]);
        }
    }

    // Wait for all threads to complete
    for (int i = 0; i < c; i++)
    {
        pthread_join(threads[i], NULL);
    }

    printf("Global Sqrt Sum: %.5e\n", global_sqrt_sum);

    pthread_mutex_destroy(&mutex); // Destroy the mutex
}
