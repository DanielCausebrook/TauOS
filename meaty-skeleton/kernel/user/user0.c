#define MAIN_FUNC  __attribute__((__section__(".main")))
#include <stdio.h>

void MAIN_FUNC main0() {
    printf("Hello, World! I am process 0!\n");
}
