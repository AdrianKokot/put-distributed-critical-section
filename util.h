#ifndef UTILH
#define UTILH
#include "main.h"
#include "queue.h"

typedef struct
{
  int timestamp;
  int process;
  int tag;
} packet_t;
#define NITEMS 3

#define TOOL 100
#define LAB 101

#define ACK 1
#define REQUEST 2
#define RELEASE 3

extern MPI_Datatype MPI_PAKIET_T;
void inicjuj_typ_pakietu();

void sendPacket(packet_t *pkt, int destination, int tag);

typedef enum
{
  Start,
  InFinish,
  RequestTool,
  UsingTool,
  RequestLab,
  UsingLab,
  SendRequest,
  WaitingForLab,
  WaitingForTool,
  SendRelease

} state_t;
extern state_t stan;
extern pthread_mutex_t stateMut;

void changeState(state_t);
void changeClock(int);
void updateProcessClock(int, int);

void processRequest(packet_t packet);
void processRelease(packet_t packet);
int canEnterCriticalSection();

packet_t *createPacket(int tag);
#endif
