#include <stdio.h>

#include <kernel/tty.h>

//TODO Add to header file somewhere. Mentioned at end of Bran's IDT page.
void gdt_init();
void idt_init();
void isrs_install();

void kernel_main(void) {
	terminal_initialize();
    gdt_init();
    idt_init();
    isrs_install();
    printf("Hello, kernel World!\n");
}
