// Developer: Alex St.Aubin
// Program: The Bakery Algorithm
// Description: The Bakery Algorithm is one of the simplest known solutions
//                to the mutual exclusion problem for the general case of n
//                processes. The basic idea is that each process has a variable
//                that indicates the position of that process in a hypothetical
//                queue of all the processes. Each process in this virtual queue
//                scans the variables of the other processes, and enters the
//                critical section only upon determining that it is at the head
//                of the queue.

#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <unistd.h>

// Global Variables:
int* entering; // fake boolean
int* tickets;
int resource;
int threadCount;

// Global Constants:
const int FALSE = 0;
const int TRUE = 1;
const int THD_MIN = 1;
const int THD_MAX = 256;

// Function: getThreadCount(int, char*)
// Description: Reads command line arguments for number of threads.
// Parameters: Argument count, argument vector.
// Returns: Pass - entered thread count
//          Fail - -1
int getThreadCount(int argc, char* argv[]){
	// Print usage message.
	if(argc == 1){
		printf("Error: must provide thread count.\n");
		printf("Usage: ./bakery <threadCount>\n");
		return -1;
	}
	// Error if not exactly 2 arguments.
	if(argc != 2){
		printf("Error: invalid command line arguments.\n");
		return -1;
	}
	// Convert form ASCII to Integer.
	int tmp = atoi(argv[1]);
	if(tmp < THD_MIN || tmp > THD_MAX){
		printf("Error: invalid thread count.\n");
		return -1;
	}
	return tmp;
}

// Function: lock(int)
// Description: Implements a pseudo-mutex lock.
// Parameters: Thread trying to enter the critical section.
// Returns: N/A
void lock(int i){
	// Take a ticket.
	entering[i] = TRUE;
	int max = 0;
	int j;
	for(j = 0; j < threadCount; j++)
		if(tickets[j] > max)
			max = tickets[j];

	tickets[i] = 1 + max;
	entering[i] = FALSE;
	// Test for available critical section.
	for(j = 0; j < threadCount; j++){
		// Wait until thread j receives its number
		while(entering[j]){
			// Do nothing.
		}
		// Wait until all threads with smaller numbers or with
		//  same number, but with higher priority finish.
		while(((tickets[j] != 0) && (tickets[j] < tickets[i])) || (tickets[j] == tickets[i] && j < i)){
			// Do nothing.
		}
	}
}

// Function: unlock(int)
// Description: Sets the passed index in the tickets[] array to 0.
// Parameters: Index of ticket to zero out.
// Returns: N/A
void unlock(int index){
	// Zero out ticket.
	tickets[index] = 0;
}

// Function: useResource(int)
// Description: Passed thread uses the resource variable.
// Parameters: Thread to use resource.
// Returns: N/A
void useResource(int x){
	// Error if resource is in use.
	if(resource != -1){
		printf("Error: resource acquired by %d but still in use by %d\n", x, resource);
		exit(0);
	}
	// Use resource for 1 second.
	resource = x;
	printf("Thread %d using resource...\n", x);
	sleep(1);
	resource = -1;
}

// Function: thdFunction()
// Description: Sends one thread to the bakery.
// Parameters: Thread number to run.
// Returns: Thread number.
void *thdFunction(void* x){
	int tmp = (int)x;
	lock(tmp);
	useResource(tmp);
	unlock(tmp);
	return x;
}

// Function: main(int, char*)
// Description: Tests the bakery algorithm.
// Parameters: Command line arguments.
// Returns: Terminated program.
int main(int argc, char* argv[]){
	// Get thread count.
	threadCount = getThreadCount(argc, argv);
	if(threadCount == -1){
		exit(1);
	}
	// Dynamically allocate the global variables
	entering = malloc(sizeof(int) * threadCount);
	tickets = malloc(sizeof(int) * threadCount);
	// Initialize global arrays/variables based on thread count.
	int i;
	for(i = 0; i < threadCount; i++){
		entering[i] =  FALSE;
		tickets[i] = 0;
	}
	resource = -1;
	// Create the appropriate number of threads.
	pthread_t* threads = malloc(threadCount * sizeof(pthread_t));
	for(i = 0; i < threadCount; i++)
		pthread_create(&threads[i], NULL, &thdFunction, (void*)((long)i));
	// Wait for the appropriate number of threads to complete.
	for(i = 0; i < threadCount; i++)
		pthread_join(threads[i], NULL);
	exit(1);
}