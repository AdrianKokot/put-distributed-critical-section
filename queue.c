#include <stdio.h>
#include "queue.h"

// Internal functions
void swap(node *a, node *b)
{
  node temp = *b;
  *b = *a;
  *a = temp;
}

void heapify(queue *queue, int index)
{
  if (queue->size < 2)
    return;

  int smallest = index,
      left = 2 * index + 1,
      right = 2 * index + 2;

  if (left < queue->size && (queue->array[left].priority < queue->array[smallest].priority || (queue->array[left].priority == queue->array[smallest].priority && queue->array[left].data < queue->array[smallest].data)))
    smallest = left;

  if (right < queue->size && (queue->array[right].priority < queue->array[smallest].priority || (queue->array[right].priority == queue->array[smallest].priority && queue->array[right].data < queue->array[smallest].data)))
    smallest = right;

  if (smallest != index)
  {
#ifdef DEBUG

    printf("Before swap\n");
    queue_print(queue);

#endif
    swap(&queue->array[index], &queue->array[smallest]);
#ifdef DEBUG

    printf("After swap\n");
    queue_print(queue);
#endif

    heapify(queue, smallest);
#ifdef DEBUG

    printf("After heapify\n");
    queue_print(queue);
#endif
  }
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
  for (int i = queue->size / 2 - 1; i >= 0; i--)
  {
    heapify(queue, i);
  }
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
  for (int i = queue->size / 2 - 1; i >= 0; i--)
  {
    heapify(queue, i);
  }

  queue_delete(queue, data);
}

void queue_print(queue *queue)
{
#ifdef DEBUG
  for (int i = 0; i < queue->size; ++i)
    printf("%d ", queue->array[i].priority);
  printf("\n");
  for (int i = 0; i < queue->size; ++i)
    printf("%d ", queue->array[i].data);
  printf("\n");
#endif
}