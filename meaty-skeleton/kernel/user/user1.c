
#define MAIN_FUNC  __attribute__((__section__(".main")))
#include <stdio.h>
#include <kernel/messages.h>

void MAIN_FUNC main1() {
    printf("Hello, World! I am process 2!\n");
    char message[50] = "Test message from process 2!";
    send(message, 50, 3);
}
