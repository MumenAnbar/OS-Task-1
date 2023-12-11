// Mumen Anbar
// 1212297
// Section: 4

#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <pthread.h>
#include <sys/wait.h>

// Declaration of arrays for matrix operations and thread tracking
int matrix1[101][101];                  // Matrix 1: integer 2D array of size 100x100
long long matrix2[101][101];            // Matrix 2: long long integer 2D array of size 100x100
long long ansOfMulForThreads[101][101]; // Result matrix for threaded operations: long long integer 2D array of size 100x100
bool done[101];                         // Array to track completion status for threads: boolean array of size 100



// Function handling tasks for child processes
void childProcess(int p1[][2], int p2[][2], int i) {
    // Close read ends of pipes for parent
    close(p1[i][1]);  // Close the read end of the pipe for the parent
    close(p2[i][0]);  // Close the read end of the pipe for the parent

    // Define interval to be received from parent
    int interval[2];
    // interval[0] represents the starting row index
    // interval[1] represents the ending row index

    // Read the interval for matrix multiplication from the pipe
    if (read(p1[i][0], interval, sizeof(interval)) == -1) {
        printf("Child: An error occurred with reading from the pipe.\n");
        exit(1);
    }

    // Calculate the segment of matrix multiplication based on received interval
    int IntervalOfSegment = interval[1] - interval[0] + 1;
    long long ansOfMulti[IntervalOfSegment + 1][101];

    for (int i = 0; i < IntervalOfSegment; i++) {
        for (int j = 0; j < 100; j++) {
            ansOfMulti[i][j] = 0;
            for (int k = 0; k < 100; k++) {
                // Perform matrix multiplication for the given segment
                ansOfMulti[i][j] += (matrix1[interval[0] + i][k] * matrix2[k][j]);
            }
        }
    }

    // Write the calculated matrix segment to the pipe
    if (write(p2[i][1], ansOfMulti, sizeof(ansOfMulti)) == -1) {
        printf("Child: An error occurred with writing to the pipe.\n");
        exit(1);
    }

    // Close write ends of pipes after writing
    close(p1[i][0]);    // Close the write end of the pipe after writing
    close(p2[i][1]);

    // Terminate the child process
    exit(0);
}

// Thread function for matrix multiplication
void *childThread(void *args){
    int *x = (int *) args;                  // Extract arguments passed to the thread
    int from = x[0], to = x[1], indexOfThread = x[2];  // Assign values from arguments
    for(int i = from; i < to; i++){        // Loop through the specified range of rows
        for(int j = 0; j < 100; j++){      // Loop through columns
            ansOfMulForThreads[i][j] = 0; // Initialize matrix cell to zero
            for(int k = 0; k < 100; k++) {
                // Perform matrix multiplication and accumulate results
                ansOfMulForThreads[i][j] += (matrix1[i][k] * matrix2[k][j]);
            }
        }
    }
    done[indexOfThread] = 1;                // Mark thread completion
}

