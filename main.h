#ifndef MAINH
#define MAINH
#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <unistd.h>
#include <string.h>
#include <pthread.h>

#include "util.h"
#define TRUE 1
#define FALSE 0
#define SEC_IN_STATE 1
#define STATE_CHANGE_PROB 10

#define ROOT 0

extern int rank;
extern int size;
extern int tools_number;
extern int positions_number;
extern int ackCount;
extern int globalLamport;
extern pthread_t threadKom;
extern int *processesClocks;
extern queue *toolsQueue;

#ifdef DEBUG
#define debug(FORMAT,...) printf("[%d] %c[%d;%dm [%d]: " FORMAT "%c[%d;%dm\n",  globalLamport, 27, (1+(rank/7))%2, 31+(6+rank)%7, rank, ##__VA_ARGS__, 27,0,37);
#else
#define debug(...) ;
#endif

#define println(FORMAT,...) printf("[%d] %c[%d;%dm [%d]: " FORMAT "%c[%d;%dm\n",  globalLamport, 27, (1+(rank/7))%2, 31+(6+rank)%7, rank, ##__VA_ARGS__, 27,0,37);

#endif
