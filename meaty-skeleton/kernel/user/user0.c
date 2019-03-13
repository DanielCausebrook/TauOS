#define MAIN_FUNC  __attribute__((__section__(".main")))
#include <kernel/stdio.h>
#include <stdio.h>

void MAIN_FUNC main0() {
    printf("Hello, World! I am process 0!\n");
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wmissing-noreturn"
    while(1) {
        char in[1];
        if(read(0, &in, 1)) {
            char str[2] = {
                    in[0],
                    '\0'
            };
            printf(str);
            printf(" key.\n");
        }
    }
#pragma clang diagnostic pop
}