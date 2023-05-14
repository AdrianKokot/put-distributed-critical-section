#include "main.h"
#include "watek_komunikacyjny.h"
#include "util.h"

void *startKomWatek(void *ptr)
{
  MPI_Status status;
  int is_message = FALSE;
  packet_t pakiet;
  while (stan != InFinish)
  {
    // debug("czekam na recv");
    MPI_Recv(&pakiet, 1, MPI_PAKIET_T, MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
    updateProcessClock(pakiet.process, pakiet.timestamp);
    changeClock((pakiet.timestamp > globalLamport ? pakiet.timestamp : globalLamport) + 1);

    switch (status.MPI_TAG)
    {
    case REQUEST:
      processRequest(pakiet);
      // debug("Ktoś coś prosi. A niech ma!");
      sendPacket(createPacket(pakiet.tag, rank, globalLamport), status.MPI_SOURCE, ACK);
      break;
    case ACK:
      // debug("Dostałem ACK od %d, mam już %d", status.MPI_SOURCE, ackCount);
      ackCount++; /* czy potrzeba tutaj muteksa? Będzie wyścig, czy nie będzie? Zastanówcie się. */
      break;
    case RELEASE:
      processRelease(pakiet);
    default:
      break;
    }
  }
}
