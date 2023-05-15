#include "main.h"
#include "watek_komunikacyjny.h"
#include "util.h"

void *startKomWatek(void *ptr)
{
  MPI_Status status;
  packet_t pakiet;
  while (stan != InFinish)
  {
    MPI_Recv(&pakiet, 1, MPI_PAKIET_T, MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
    updateProcessClock(pakiet.process, pakiet.timestamp);
    changeClock((pakiet.timestamp > globalLamport ? pakiet.timestamp : globalLamport) + 1);

    switch (status.MPI_TAG)
    {
    case REQUEST:
      processRequest(pakiet);
      sendPacket(createPacket(pakiet.tag), status.MPI_SOURCE, ACK);
      break;
    case ACK:
      ackCount++;
      break;
    case RELEASE:
      processRelease(pakiet);
      break;
    default:
      break;
    }
  }
}
