//Kyle Schultz

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <sys/types.h>
#include <sys/time.h>
#include <unistd.h>
#include <sys/syscall.h>
#include <stdbool.h>

#define MAX_INPUT_FILES 10
#define MAX_RESOLVER_THREADS 10
#define MAX_REQUESTER_THREADS 5
#define MAX_NAME_LENGTH 1025
#define MAX_IP_LENGTH INET6_ADDRSTRLEN


typedef struct args{
	pthread_mutex_t* the_mutex;
	char buffer[50][MAX_NAME_LENGTH];
	char** files;
	int file_index;
	int buffer_size;
	int requester_done;
	FILE* output;
	FILE * request_output;
	int num_files;
	pthread_cond_t* condc;
	pthread_cond_t* condp;
}args;

void* request(void* ptr);

void* resolve(void* ptr);