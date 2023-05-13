// Priority Queue implementation in C

#include <stdio.h>
#include "queue.h"
#include "util.h"
extern int rank;

int q_size = 0;
void swap(node *a, node *b)
{
  node temp = *b;
  *b = *a;
  *a = temp;
}

// Function to heapify the tree
void heapify(node array[], int q_size, int i)
{
  if (q_size == 1)
  {
    printf("Single element in the heap");
  }
  else
  {
    // Find the largest among root, left child and right child
    int smallest = i;
    int l = 2 * i + 1;
    int r = 2 * i + 2;
    if (l < q_size && (array[l].priority < array[smallest].priority || (array[l].priority == array[smallest].priority && array[l].data < array[smallest].data)))
      smallest = l;
    if (r < q_size && (array[r].priority < array[smallest].priority || (array[r].priority == array[smallest].priority && array[r].data < array[smallest].data)))
      smallest = r;

    // Swap and continue heapifying if root is not smallest
    if (smallest != i)
    {
      swap(&array[i], &array[smallest]);
      heapify(array, q_size, smallest);
    }
  }
}

// Function to insert an element into the tree
void insert(node array[], node newNode)
{
  if (q_size == 0)
  {
    array[0] = newNode;
    q_size += 1;
  }
  else
  {
    array[q_size] = newNode;
    q_size += 1;
    for (int i = q_size / 2 - 1; i >= 0; i--)
    {
      heapify(array, q_size, i);
    }
  }
}

// Function to delete an element from the tree
void deleteRoot(node array[], int data)
{
  int i;
  for (i = 0; i < q_size; i++)
  {
    if (data == array[i].data)
      break;
  }

  if (i == q_size)
  {
    return;
  }

  swap(&array[i], &array[q_size - 1]);
  q_size -= 1;
  for (int i = q_size / 2 - 1; i >= 0; i--)
  {
    heapify(array, q_size, i);
  }

  deleteRoot(array, data);
}

// Print the array
void printArray(node array[], int q_size)
{
#ifdef DEBUG

  printf("queue by %d\n", rank);
  for (int i = 0; i < q_size; ++i)
    printf("%d ", array[i].priority);
  printf("\n");
  for (int i = 0; i < q_size; ++i)
    printf("%d ", array[i].data);
  printf("\n");
#endif
}
