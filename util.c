#include "main.h"
#include "util.h"
MPI_Datatype MPI_PAKIET_T;

state_t stan = Start;

pthread_mutex_t stateMut = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t lamportMut = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t processesClocksMut = PTHREAD_MUTEX_INITIALIZER;

pthread_mutex_t toolsQueueMut = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t positionsQueueMut = PTHREAD_MUTEX_INITIALIZER;

struct tagNames_t
{
  const char *name;
  int tag;
} tagNames[] = {
    {"ACK", ACK},
    {"REQUEST", REQUEST},
    {"RELEASE", RELEASE}};

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
  offsets[0] = offsetof(packet_t, timestamp);
  offsets[1] = offsetof(packet_t, process);
  offsets[2] = offsetof(packet_t, tag);

  MPI_Type_create_struct(NITEMS, blocklengths, offsets, typy, &MPI_PAKIET_T);

  MPI_Type_commit(&MPI_PAKIET_T);
}

void sendPacket(packet_t *pkt, int destination, int tag)
{
  MPI_Send(pkt, 1, MPI_PAKIET_T, destination, tag, MPI_COMM_WORLD);
  debug("Wysyłam %s do %d\n", tag2string(tag), destination);
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
  printProcessesClocks();
  pthread_mutex_unlock(&processesClocksMut);
}

void processRequest(packet_t packet)
{
  node newNode = {packet.process, packet.timestamp};

  if (packet.tag == TOOL)
  {
    pthread_mutex_lock(&toolsQueueMut);
    queue_insert(toolsQueue, newNode);
    debug("ToolsQueue insert by %d", rank);
    queue_print(toolsQueue);
    pthread_mutex_unlock(&toolsQueueMut);
  }
  else
  {
    pthread_mutex_lock(&positionsQueueMut);
    queue_insert(positionsQueue, newNode);
    debug("PositionsQueue insert by %d", rank);
    queue_print(positionsQueue);
    pthread_mutex_unlock(&positionsQueueMut);
  }
}

void processRelease(packet_t packet)
{
  if (packet.tag == TOOL)
  {
    pthread_mutex_lock(&toolsQueueMut);
    queue_delete(toolsQueue, packet.process);
    debug("ToolsQueue release by %d", rank);
    queue_print(toolsQueue);
    pthread_mutex_unlock(&toolsQueueMut);
  }
  else
  {
    pthread_mutex_lock(&positionsQueueMut);
    queue_delete(positionsQueue, packet.process);
    debug("PositionsQueue release by %d", rank);
    queue_print(positionsQueue);
    pthread_mutex_unlock(&positionsQueueMut);
  }
};

int canEnterCriticalSection(queue *queue, int first_n_allowed)
{
  if (queue->size == 0)
    return FALSE;

  for (int i = 0; i < first_n_allowed && i < queue->size; i++)
  {
    if (queue->array[i].data == rank)
      return TRUE;
  }

  return FALSE;
}

/// @brief Create new packet
/// @param tag
/// @param process
/// @param timestamp
/// @return
packet_t *createDetailedPacket(int tag, int process, int timestamp)
{
  packet_t *pkt = malloc(sizeof(packet_t));
  pkt->tag = tag;
  pkt->process = process;
  pkt->timestamp = timestamp;
  return pkt;
}

/// @brief Create new packet
/// @param tag
/// @return
packet_t *createPacket(int tag)
{
  return createDetailedPacket(tag, rank, globalLamport);
}
