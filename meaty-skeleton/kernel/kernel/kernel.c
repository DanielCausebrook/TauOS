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
    kheap_init();
    clock_set_freq(100);
    clock_install();
    keyboard_install();
    install_userprogs();
    asm("sti");
    printf("TauOS Minimal operating system\n\n");
    printf("COMMANDS:\n");
    printf("[c] to toggle clock timer.\n");
    printf("[t] to display milliseconds since boot.\n");

    printf("[0-2] to execute user processes:\n");
    printf("    0: Benign process (log an action)\n");
    printf("    1: Malicious program (attempt buffer overflow attack)\n");
    printf("    2: Logging process that is vulnerable to buffer overflow attacks.\n");

    printf("[p] to enable kernel protections forcing specification of maximum message size.");
    printf("    Kernel protections are currently ");
    if(get_message_passing_protection()) printf("ON");
    else printf("OFF");
    printf(".\n----------\n\n");
    while(1);
}
