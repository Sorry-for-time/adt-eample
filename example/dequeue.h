#undef DATA_TYPE
#define DATA_TYPE(T) _Generic((T), \
  char: sizeof(char), \
  int: sizeof(int), \
  long: sizeof(long), \
  long long: sizeof(long long), \
  long double: sizeof(long double) \
)

#undef DEFAULT_SIZE
#define DEFAULT_SIZE 16

#undef LOAD_FACTOR
#define LOAD_FACTOR 0.75

#undef THRESHOLD
#define THRESHOLD(size) (size*LOAD_FACTOR)

#ifndef ADT_EXAMPLE_DEQUEUE_H

#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>
#include <pthread.h>

#define ADT_EXAMPLE_DEQUEUE_H

typedef struct dequeue_node *restrict DequeueNode;

typedef struct dequeue_node {
    void *restrict data; /* data domain */
    DequeueNode restrict next;
    DequeueNode restrict prev;
} dequeue_node;

typedef struct dequeue_define {
    volatile _Atomic _Bool mutex;
    unsigned int size;
    DequeueNode restrict head;
    DequeueNode restrict rear;
} dequeue_define, *restrict Dequeue;

DequeueNode createQueueNode(void *restrict data);

Dequeue initialDequeue(void);

void add(Dequeue dequeue, DequeueNode dequeueNode);

void releaseDequeue(Dequeue dequeue);

void cleanupQueueNode(DequeueNode restrict dequeueNode);

DequeueNode poll(Dequeue restrict dequeue);

DequeueNode peek(Dequeue restrict dequeue);

void *consumeQueueNode(void *restrict dequeue);

void createQueueTest(void);

#endif //ADT_EXAMPLE_DEQUEUE_H
