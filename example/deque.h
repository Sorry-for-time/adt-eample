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

#ifndef ADT_EXAMPLE_DEQUE_H

#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>
#include <pthread.h>

#define ADT_EXAMPLE_DEQUE_H

typedef struct deque_node *restrict DequeNode;

typedef struct deque_node {
    void *restrict data; /* data domain */
    DequeNode restrict next;
    DequeNode restrict prev;
} deque_node;

typedef struct deque_define {
    volatile _Atomic _Bool mutex;
    unsigned int size;
    DequeNode restrict head;
    DequeNode restrict rear;
} deque_define, *restrict Deque;

DequeNode createDequeNode(void *restrict data);

Deque initialDeque(void);

void add(Deque deque, DequeNode dequeNode);

void releaseDeque(Deque deque);

void cleanupDequeNode(DequeNode restrict dequeNode);

DequeNode poll(Deque restrict deque);

DequeNode peek(Deque restrict deque);

void *consumeDequeNode(void *restrict deque);

void createDequeAndPthreadTest(void);

void TraverseDeque(Deque restrict deque, void(*accept)(DequeNode restrict node));

void testTraverse(void);

#endif //ADT_EXAMPLE_DEQUE_H
