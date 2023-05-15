#include <stdio.h>
#include <stdlib.h>
#include "queue.h"

// Internal functions
void swap(node *a, node *b)
{
  node temp = *b;
  *b = *a;
  *a = temp;
}

void queue_bubble_sort(queue *queue)
{
  int i, j;
  for (i = 0; i < queue->size - 1; i++)
    for (j = 0; j < queue->size - i - 1; j++)
      if (queue->array[j].priority > queue->array[j + 1].priority || (queue->array[j].priority == queue->array[j + 1].priority && queue->array[j].data > queue->array[j + 1].data))
        swap(&queue->array[j], &queue->array[j + 1]);
}

// Interface functions

/// @brief Add node to priority queue
/// @param queue
/// @param node
void queue_insert(queue *queue, node node)
{
  if (queue->size == 0)
  {
    queue->array[0] = node;
    queue->size += 1;
    return;
  }

  queue->array[queue->size] = node;
  queue->size += 1;
  queue_bubble_sort(queue);
}

/// @brief Delete node with given data from priority queue
/// @param queue
/// @param data
void queue_delete(queue *queue, int data)
{
  int i;
  for (i = 0; i < queue->size; i++)
  {
    if (data == queue->array[i].data)
      break;
  }

  if (i == queue->size)
  {
    return;
  }

  swap(&queue->array[i], &queue->array[queue->size - 1]);
  queue->size -= 1;
  queue_bubble_sort(queue);
}

void queue_print(queue *queue)
{
#ifdef QUEUE_PRINT
  printf("-----------------------------\n");
  for (int i = 0; i < queue->size; ++i)
    printf("%d ", queue->array[i].priority);
  printf("\n");
  for (int i = 0; i < queue->size; ++i)
    printf("%d ", queue->array[i].data);
  printf("\n");
#endif
}

/// @brief Create new priority queue with internal array size of size
/// @param size
/// @return
queue *queue_init(int size)
{
  queue *q;
  q = malloc(sizeof(queue));
  q->size = 0;
  q->array = malloc(size * sizeof(node));

  return q;
}