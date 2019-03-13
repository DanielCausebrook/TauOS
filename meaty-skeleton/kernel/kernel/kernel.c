#include <stdio.h>
#include <stdint.h>

#include <kernel/multiboot.h>
#include <kernel/tty.h>
#include <kernel/system.h>
#include <kernel/memlocs.h>
#include <stdlib.h>

//TODO Add to header file somewhere. Mentioned at end of Bran's IDT page.

void kernel_main(void) {
    terminal_initialize();
    gdt_init();
    idt_init();
    isrs_install();
    irqs_install();
    asm("sti");
    kheap_init();
    kmalloc(2);
    int *arr = kmalloc_aligned(2000* sizeof(int), 25);
    for(int i = 0; i < 2000; i++) {
        arr[i] = 0xFFFFAAAA;
    }

    printf("Addr: %u\n", (uint32_t)arr);

    uint32_t mem = KHEAP_BASE;
    for(int i = 0; i<16; i++) {
        for(int j = 0; j<16; j++) {
            printf("%X ", *(uint8_t *)(uint64_t)(mem + (i*16) + j));
        }
        printf("\n");

    }
    clock_set_freq(100);
    //clock_install();
    keyboard_install();
    printf("Hello, kernel World!\n");
    int pid0 = install_userprog(0);
    int pid1 = install_userprog(1);
    int pid2 = install_userprog(2);
    printf("Installed users.\n");
    //if(pid0) begin_process(pid0);
    if(pid1) begin_process(pid1);
    if(pid2) begin_process(pid2);
    //printf("Page: 0x%X\n", *((char *)0xF0000000));
}
