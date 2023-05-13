#include "main.h"
#include "watek_glowny.h"

int allAckOlder() {
  // for (int i = 0; i < size; i++) {
  //   if (i != rank && processesClocks[i] < processesClocks[rank]) {
  //     return FALSE;
  //   }
  // }
  return TRUE;
}

void mainLoop()
{
  srandom(rank);
  int tag;
  int perc;

  while (stan != InFinish)
  {
    switch (stan)
    {
    case InRun:
      perc = random() % 100;
      if (perc < 25)
      {
        // debug("Perc: %d", perc);
        println("Ubiegam się o sekcję krytyczną");
        changeClock(globalLamport + 1);

        // debug("Zmieniam stan na wysyłanie");
        packet_t *pkt = malloc(sizeof(packet_t));
        pkt->data = perc;
        pkt->src = rank;
        pkt->ts = globalLamport;
        ackCount = 0;
        for (int i = 0; i <= size - 1; i++)
        {
          if (i != rank)
          {
            sendPacket(pkt, i, REQUEST);
          }
          else
          {
            processRequest(*pkt);
          }
        }
        changeState(InWant);
        free(pkt);
      }
      // debug("Skończyłem myśleć");
      break;
    case InWant:
      #ifdef IGNORE_WAIT
      #else
      println("Czekam na wejście do sekcji krytycznej");
      #endif
      if (ackCount == size - 1 && canEnterCriticalSection() && allAckOlder())
        changeState(InSection);
      break;
    case InSection:
      // tutaj zapewne jakiś muteks albo zmienna warunkowa
      println("Jestem w sekcji krytycznej");
      sleep(5);
      // if ( perc < 25 ) {
      // debug("Perc: %d", perc);
      println("Wychodzę z sekcji krytycznej");
      // debug("Zmieniam stan na wysyłanie");
      packet_t *pkt = malloc(sizeof(packet_t));
      pkt->data = perc;
      pkt->src = rank;
      pkt->ts = globalLamport;
      for (int i = 0; i <= size - 1; i++)
        if (i != rank)
          sendPacket(pkt, (rank + 1) % size, RELEASE);
        else
          processRelease(*pkt);

      changeState(InRun);
      free(pkt);
      //}
      break;
    default:
      break;
    }
    sleep(SEC_IN_STATE);
  }
}
