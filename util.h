#ifndef UTILH
#define UTILH
#include "main.h"
#include "queue.h"

typedef struct
{
  int ts;
  int src;
  int data;
} packet_t;
#define NITEMS 3

#define ACK 1
#define REQUEST 2
#define RELEASE 3
#define APP_PKT 4
#define FINISH 5

extern MPI_Datatype MPI_PAKIET_T;
void inicjuj_typ_pakietu();

void sendPacket(packet_t *pkt, int destination, int tag);

typedef enum
{
  InRun,
  InMonitor,
  InWant,
  InSection,
  InFinish
} state_t;
extern state_t stan;
extern pthread_mutex_t stateMut;

void changeState(state_t);
void changeClock(int);
void updateProcessClock(int, int);

void processRequest(packet_t packet);
void processRelease(packet_t packet);
int canEnterCriticalSection();
#endif
