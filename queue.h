#ifndef QUEUEH
#define QUEUEH

struct node
{
  int data;
  int priority;
};

typedef struct node node;

void insert(node array[], node newNode);

void deleteRoot(node array[], int data);

void printArray(node array[], int q_size);

#endif