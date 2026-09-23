#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <sys/time.h>
#include <unistd.h>



void *sayHello(void* args) {
    printf("Hello, from the new thread!\n");
    return NULL;
}


int main(int argc, char **argv) {
    pthread_t   thread_id;
    pthread_create(&thread_id, NULL, sayHello, NULL);
    pthread_join(thread_id, NULL);
    printf("Thread finished!\n");
}