int main() {

    // Initialize matrix1 with values from the 'id' array in a repeating pattern
    int id[] = {1, 2, 1, 2, 2, 9, 7};   // Array containing values for initialization
    int size1 = 7;                      // Size of the 'id' array
    int cur = 0;                        // Initialize counter variable

    // Loop to assign values to matrix1 using 'id' array values in a repeated sequence
    for(int i = 0; i < 100; i++){           // Loop through rows
        for(int j = 0; j < 100; j++){       // Loop through columns
            matrix1[i][j] = id[cur % size1];// Assign values from 'id' array in a repeating sequence
            cur += 1;                       // Move to the next value in 'id' array
        }
    }

    // Initialize matrix2 with a constant value (2003 * 1212297)
    for(int i = 0; i < 100; i++){           // Loop through rows
        for(int j = 0; j < 100; j++){       // Loop through columns
            matrix2[i][j] = 2003 * 1212297; // Assign a constant value to each cell in matrix2
        }
    }


     /*Brute Force Solution:*/
    /* Brute Force Solution: */
    struct timeval start_time1, end_time1;   // Structures to hold start and end times
    long long elapsed_time1;                 // Variable to store elapsed time

    // Get start time
    if (gettimeofday(&start_time1, NULL) != 0) {   // Get current time (start)
        perror("Error getting start time");
        exit(EXIT_FAILURE);
    }

    // Perform matrix multiplication using brute force approach
    long long ansOfMulti[101][101];        // Matrix to store multiplication results
    for(int i = 0; i < 100; i++){          // Loop through rows
        for(int j = 0; j < 100; j++){      // Loop through columns
            ansOfMulti[i][j] = 0;          // Initialize cell to zero
            for(int k = 0; k < 100; k++){  // Loop for multiplication
                ansOfMulti[i][j] += (matrix1[i][k] * matrix2[k][j]); // Perform multiplication
            }
        }
    }

    // Get end time
    if (gettimeofday(&end_time1, NULL) != 0) {     // Get current time (end)
        perror("Error getting end time");
        exit(EXIT_FAILURE);
    }

    // Calculate and display elapsed time
    elapsed_time1 = ((end_time1.tv_sec - start_time1.tv_sec) * 1000000LL) + (end_time1.tv_usec - start_time1.tv_usec);
    printf("Elapsed Time When Using Native Approach To Find The Answer: %lld Microseconds\n", elapsed_time1);


     /*MultiProcessing Solution:*/
     // Loop for different numbers of processes (from 2 to 32)
    for(int numOfProcesses = 2;numOfProcesses <= 32; numOfProcesses *= 2){
        long long ansOfMulti[101][101];        // Matrix to store multiplication results
        struct timeval start_time, end_time;   // Structures to hold start and end times
        long long elapsed_time;                 // Variable to store elapsed time

        // Get start time
        if (gettimeofday(&start_time, NULL) != 0) {   // Get current time (start)
            perror("Error getting start time");
            exit(EXIT_FAILURE);
        }
        int curNumOfProcesses = numOfProcesses;    // Current number of processes
        int p1[curNumOfProcesses][2];             // File descriptors for the pipe P -> C
        int p2[curNumOfProcesses][2];             // File descriptors for the pipe C -> P
        // p[i][1] is for writing on the pipe
        // p[i][0] is for reading from the pipe
        pthread_t thread[curNumOfProcesses];       // Thread identifiers
        int segLength = 100 / curNumOfProcesses;   // Length of each segment for processing

        int curInterval[] = {0, segLength};   // Initialize interval for matrix segment
        // Loop to create pipes and fork processes
        for(int i = 0; i < curNumOfProcesses; i++){
            // Create pipes for communication between parent and child processes
            if (pipe(p1[i]) == -1 || pipe(p2[i]) == -1) {
                perror("Pipe creation failed\n");
                exit(1);
            }

            pid_t pid1 = fork();    // Create a child process using fork()

            if (pid1 == -1) {
                printf("Fork creation failed!\n");
                exit(1);
            }
            else if (pid1 == 0) {
                // This is the child process
                childProcess(p1, p2, i);   // Execute child process function
            }
            else {
                // This is the Parent Process
                close(p1[i][0]);    // Close the read end of the pipe for the parent
                close(p2[i][1]);    // Close the write end of the pipe for the parent

                // Write the message (interval) to the pipe
                if(write(p1[i][1], curInterval, sizeof(curInterval)) == -1){
                    printf("Parent: An error occurred with writing to the pipe.\n");
                    exit(1);
                }

                // Update interval values for the next child process
                if(i == curNumOfProcesses - 2){
                    curInterval[0] = curInterval[1];
                    curInterval[1] = 100;
                }
                else {
                    curInterval[0] = curInterval[1];
                    curInterval[1] = curInterval[0] + segLength;
                }
                close(p1[i][1]);    // Close the write end of the pipe after writing
            }
        }

        // Wait for all child processes to complete
        while(wait(NULL) != -1);

        // Reset interval for subsequent iterations
        curInterval[0] = 0;
        curInterval[1] = segLength;

        // Loop through the processes to gather results from pipes
        for(int i = 0; i < curNumOfProcesses; i++){
            int curLen = abs(curInterval[1] - curInterval[0]);    // Calculate current interval length
            long long toReceive[curLen][101];                     // Array to receive data from pipes

            // Read data from the pipe into the 'toReceive' array
            if(read(p2[i][0], toReceive, sizeof(toReceive)) == -1){
                printf("Parent: An error occurred with reading from the pipe.\n");
                exit(1);
            }

            // Copy received data into 'ansOfMulti' matrix based on current interval
            for(int i = 0; i < curLen; i++){
                for(int j = 0; j < 100; j++){
                    ansOfMulti[i + curInterval[0]][j] = toReceive[i][j];
                }
            }

            // Update interval values for the next set of data from child processes
            if(i == curNumOfProcesses - 2){
                curInterval[0] = curInterval[1];
                curInterval[1] = 100;
            }
            else{
                curInterval[0] = curInterval[1];
                curInterval[1] = curInterval[0] + segLength;
            }
        }

        // Calculate elapsed time and display for the current number of processes
        if (gettimeofday(&end_time, NULL) != 0) {
            perror("Error getting end time");
            exit(EXIT_FAILURE);
        }
        elapsed_time = ((end_time.tv_sec - start_time.tv_sec) * 1000000LL) + (end_time.tv_usec - start_time.tv_usec);

        printf("Elapsed Time When Using %d Processes To Find The Answer: %lld Microseconds\n",curNumOfProcesses, elapsed_time);

    }

     /* Threads Section */
    /* Joining Threads */
    // Loop for different numbers of threads (from 2 to 32)
    for(int numOfThreads = 2; numOfThreads <= 32; numOfThreads *= 2){
        struct timeval startOfThread, endOfThread;    // Structures to hold start and end times
        long long TimeForThread;                      // Variable to store elapsed time

        // Get start time
        if (gettimeofday(&startOfThread, NULL) != 0) {    // Get current time (start)
            perror("Error getting start time");
            exit(EXIT_FAILURE);
        }

        int curNumOfThreads = numOfThreads;            // Current number of threads
        pthread_t thread[curNumOfThreads];             // Thread identifiers
        int segLength = 100 / numOfThreads;            // Length of each segment for processing
        int from = 0, to = segLength;                  // Initialize segment bounds

        // Loop to create and execute threads
        for(int i = 0; i < curNumOfThreads; i++){
            int *curInterval;
            curInterval = (int *) malloc(3 * sizeof(int));   // Allocate memory for current interval
            curInterval[0] = from;                           // Set start of the interval
            curInterval[1] = to;                             // Set end of the interval
            curInterval[2] = i;                              // Set index of the interval

            // Create threads and pass interval data to the childThread function
            if(pthread_create(&thread[i], NULL, &childThread, curInterval)){
                printf("Thread creation failed.\n");
                exit(1);
            }

            // Update interval bounds for the next thread
            if(i == curNumOfThreads - 2){
                from = to;
                to = 100;
            }
            else if(i < curNumOfThreads - 2){
                from = to;
                to = from + segLength;
            }
        }

        // Wait for all threads to finish
        for(int i = 0; i < curNumOfThreads; i++){
            if(pthread_join(thread[i], NULL)){
                printf("Thread join failed.\n");
                exit(1);
            }
        }

        // Get end time
        if (gettimeofday(&endOfThread, NULL) != 0) {    // Get current time (end)
            perror("Error getting end time");
            exit(EXIT_FAILURE);
        }

        // Calculate and display elapsed time for the current number of threads
        TimeForThread = ((endOfThread.tv_sec - startOfThread.tv_sec) * 1000000LL) + (endOfThread.tv_usec - startOfThread.tv_usec);
        printf("Elapsed time when using %d Joinable Threads: %lld microseconds\n", numOfThreads, TimeForThread);
    }



    // Loop for different numbers of detached threads (from 2 to 32)
    for(int numOfThreads = 2; numOfThreads <= 32; numOfThreads *= 2){

        // Initialize the 'done' array elements to 0
        for(int i = 0; i < numOfThreads; i++) done[i] = 0;

        struct timeval StartOfDetached, endOfDetached; // Structures to hold start and end times
        long long elapsedTimeDetached;                 // Variable to store elapsed time

        // Get start time
        if (gettimeofday(&StartOfDetached, NULL) != 0) {
            perror("Error getting start time");
            exit(EXIT_FAILURE);
        }

        pthread_attr_t detached_Thread;    // Thread attribute for detached threads
        pthread_attr_init(&detached_Thread);    // Initialize thread attributes
        pthread_attr_setdetachstate(&detached_Thread, PTHREAD_CREATE_DETACHED); // Set detached state

        int curNumOfThreads = numOfThreads;    // Current number of threads
        pthread_t detachedThread[curNumOfThreads]; // Thread identifiers for detached threads
        int segLength = 100 / numOfThreads;    // Length of each segment for processing
        int from = 0, to = segLength;          // Initialize segment bounds

        // Loop to create and execute detached threads
        for(int i = 0; i < curNumOfThreads; i++){
            int *curInterval;
            curInterval = (int *) malloc(3 * sizeof(int));   // Allocate memory for current interval
            curInterval[0] = from;                           // Set start of the interval
            curInterval[1] = to;                             // Set end of the interval
            curInterval[2] = i;                              // Set index of the interval

            // Create detached threads and pass interval data to the childThread function
            if(pthread_create(&detachedThread[i], &detached_Thread , &childThread, curInterval)){
                printf("Thread creation failed.\n");
                exit(1);
            }

            // Update interval bounds for the next detached thread
            if(i == curNumOfThreads - 2){
                from = to;
                to = 100;
            }
            else if(i < curNumOfThreads - 2){
                from = to;
                to = from + segLength;
            }
        }

        bool ok = 1; // Flag to track completion of all detached threads
        // Wait until all detached threads are done processing
        while(true){
            for(int i = 0; i < numOfThreads; i++) ok &= done[i]; // Check if all threads are done
            if(ok) break; // If all threads are done, exit the loop
            else ok = 1;  // Reset the flag for the next iteration
        }

        pthread_attr_destroy(&detached_Thread); // Destroy thread attributes

        // Get end time
        if (gettimeofday(&endOfDetached, NULL) != 0) {
            perror("Error getting end time");
            exit(EXIT_FAILURE);
        }

        // Calculate and display elapsed time for the current number of detached threads
        elapsedTimeDetached = ((endOfDetached.tv_sec - StartOfDetached.tv_sec) * 1000000LL) + (endOfDetached.tv_usec - StartOfDetached.tv_usec);
        printf("Elapsed Time When Using %d Detached Threads To Find The Answer: %lld Microseconds\n", numOfThreads, elapsedTimeDetached);
    }

    return 0;
}
