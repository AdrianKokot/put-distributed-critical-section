#include "main.h"
#include "util.h"
MPI_Datatype MPI_PAKIET_T;

state_t stan = InRun;

pthread_mutex_t stateMut = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t lamportMut = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t processesClocksMut = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t queueMut = PTHREAD_MUTEX_INITIALIZER;

struct tagNames_t
{
  const char *name;
  int tag;
} tagNames[] = {
    {"pakiet aplikacyjny", APP_PKT},
    {"finish", FINISH},
    {"potwierdzenie", ACK},
    {"prośbę o sekcję krytyczną", REQUEST},
    {"zwolnienie sekcji krytycznej", RELEASE}};

const char *const tag2string(int tag)
{
  for (int i = 0; i < sizeof(tagNames) / sizeof(struct tagNames_t); i++)
  {
    if (tagNames[i].tag == tag)
      return tagNames[i].name;
  }
  return "<unknown>";
}

void inicjuj_typ_pakietu()
{
  int blocklengths[NITEMS] = {1, 1, 1};
  MPI_Datatype typy[NITEMS] = {MPI_INT, MPI_INT, MPI_INT};

  MPI_Aint offsets[NITEMS];
  offsets[0] = offsetof(packet_t, ts);
  offsets[1] = offsetof(packet_t, src);
  offsets[2] = offsetof(packet_t, data);

  MPI_Type_create_struct(NITEMS, blocklengths, offsets, typy, &MPI_PAKIET_T);

  MPI_Type_commit(&MPI_PAKIET_T);
}

void sendPacket(packet_t *pkt, int destination, int tag)
{
  // changeClock(globalLamport + 1);
  int freepkt = 0;
  if (pkt == 0)
  {
    pkt = malloc(sizeof(packet_t));
    freepkt = 1;
  }
  pkt->src = rank;
  pkt->ts = globalLamport;
  MPI_Send(pkt, 1, MPI_PAKIET_T, destination, tag, MPI_COMM_WORLD);
  debug("Wysyłam %s do %d\n", tag2string(tag), destination);
  if (freepkt)
    free(pkt);
}

void changeState(state_t newState)
{
  pthread_mutex_lock(&stateMut);
  if (stan == InFinish)
  {
    pthread_mutex_unlock(&stateMut);
    return;
  }
  stan = newState;
  pthread_mutex_unlock(&stateMut);
}

void changeClock(int newLamport)
{
  pthread_mutex_lock(&lamportMut);
  globalLamport = newLamport;
  pthread_mutex_unlock(&lamportMut);
}

void printProcessesClocks()
{
  #ifdef DEBUG
  printf("printProcessesClocks by %d\n", rank);
  for (int i = 0; i < size; i++)
  {
    printf("%s ", i == rank ? "I" : "-");
  }
  printf("\n");
  for (int i = 0; i < size; i++)
  {
    printf("%d ", processesClocks[i]);
  }
  printf("\n");
  #endif
}

void updateProcessClock(int process, int newClock)
{
  pthread_mutex_lock(&processesClocksMut);
  processesClocks[process] = newClock;
  processesClocks[rank] = globalLamport;
  printProcessesClocks();
  pthread_mutex_unlock(&processesClocksMut);
}

extern int q_size;
void processRequest(packet_t packet)
{
  pthread_mutex_lock(&queueMut);
  node newNode = {packet.src, packet.ts};
  insert(queue, newNode);
  pthread_mutex_unlock(&queueMut);
  printArray(queue, q_size);
}

void processRelease(packet_t packet)
{
  pthread_mutex_lock(&queueMut);
  deleteRoot(queue, packet.src);
  pthread_mutex_unlock(&queueMut);
  printArray(queue, q_size);
};

int canEnterCriticalSection()
{
  return q_size > 0 && queue[0].data == rank;
}