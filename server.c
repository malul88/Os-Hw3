#include "FDqueue.h"
#include "segel.h"
#include "request.h"
#include <assert.h>



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
int wait_size;
int handle_size;
bool to_listen;
int j;
pthread_cond_t exist;
pthread_cond_t finished;
pthread_mutex_t mutex_lock;
pthread_mutex_t mutex_lock2;
pthread_mutex_t mutex3;
FDQueue wait_q;
//FDQueue handle_q;


void getargs(int *port, int argc, char *argv[])
{
    if (argc < 2) {
	fprintf(stderr, "Usage: %s <port>\n", argv[0]);
	exit(1);
    }
    *port = atoi(argv[1]);
}

double TimeGetSeconds() {
    struct timeval t;
    int rc = gettimeofday(&t, NULL);
    assert(rc == 0);
    return (double) ((double)t.tv_sec + (double)t.tv_usec / 1e6);
}
void* handle_t(struct stats **args) {
    int fd;
    pthread_mutex_lock(&mutex3);
    int id = j;
    j++;
    pthread_mutex_unlock(&mutex3);
    struct stats *stat = args[id];
    stat->handler_thread_stats_t.handler_thread_id = id;

    while (1) {
        pthread_mutex_lock(&mutex_lock);
        while (wait_size == 0) {       // Wait for requests to arrive.
            pthread_cond_wait(&exist, &mutex_lock);
        }
        // Pop the first request in the queue and update .
        Node temp = fdPopFirst(wait_q);
        double time_picked = TimeGetSeconds();
        wait_size--;
        handle_size++;
        fd = nodeGetElement(temp);
        assert(fd != -1);
        //int rc = fdInsert(handle_q, fd, time_picked);
        //assert(rc != -1);
        pthread_mutex_unlock(&mutex_lock);
        // Handle the request and then close it's fd.
        stat->arrival_time = nodeGetArrival(temp);
        stat->dispatch_interval = time_picked - stat->arrival_time;
        (stat->handler_thread_stats_t.handler_thread_req_count)++;
        free(temp);
        requestHandle(fd, stat);
        close(fd);
        // Update the queue of handling.
        pthread_mutex_lock(&mutex_lock);
        handle_size--;
        //rc = fdRemoveByElement(handle_q, fd);
        //printf("returned %d from remove by element = %d\n", rc, fd);
        pthread_cond_signal(&finished);
        pthread_mutex_unlock(&mutex_lock);
    }
}

void handleOverload(char* method, int confd){
    int half = wait_size/2;
    if (strcmp("block", method) == 0){ //wait until someone finished and there's a place to enqueue;
        pthread_cond_wait(&finished, &mutex_lock);
        return;
    } else if (strcmp("dh", method) == 0){ // Drop-head
        Node rc = fdPopFirst(wait_q);
        if (rc){
            close(nodeGetElement(rc));
        }
        wait_size--;
        return;
    } else if (strcmp(method, "random") == 0){
        Node temp = fdGetHead(wait_q);
        while (temp != NULL && wait_size > half){
            //sleep(2);
            if(rand()%2){
                close(nodeGetElement(temp));
                if (fdRemoveByElement(wait_q, nodeGetElement(temp)) == -1){
                    temp = nodeGetNext(temp);
                    continue;
                } else {
                    wait_size--;
                }
            }
            temp = nodeGetNext(temp);
            if (temp == NULL){
                temp = fdGetHead(wait_q);
            }
        }
        return;
    } else if (strcmp(method, "dt") == 0){
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
    wait_q = fdCreate();
    pthread_cond_init(&exist,NULL);
    pthread_cond_init(&finished,NULL);
    pthread_mutex_init(&mutex_lock, NULL);
    pthread_mutex_init(&mutex_lock2, NULL);
    pthread_mutex_init(&mutex3, NULL);
    wait_size = 0;
    handle_size = 0;
    j = 0;
    to_listen = false;
    //handle_q = fdCreate();

    if (argc < 5) {
        fprintf(stderr, "Usage: %s <port> <threads> <queue_size> <sched_alg>\n", argv[0]);
        exit(1);
    }
    num_of_threads = atoi(argv[2]);
    queue_size = atoi(argv[3]);
    strcpy(sched_alg, argv[4]);

    getargs(&port, argc, argv);
    // Creating pool of threads

    struct stats* args[num_of_threads];
    for (int i = 0; i < num_of_threads; ++i) {
        struct stats* stats1 = statsCreate();
        args[i] = stats1;
        pthread_t p;
        pthread_create(&p, NULL, (void *(*)(void *)) handle_t, args);
    }

    listenfd = Open_listenfd(port);
    while (1) {
	    clientlen = sizeof(clientaddr);

        LISTEN:
        connfd = Accept(listenfd, (SA *)&clientaddr, (socklen_t *) &clientlen);

        double arrival_time = TimeGetSeconds();
        pthread_mutex_lock(&mutex_lock);
        current_size = handle_size + wait_size;  // Update current size of buffers.
        while (current_size >= queue_size){   // If number of request reach the maximum, then wait.
            handleOverload(sched_alg, connfd);
            current_size = handle_size + wait_size;
            if (to_listen){
                to_listen = false;
                pthread_mutex_unlock(&mutex_lock);
                goto LISTEN;
            }
        }
//        pthread_mutex_unlock(&mutex_lock);
//
//        pthread_mutex_lock(&mutex_lock);
        int res = fdInsert(wait_q, connfd, arrival_time);
        assert(res == 0);
        wait_size++;
        pthread_cond_signal(&exist);
        pthread_mutex_unlock(&mutex_lock);
    }
}






 
