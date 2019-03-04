
#define MAIN_FUNC  __attribute__((__section__(".main")))
#include <stdio.h>

void MAIN_FUNC main1() {
    printf("Hello, World! I am process 1!\n");
}
