#define MAIN_FUNC  __attribute__((__section__(".main")))
#include <kernel/stdio.h>
#include <stdio.h>
#include <stdint.h>
#include <kernel/messages.h>

void MAIN_FUNC main0() {
    printf("Arbitrary safe process 2.3\n");
    printf("--------------------------\n");
    printf("Sending report of action 81 to logger...");

    struct report {
        uint64_t time;
        uint32_t actionID;
    };

    struct report log;

    log.actionID = 81;
    log.time = 7302175877;

    if(send(&log, sizeof(struct report), 3)) printf("Success!\n");
    else printf("Failed.\n");

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wmissing-noreturn"
//    while(1) {
//        char in[1];
//        if(read(0, &in, 1)) {
//            char str[2] = {
//                    in[0],
//                    '\0'
//            };
//            printf(str);
//            printf(" key.\n");
//        }
//    }
#pragma clang diagnostic pop
}