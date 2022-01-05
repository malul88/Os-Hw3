
#include "node.h"

struct node{
    int element ;
    Node next ;
    Node prev ;
};

Node nodeCreate(int element){
    Node node = malloc(sizeof(*node));
    if (node == NULL){
        return NULL;
    }
    node->element = element;
    node -> next = NULL;
    node -> prev = NULL;

    return node;
}

bool nodeDestroy(Node node){
    if (node == NULL) {
        return 0;
    }
    free(node);
    return 1;
}

Node nodeGetNext(Node node){
    if (node == NULL){
        return NULL;
    }
    return node -> next;
}

Node nodeGetPrev(Node node){
    if (node == NULL){
        return NULL;
    }
    return node -> prev;
}

int nodeGetElement(Node node){
    if (node == NULL){
        return -1;
    }
    return node -> element;
}


bool nodeNewNext(Node node, Node new_next_node){
    if (node == NULL){
        return 0;
    }
    node -> next = new_next_node;
    return 1;
}

bool nodeNewPrev(Node node, Node new_prev_node){
    if (node == NULL){
        return 0;
    }
    node -> prev = new_prev_node;
    return 1;
}

