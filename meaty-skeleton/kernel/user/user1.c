
#define MAIN_FUNC  __attribute__((__section__(".main")))
#include <stdio.h>
#include <kernel/messages.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

void MAIN_FUNC main1() {
    printf("Malicious PrOceS5\n");
    printf("-----------------\n");
    printf("Executing buffer overflow attack on MacroSoft.\n");
    printf("Attempting to send payload to logger...");

    struct overflow {
        uint64_t time;
        uint32_t actionID;

        // OVERFLOW!
        int32_t heap_pointer_a;
        uint8_t heap_flags;
        char injectFile[50];
    };

    struct overflow *attack = malloc(sizeof(struct overflow));

    char *malFile = "../../admin/important.conf";
    memcpy(attack->injectFile, malFile, 27);

    attack->heap_pointer_a = 0x90000005 + sizeof(struct overflow);
    attack->heap_flags = 0x1;

    attack->time = 0x7777777777777777;
    attack->actionID = 0x77777777;

    if(send(attack, sizeof(struct overflow), 3)) printf("Success!\n");
    else printf("Failed.\n");
}
