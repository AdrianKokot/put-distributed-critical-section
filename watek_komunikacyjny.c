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

    switch (status.MPI_TAG)
    {
    case REQUEST:
      changeClock((pakiet.ts > globalLamport ? pakiet.ts : globalLamport) + 1);
      processRequest(pakiet);
      // debug("Ktoś coś prosi. A niech ma!");
      sendPacket(0, status.MPI_SOURCE, ACK);
      break;
    case ACK:
      // debug("Dostałem ACK od %d, mam już %d", status.MPI_SOURCE, ackCount);
      updateProcessClock(pakiet.src, pakiet.ts);
      ackCount++; /* czy potrzeba tutaj muteksa? Będzie wyścig, czy nie będzie? Zastanówcie się. */
      break;
    case RELEASE:
      processRelease(pakiet);
    default:
      break;
    }
  }
}
