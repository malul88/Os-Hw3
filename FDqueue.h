#ifndef TEST_FDQUEUE_H
#define TEST_FDQUEUE_H

#include <stdbool.h>
#include "node.h"
/** Type for defining the priority queue */
typedef struct FDQueue_t *FDQueue;

FDQueue fdCreate();

void fdDestroy(FDQueue queue);

int fdGetSize(FDQueue queue);

//bool fdContains(FDQueue queue, int element);

int fdInsert(FDQueue queue, int element, double arrival_time);

int fdRemoveFirst(FDQueue queue);

Node fdPopFirst(FDQueue queue);

int fdRemoveByElement(FDQueue q, int element);

Node fdGetHead(FDQueue queue);

int fdClear(FDQueue queue);

#endif //TEST_FDQUEUE_H
