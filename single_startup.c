#include <stdio.h>
#include <pthread.h>
#include <unistd.h>

#include "egd.h"

void* sendWrapper(void* threadId);
void* recvWrapper(void* threadid);

int main(int argc, char *argv[])
{
    pthread_t sender, receiver;
    int rc = 0;
    void* threadRet;
    
    printf("Creating receiver.\n");
    rc = pthread_create(&receiver, NULL, recvWrapper, NULL);
    if (rc) {
        printf("Coudln't create receiver thread, terminating.\n");
        return -1;
    }
    sleep(1);
    printf("Creating sender.\n");
    rc = pthread_create(&sender, NULL, sendWrapper, NULL);
    if (rc) {
        printf("Coudln't create sender thread, terminating.\n");
        return -2;
    }

    pthread_join(sender, &threadRet);
    printf("Sender exited with return code: %lu\n", (size_t)threadRet);
    pthread_join(receiver, &threadRet);
    printf("Receiver exited with return code: %lu\n", (size_t)threadRet);
    
    printf("Exitting.\n");
    return 0;
}


void* sendWrapper(void* threadId) {
    size_t rc = gefSockSend();
    pthread_exit((void *) rc);
}

void* recvWrapper(void* threadid) {
    size_t rc = gefSockReceive();
    pthread_exit((void *) rc);
}
