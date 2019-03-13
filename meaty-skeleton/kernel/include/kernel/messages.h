//
// Created by daniel on 11/03/19.
//

#ifndef KERNEL_MESSAGES_H
#define KERNEL_MESSAGES_H
#include <stddef.h>

struct message {
    int sender;
    void *message;
    size_t  size;
};

int send(void *message, size_t message_size, int dest_pid);
int recieve(struct message *messageblock);

#endif //KERNEL_MESSAGES_H
