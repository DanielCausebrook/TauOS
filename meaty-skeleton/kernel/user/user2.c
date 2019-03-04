#define MAIN_FUNC  __attribute__((__section__(".main")))
#include <stdio.h>

void MAIN_FUNC main2() {
    printf("Hello, World! I am process 2!\n");
}

