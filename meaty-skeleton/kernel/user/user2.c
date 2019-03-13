#define MAIN_FUNC  __attribute__((__section__(".main")))
#include <stdio.h>
#include <kernel/messages.h>
#include <stdlib.h>

void MAIN_FUNC main2() {
    printf("Hello, World! I am process 3!\n");
    struct message *msg = malloc(sizeof(struct message));
    if(recieve(msg)) {
        printf("Recieved \"");
        printf(msg->message);
        printf("\".\n");
    }
}

