#include "main.h"
#include "watek_glowny.h"

void request(int tag)
{
  changeClock(globalLamport + 1);
  packet_t *pkt = createPacket(tag);

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
}

void release(int tag)
{
  changeClock(globalLamport + 1);
  packet_t *pkt = createPacket(tag);

  for (int i = 0; i < size; i++)
  {
    if (i != rank)
    {
      sendPacket(pkt, i, RELEASE);
    }
    else
    {
      processRelease(*pkt);
    }
  }

  free(pkt);
}

void *pthread_delayed_release(void *ptr)
{
  int *tag = ((int *)ptr);
  srandom(rank);
  println("Odkładam narzędzie do naładowania");

  sleep(random() % 20 + 5);

  release(*tag);
  println("Narzędzie naładowane");
  free(tag);
}

void delayed_release(int tag)
{
  pthread_t tid;
  int *i = malloc(sizeof(int));
  *i = tag;
  pthread_create(&tid, NULL, pthread_delayed_release, i);
}

void mainLoop()
{
  srandom(rank);
  int tag;
  packet_t *pkt;

  while (stan != Finish)
  {
    switch (stan)
    {
    case Start:
      changeState(RequestTool);
      break;
    case RequestTool:
      println("Ubiegam się o sprzęt");
      request(TOOL);
      changeState(WaitingForTool);
      break;

    case RequestLab:
      println("Ubiegam się o miejsce");
      request(LAB);
      changeState(WaitingForLab);
      break;

    case WaitingForTool:
#ifndef IGNORE_WAIT
      println("Czekam na sprzęt");
#endif
      if (canEnterCriticalSection(toolsQueue, toolsNumber))
        changeState(UsingTool);

      break;

    case WaitingForLab:
#ifndef IGNORE_WAIT
      println("Czekam na miejsce");
#endif
      if (canEnterCriticalSection(positionsQueue, positionsNumber))
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

      release(LAB);
      changeState(RequestTool);
      break;

    default:
      break;
    }
    sleep(SEC_IN_STATE);
  }
}
