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

void *pthread_delayed_release(void *ptr)
{
  srandom(rank);
  println("Odkładam narzędzie do naładowania");
  packet_t *pkt = (packet_t *)ptr;
  sleep(random() % 20);
  pkt->timestamp = getClock();
  for (int i = 0; i < size; i++)
    if (i != rank)
      sendPacket(pkt, i, RELEASE);
    else
      processRelease(*pkt);

  free(pkt);
  println("Narzędzie naładowane");
}

void delayed_release(int tag)
{
  pthread_t tid;
  pthread_create(&tid, NULL, pthread_delayed_release, createPacket(tag));
}

void mainLoop()
{
  srandom(rank);
  int tag;
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
      changeState(pkt->tag == TOOL ? WaitingForTool : WaitingForLab);
      changeClock(globalLamport + 1);
      pkt->timestamp = getClock();
      updateProcessClock(pkt->process, pkt->timestamp);

      ackCount = 0;
      for (int i = 0; i < size; i++)
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

      free(pkt);
      break;

    case RequestTool:
      println("Ubiegam się o sprzęt");
      changeClock(globalLamport + 1);
      pkt = createPacket(TOOL);
      changeState(SendRequest);
      break;

    case RequestLab:
      println("Ubiegam się o miejsce");
      changeClock(globalLamport + 1);
      pkt = createPacket(LAB);
      changeState(SendRequest);
      break;

    case WaitingForTool:

#ifndef IGNORE_WAIT
      println("Czekam na sprzęt");
#endif
      if (ackCount == size - 1 && canEnterCriticalSection(toolsQueue, tools_number) && allAckOlder())
        changeState(UsingTool);

      break;

    case WaitingForLab:
#ifndef IGNORE_WAIT
      println("Czekam na miejsce");
#endif
      if (ackCount == size - 1 && canEnterCriticalSection(positionsQueue, positions_number) && allAckOlder())
        changeState(UsingLab);

      break;
    case UsingTool:
      println("Dostałem sprzęt, krążę po mieście");
      sleep(random() % 10);
      println("Znalazłem kogoś z dobrym nastrojem");

      delayed_release(TOOL);

      changeState(RequestLab);
      break;
    case UsingLab:
      println("Zajmuję miejsce w laboratorium");
      sleep(random() % 10);
      println("Zwalniam miejsce w laboratorium");

      pkt = createPacket(LAB);
      changeState(SendRelease);
      break;

    default:
      break;
    }
    sleep(SEC_IN_STATE);
  }
}
