#define MAIN_FUNC  __attribute__((__section__(".main")))
#include <stdio.h>
#include <kernel/messages.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

struct report {
    uint64_t time;
    uint32_t actionID;
};

int next_log(struct report *log, int *sender) {
    struct message *msg = malloc(sizeof(struct message));
    int recieved = 0;
    if(recieve(msg)) {
        memcpy(log, msg->message, msg->size);
        *sender = msg->sender;
        recieved = 1;
    }
    free(msg);
    return recieved;
}

void MAIN_FUNC main2() {
    printf("MacroSoft Activity log 1.3\n");
    printf("--------------------------\n");

    struct report *log = malloc(sizeof(struct report));

    char *file = malloc(50);

    // Take input from argument, command line, config file etc.
    char *input = "logs/activity.log";
    size_t inputsize = 18;
    memcpy(file, input, inputsize);

    printf("Listening for incoming report messages...\n\n");
    enable_message_passing(sizeof(struct report));

    int sender;

    if(next_log(log, &sender)) {
        printf("Received report from process %d.\n", sender);
        printf("Writing report of action %u to ",log->actionID);
        printf(file);
        printf(".\n");
    } else {
        printf("No messages received.\n");
    }

    free(file);
    free(log);
}
