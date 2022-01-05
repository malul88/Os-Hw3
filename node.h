

#ifndef PART3_NODE_H
#define PART3_NODE_H

#include <stdbool.h>
#include <stdlib.h>
#include <sys/time.h>

typedef struct node *Node;

Node nodeCreate(int , double );

bool nodeDestroy(Node node);

Node nodeGetNext(Node node);

Node nodeGetPrev(Node node);

int nodeGetElement(Node node);

double nodeGetArrival(Node node);

bool nodeNewNext(Node node, Node new_next_node);

bool nodeNewPrev(Node node, Node new_prev_node);

#endif //PART3_NODE_H
