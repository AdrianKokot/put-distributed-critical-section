#include "main.h"
#include "watek_glowny.h"

int allAckOlder()
{
  for (int i = 0; i < size; i++)
  {
    if (i != rank && processesClocks[i] <= processesClocks[rank])
    {
      return FALSE;
    }
  }
  return TRUE;
}

void mainLoop()
{
  srandom(rank);
  int tag;
  int perc;
  packet_t *pkt;

  while (stan != InFinish)
  {
    switch (stan)
    {
    case Start:
      changeState(RequestTool);
      break;
    case SendRelease:
      changeClock(globalLamport + 1);
      updateProcessClock(pkt->process, pkt->timestamp);

      for (int i = 0; i < size; i++)
        if (i != rank)
          sendPacket(pkt, i, RELEASE);
        else
          processRelease(*pkt);

      changeState(pkt->tag == TOOL ? RequestLab : RequestTool);
      free(pkt);
      break;
    case SendRequest:
      changeClock(globalLamport + 1);
      ackCount = 0;
      for (int i = 0; i < size; i++)
      {
        updateProcessClock(pkt->process, pkt->timestamp);

        if (i != rank)
        {
          sendPacket(pkt, i, REQUEST);
        }
        else
        {
          processRequest(*pkt);
        }
      }

      changeState(pkt->tag == TOOL ? WaitingForTool : WaitingForLab);
      free(pkt);
      break;

    case RequestTool:
      perc = random() % 100;

      if (perc < 70)
      {
        println("Ubiegam się o narzędzie");
        changeClock(globalLamport + 1);
        pkt = createPacket(TOOL, rank, globalLamport);
        changeState(SendRequest);
      }
      break;

    case RequestLab:
      perc = random() % 100;

      if (perc < 70)
      {
        println("Ubiegam się o miejsce");
        changeClock(globalLamport + 1);
        pkt = createPacket(LAB, rank, globalLamport);
        changeState(SendRequest);
      }
      break;

    case WaitingForTool:
#ifdef IGNORE_WAIT
#else
      println("Czekam na narzędzie");
#endif
      if (ackCount == size - 1 && canEnterCriticalSection(toolsQueue, tools_number) && allAckOlder())
        changeState(UsingTool);

      break;

    case WaitingForLab:
#ifdef IGNORE_WAIT
#else
      println("Czekam na miejsce");
#endif
      if (ackCount == size - 1 && canEnterCriticalSection(positionsQueue, positions_number) && allAckOlder())
        changeState(UsingLab);

      break;
    case UsingTool:
      println("Używam narzędzia");
      sleep(random() % 10);
      println("Oddaję narzędzie");

      pkt = createPacket(TOOL, rank, globalLamport);
      changeState(SendRelease);
      break;
    case UsingLab:
      println("Używam miejsca");
      sleep(random() % 10);
      println("Oddaję miejsce");

      pkt = createPacket(LAB, rank, globalLamport);
      changeState(SendRelease);
      break;

    default:
      break;
    }
    sleep(SEC_IN_STATE);
  }
}
