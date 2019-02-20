#ifndef KERNEL_SYSTEM_H
#define KERNEL_SYSTEM_H


#include <stdint.h>

struct registers {
    unsigned int gs, fs, es, ds;      /* pushed the segs last */
    unsigned int edi, esi, ebp, esp, ebx, edx, ecx, eax;  /* pushed by 'pusha' */
    unsigned int interrupt_num, err_code;
    unsigned int eip, cs, eflags, useresp, ss;   /* pushed by the processor automatically */
};

unsigned char inportb (unsigned short _port);
void outportb (unsigned short _port, unsigned char _data);

void gdt_init();
void idt_init();
void isrs_install();
void irqs_install();
void clock_install();

void idt_set_gate(uint8_t gateNum, uint64_t offset, uint16_t selector, uint8_t type_attr);

void irq_install_handler(int irq, void (*handler)(struct registers *r));
void irq_uninstall_handler(int irq);

void clock_set_freq(int hz);
#endif //KERNEL_SYSTEM_H
