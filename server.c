#include "FDqueue.h"
#include "segel.h"
#include "request.h"


#define SCHED_MAX 10

// 
// server.c: A very, very simple web server
//
// To run:
//  ./server <portnum (above 2000)>
//
// Repeatedly handles HTTP requests sent to this port number.
// Most of the work is done within routines written in request.c
//

// HW3: Parse the new arguments too
int wait_size = 0;
int handle_size = 0;
bool to_listen = false;
void getargs(int *port, int argc, char *argv[])
{
    if (argc < 2) {
	fprintf(stderr, "Usage: %s <port>\n", argv[0]);
	exit(1);
    }
    *port = atoi(argv[1]);
}

void* handle_t(void **args) {

    int fd;
    pthread_mutex_t *mutex_lock = args[0];
    pthread_mutex_t *mutex_lock2 = args[1];
    pthread_cond_t *exist = (pthread_cond_t *) args[2];
    pthread_cond_t *finish = (pthread_cond_t *) args[3];
    FDQueue wait_q = args[4];
    FDQueue handle_q = args[5];

    while (1) {
        pthread_mutex_lock(mutex_lock);
        while (wait_size == 0) {       // Wait for requests to arrive.
            pthread_cond_wait(exist, mutex_lock);
        }
        // Pop the first request in the queue and update .
        Node temp = fdPopFirst(wait_q);
        wait_size--;
        handle_size++;
        fd = nodeGetElement(temp);
        fdInsert(handle_q, fd);
        free(temp);
        pthread_mutex_unlock(mutex_lock);
        // Handle the request and then close it's fd.
        requestHandle(fd);
        close(fd);
        // Update the queue of handling.
        pthread_mutex_lock(mutex_lock2);
        handle_size--;
        fdRemoveByElement(handle_q, fd);
        pthread_cond_signal(finish);
        pthread_mutex_unlock(mutex_lock2);
    }
}

void handleOverload(char* method, FDQueue wait_q, pthread_mutex_t *mutex, pthread_cond_t* finished, int confd){
    int half = wait_size/2;
    if (strcmp(method, "block")){ //wait until someone finished and there's a place to enqueue
        pthread_cond_wait(finished, mutex);
        return;
    } else if (strcmp(method, "dh")){ // Drop-head
        fdPopFirst(wait_q);
        wait_size--;
        return;
    } else if (strcmp(method, "random")){
        Node temp = fdGetHead(wait_q);
        while (temp != NULL && wait_size > half){
            if(rand()%2){
                if (fdRemoveByElement(wait_q, nodeGetElement(temp) == -1)){
                    return;
                }
            }
            temp = nodeGetNext(temp);
            if (temp == NULL){
                temp = fdGetHead(wait_q);
            }
        }
        return;
    } else if (strcmp(method, "dt")){
        close(confd);
        to_listen = true;
        return;
    } else {
        printf("error : sched_alg %s is unknown\n",method);
    }
}




int main(int argc, char *argv[])
{
    int listenfd, connfd, port, clientlen, num_of_threads, queue_size, current_size;
    char sched_alg[SCHED_MAX];
    struct sockaddr_in clientaddr;
    pthread_cond_t exist;
    pthread_cond_t finished;
    pthread_mutex_t mutex_lock;
    pthread_mutex_t mutex_lock2;
    pthread_cond_init(&exist,NULL);
    pthread_cond_init(&finished,NULL);
    pthread_mutex_init(&mutex_lock, NULL);
    pthread_mutex_init(&mutex_lock2, NULL);
    FDQueue wait_q = fdCreate();
    FDQueue handle_q = fdCreate();
    if (argc < 5) {
        fprintf(stderr, "Usage: %s <port> <threads> <queue_size> <sched_alg>\n", argv[0]);
        exit(1);
    }
    num_of_threads = atoi(argv[2]);
    queue_size = atoi(argv[3]);
    strcpy(sched_alg, argv[4]);
    void* args[6];
    args[0] = &mutex_lock;
    args[1] = &mutex_lock2;
    args[2] = &exist;
    args[3] = &finished;
    args[4] = wait_q;
    args[5] = handle_q;

    getargs(&port, argc, argv);
    // Creating pool of threads
    for (int i = 0; i < num_of_threads; ++i) {
        pthread_t p;
        pthread_create(&p, NULL, (void *(*)(void *)) handle_t, args);
    }

    listenfd = Open_listenfd(port);
    while (1) {
	    clientlen = sizeof(clientaddr);

        LISTEN: connfd = Accept(listenfd, (SA *)&clientaddr, (socklen_t *) &clientlen);
        pthread_mutex_lock(&mutex_lock);
        current_size = handle_size + wait_size;  // Update current size of buffers.
        while (current_size >= queue_size){   // If number of request reach the maximum, then wait.
            handleOverload(sched_alg, wait_q, &mutex_lock, &finished, connfd);
            current_size = handle_size + wait_size;
            if (to_listen){
                to_listen = false;
                goto LISTEN;
            }
        }
        pthread_mutex_unlock(&mutex_lock);

        pthread_mutex_lock(&mutex_lock);
        fdInsert(wait_q, connfd);
        wait_size++;
        pthread_cond_signal(&exist);
        pthread_mutex_unlock(&mutex_lock);
    }
}






 
