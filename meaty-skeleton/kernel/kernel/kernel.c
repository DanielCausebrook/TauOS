#include <stdio.h>
#include <stdint.h>

#include <kernel/multiboot.h>
#include <kernel/tty.h>
#include <kernel/system.h>
#include <stdlib.h>

//TODO Add to header file somewhere. Mentioned at end of Bran's IDT page.

void kernel_main(void) {
    terminal_initialize();
    gdt_init();
    idt_init();
    isrs_install();
    irqs_install();
    asm("sti");
    clock_set_freq(100);
    //clock_install();
    keyboard_install();
    printf("Hello, kernel World!\n");
    //printf("Page: 0x%X\n", *((char *)0xF0000000));
    while(1);
}
