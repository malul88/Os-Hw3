

#ifndef PART3_NODE_H
#define PART3_NODE_H

#include <stdbool.h>
#include <stdlib.h>

typedef struct node *Node;

Node nodeCreate(int element);

bool nodeDestroy(Node node);

Node nodeGetNext(Node node);

Node nodeGetPrev(Node node);

int nodeGetElement(Node node);

bool nodeNewNext(Node node, Node new_next_node);

bool nodeNewPrev(Node node, Node new_prev_node);

#endif //PART3_NODE_H
