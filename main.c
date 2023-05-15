#include "main.h"
#include "watek_glowny.h"
#include "watek_komunikacyjny.h"

int rank, size, tools_number, positions_number;
int ackCount = 0;
int globalLamport = 0;
int *processesClocks;
queue *toolsQueue;
queue *positionsQueue;

pthread_t threadKom;

void finalizuj()
{
  pthread_mutex_destroy(&stateMut);
  println("czekam na wątek \"komunikacyjny\"\n");
  pthread_join(threadKom, NULL);
  MPI_Type_free(&MPI_PAKIET_T);
  MPI_Finalize();
}

void check_thread_support(int provided)
{
  printf("THREAD SUPPORT: chcemy %d. Co otrzymamy?\n", provided);
  switch (provided)
  {
  case MPI_THREAD_SINGLE:
    printf("Brak wsparcia dla wątków, kończę\n");
    fprintf(stderr, "Brak wystarczającego wsparcia dla wątków - wychodzę!\n");
    MPI_Finalize();
    exit(-1);
    break;
  case MPI_THREAD_FUNNELED:
    printf("tylko te wątki, ktore wykonaly mpi_init_thread mogą wykonać wołania do biblioteki mpi\n");
    break;
  case MPI_THREAD_SERIALIZED:
    printf("tylko jeden watek naraz może wykonać wołania do biblioteki MPI\n");
    break;
  case MPI_THREAD_MULTIPLE:
    printf("Pełne wsparcie dla wątków\n");
    break;
  default:
    printf("Nikt nic nie wie\n");
  }
}

int main(int argc, char **argv)
{
  if (argc < 3)
  {
    printf("Usage: %s tools_number positions_number\n", argv[0]);
    exit(1);
  }

  tools_number = atoi(argv[1]);
  positions_number = atoi(argv[2]);

  MPI_Status status;
  int provided;
  MPI_Init_thread(&argc, &argv, MPI_THREAD_MULTIPLE, &provided);
  check_thread_support(provided);
  inicjuj_typ_pakietu();
  MPI_Comm_size(MPI_COMM_WORLD, &size);
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);
  srand(rank);

  if (rank == 0)
  {
    printf("====================================\n");
    printf("Liczba kradziejów:\t%d\nIlość sprzętu:\t\t%d\nLiczba miejsc:\t\t%d\n", size, tools_number, positions_number);
    printf("====================================\n");
  }
  processesClocks = malloc(size * sizeof(int));
  toolsQueue = queue_init(size);
  positionsQueue = queue_init(size);
  for (int i = 0; i < size; i++)
  {
    processesClocks[i] = 0;
  }
  pthread_create(&threadKom, NULL, startKomWatek, 0);
  mainLoop();
  finalizuj();
  return 0;
}
