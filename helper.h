/******************************************************************
 * Header file for the helper functions. This file includes the
 * required header files, as well as the function signatures and
 * the semaphore values (which are to be changed as needed).
 ******************************************************************/


# include <stdio.h>
# include <stdlib.h>
# include <unistd.h>
# include <sys/types.h>
# include <sys/ipc.h>
# include <sys/shm.h>
# include <sys/sem.h>
# include <sys/time.h>
# include <math.h>
# include <errno.h>
# include <string.h>
# include <pthread.h>
# include <ctype.h>
# include <iostream>
#include<cstdlib>
#include<time.h>
using namespace std;
#define mutex 0
#define space 1
#define item 2
#define output 3
# define SEM_KEY 0x01139563 // Change this number as needed
union semun {
    int val;               /* used for SETVAL only */
    struct semid_ds *buf;  /* used for IPC_STAT and IPC_SET */
    ushort *array;         /* used for GETALL and SETALL */
};

int check_arg (char *);
int sem_create (key_t, int);
int sem_init (int, int, int);
void sem_wait (int, short unsigned int);
void sem_signal (int, short unsigned int);
int sem_close (int);
void initialize_parameter(int**& queue, int &size_queue,int &num_job,int &num_consumer,int &num_producer,int& jobID, int& jobc_id,int*& producer_id,int *& consumer_id, int argc,char** argv);
int initialise_semaphore(int);
int rand_between(int min, int max);
int  sem_wait_time (int , short unsigned int);
void clean_up(int**& queue, int*& producer_id,int*& consumer_id,int size_queue);
