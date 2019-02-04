#include <stdio.h>

#include <kernel/tty.h>

void gdt_init();

void kernel_main(void) {
	terminal_initialize();
    gdt_init();
    printf("Hello, kernel World!\n");
}
