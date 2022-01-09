#include "FDqueue.h"
#include <malloc.h>

struct FDQueue_t{
    int size ;
    Node head ;
    Node end ;
};

FDQueue fdCreate(){

    FDQueue queue = malloc(sizeof(*queue));
    if (queue == NULL){
        return NULL;
    }

    queue -> size = 0;
    queue -> head = NULL;
    queue -> end = NULL;
    return queue;
}

void fdDestroy(FDQueue queue){
    fdClear(queue);
    free(queue);
}

int fdGetSize(FDQueue queue){
    return queue->size;
}

int fdInsert(FDQueue queue, int element, double arrival_time){
    if (queue == NULL){
        return -1;
    }
    Node node = nodeCreate(element, arrival_time);
    if (node == NULL){
        return -1;
    }
    if ((queue -> size) == 0){
        queue -> head = node;
        queue -> end = node;
    }else {
        Node temp = queue->head;
        while (nodeGetNext(temp) != NULL){
            temp = nodeGetNext(temp);
        }
        nodeNewNext(temp,node);
        nodeNewPrev(node, temp);
        queue -> end = node;
    }
    queue -> size = (queue -> size) + 1;
    return 0;
}

int fdRemoveFirst(FDQueue queue){
    if (!queue || queue->head == NULL){
        return -1;
    }
    Node temp = queue->head;
    queue->head = nodeGetNext(temp);
    nodeNewPrev(queue->head,NULL);
    free(temp);
    (queue->size)--;
    return 0;

}
Node fdPopFirst(FDQueue queue){
    if (!queue || queue->head == NULL){
        return NULL;
    }
    Node temp = queue->head;
    queue->head = nodeGetNext(temp);
    nodeNewPrev(queue->head,NULL);
    (queue->size)--;
    return temp;
}

int fdClear(FDQueue queue){
    Node on_run = queue->head;
    while (on_run){
        Node temp = on_run;
        on_run = nodeGetNext(on_run);
        free(temp);
    }
    queue->size = 0;
    queue->head = NULL;
    return 0;
}

int fdRemoveByElement(FDQueue q, int element){
    Node temp = q->head;
    if (!temp){
        printf("list is empty\n");
        return -1;
    }
    if (q->size == 1 && nodeGetElement(q->head) == element){
        free(temp);
        q->head = NULL;
        q->size -= 1;
        return 0;
    } else if (q->size == 1 && nodeGetElement(q->head) != element){
        printf("list size == 1 but head->element not correct\n");
        return -1;
    }else if (nodeGetElement(q->head) == element){
        Node new_head = nodeGetNext(q->head);
        free(temp);
        q->head = new_head;
        nodeNewPrev(q->head, NULL);
        q->size--;
        return 0;
    } else {
        while (temp != NULL){
            if (nodeGetElement(temp) == element){
                break;
            }
            temp = nodeGetNext(temp);
        }
        if (temp == NULL){
            printf("not found in the list \n");
            return -1;
        }
        if (nodeGetPrev(temp)){
            nodeNewNext(nodeGetPrev(temp),nodeGetNext(temp));
        }
        if(nodeGetNext(temp)){
            nodeNewPrev(nodeGetNext(temp), nodeGetPrev(temp));
        }
        free(temp);
        q->size -= 1;
    }
    return 0;
}

Node fdGetHead(FDQueue queue){
    if (!queue){
        return NULL;
    }
    return queue->head;
}
