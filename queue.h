#ifndef QUEUEH
#define QUEUEH

typedef struct node
{
  int data;
  int priority;
} node;

typedef struct queue
{
  int size;
  node *array;
} queue;

void queue_insert(queue *, node);
void queue_delete(queue *, int);
void queue_print(queue *);
queue* queue_init(int);

#endif