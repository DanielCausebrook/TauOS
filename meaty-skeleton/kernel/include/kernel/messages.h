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
void enable_message_passing(size_t size);
void enable_message_passing_pid(int pid, size_t size);

#endif //KERNEL_MESSAGES_H
