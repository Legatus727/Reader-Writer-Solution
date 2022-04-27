#include <stdio.h>
#include <pthread.h>
#include <semaphore.h>
#include <stdlib.h>
#include <time.h>
#include <errno.h>

// These are the globals shared by all threads, including main
#define RANGE 1000000000
#define BASE 500000000
int rICrange = RANGE;
int rICbase = BASE;
int rOOCrange = RANGE;
int rOOCbase = BASE;
int wICrange = RANGE;
int wICbase = BASE;
int wOOCrange = RANGE;
int wOOCbase = BASE;
int keepgoing = 1;
int totalReads = 0;
int totalWrites = 0;

void threadSleep(int range, int base);
void *readers(void *args);
void *writers(void *args);

// The global area must include semaphore declarations and
// declarations of any state variables (reader counts,
// total number of readers and writers).
sem_t read_mutex;
sem_t write_mutex;
int numReader = 0;
int totalReads = 0;
int totalWrites = 0;

int main(int argc, char **argv) {
    
    int numRThreads = 0;
    int numWThreads = 0;
    
    if (argc == 11) {
        rICrange = atoi(argv[1]);
        rICbase = atoi(argv[2]);
        rOOCrange = atoi(argv[3]);
        rOOCbase = atoi(argv[4]);
        wICrange = atoi(argv[5]);
        wICbase = atoi(argv[6]);
        wOOCrange = atoi(argv[7]);
        wOOCbase = atoi(argv[8]);
        numRThreads = atoi(argv[9]);
        numWThreads = atoi(argv[10]);
    }
    else {
        printf("Usage: %s <reader in critical section sleep range> <reader in
            critical section sleep base> \n\t <reader out of critical section sleep range> <reader out
            of critical section sleep base> \n\t <writer in critical section sleep range> <writer in
            critical section sleep base> \n\t <writer out of critical section sleep range> <writer
            out of critical section sleep base> \n\t <number of readers> <number of writers>\n",
            argv[0]);
        exit(-1);
    }
    
    // Add declarations for pthread arrays, one for reader threads and
    // one for writer threads.
    pthread_t read[numRThreads], write[numWThreads];
    
    // Add declarations for arrays for reader and writer thread identities. As in the
    // dining philosopher problem, arrays of int are used.
    int rID[numRThreads];
    for (int i = 0; i < numRThreads; i++) {
        rID[i] = i;
    }
    int wID[numWThreads];
    for (int i = 0; i < numWThreads; i++) {
        wID[i] = i;
    }
    
    // Add code to initialize the binary semaphores used by the readers and writers.
    sem_init(&read_mutex, 0, 1);
    sem_init(&write_mutex, 0, 1);

    // Add a for loop to create numRThread reader threads.
    for (int i = 0; i < numRThreads; i++) {
        pthread_create(&read[i], NULL, (void *)readers, (void *)&rID[i]);
    }
    
    // Add a for loop to create numWThread writer threads.
    for (int i = 0; i < numWThreads; i++) {
        pthread_create(&write[i], NULL, (void *)writers, (void *)&wID[i]);
    }

    // These statements wait for the user to type a character and press
    // the Enter key. Then, keepgoing will be set to 0, which will cause
    // the reader and writer threads to quit.
    char buf[256];
    scanf("%s", &buf);
    keepgoing = 0;
    
    // Add two for loops using pthread_join in order to wait for the reader
    // and writer threads to quit.
    for (int i = 0; i < numRThreads; i++) {
        pthread_join(read[i], NULL);
    }
    for (int i = 0; i < numWThreads; i++) {
        pthread_join(write[i], NULL);
    }
    
    printf("Total number of reads: %d\nTotal number of writes: %d\n",
        totalReads, totalWrites);
}

// Use this function to sleep within the threads
void threadSleep(int range, int base) {
    struct timespec t;
    t.tv_sec = 0;
    t.tv_nsec = (rand() % range) + base;
    nanosleep(&t, 0);
}

void *readers(void *args) {
    int id = *((int *) args);
    threadSleep(rOOCrange, rOOCbase);
    while (keepgoing) {

        // Add code for each reader to enter the
        // reading area.
        // The totalReads variable must be
        // incremented just before entering the
        // reader area. However, you must use mutual exclusion
        // on the increment operation to prevent race conditions
        // for the increment.
        sem_wait(&read_mutex);
        numReader++;
        if (numReader == 1) {
            sem_wait(&write_mutex); // Block writer when first reader enters queue
        }
        totalReads++;

        printf("Reader %d starting to read\n", id);

        threadSleep(rICrange, rICbase);

        
        printf("Reader %d finishing reading\n", id);
        
        // Add code for each reader to leave the
        // reading area.
        numReader--;
        if (numReader == 0) {
            sem_post(&write_mutex); // Last reader wakes the writer
        }
        sem_post(&read_mutex);
        
        threadSleep(rOOCrange, rOOCbase);
    }
    
    printf("Reader %d quitting\n", id);
}

void *writers(void *args) {
    int id = *((int *) args);
    
    threadSleep(wOOCrange, wOOCbase);
    while (keepgoing) {
        // Add code for each writer to enter
        // the writing area.
        sem_wait(&write_mutex);
        totalWrites++;

        printf("Writer %d starting to write\n", id);

        
        threadSleep(wICrange, wICbase);
        
        printf("Writer %d finishing writing\n", id);
        
        // Add code for each writer to leave
        // the writing area.
        sem_post(&write_mutex);
        
        threadSleep(wOOCrange, wOOCbase);
    }
    
    printf("Writer %d quitting\n", id);
}