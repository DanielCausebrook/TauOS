#ifndef KERNEL_SYSTEM_H
#define KERNEL_SYSTEM_H


#include <stdint.h>
#include <stddef.h>

struct isr_registers {
    unsigned int gs, fs, es, ds;      /* pushed the segs last */
    unsigned int edi, esi, ebp, esp, ebx, edx, ecx, eax;  /* pushed by 'pusha' */
    unsigned int interrupt_num, err_code;
    unsigned int eip, cs, eflags, useresp, ss;   /* pushed by the processor automatically */
};

void init_meta_pagetable();
void *allocate_real_page();
void free_real_page(void *page);
int allocate_page(uint32_t viraddr);

int map_page(uint32_t realaddr, uint32_t viraddr);
void unmap_page(uint32_t viraddr);

void *kmalloc(size_t size);
void *kcalloc(size_t size);
void *kmalloc_aligned(size_t size, uint32_t alignment);
void *kcalloc_aligned(size_t size, uint32_t alignment);
void kfree(void *ptr);

unsigned char inportb (unsigned short _port);
void outportb (unsigned short _port, unsigned char _data);

void gdt_init();
void idt_init();
void isrs_install();
void irqs_install();
void kheap_init();
void clock_install();
void keyboard_install();

void *paging_init_vga();

void idt_set_gate(uint8_t gateNum, uint64_t offset, uint16_t selector, uint8_t type_attr);

void irq_install_handler(int irq, void (*handler)(struct isr_registers *r));
void irq_uninstall_handler(int irq);

int install_userprog(int progno);
void begin_process(int pid);
void uheap_init(void *heap);
int running_pid();

#define STDIN_MAXSIZE 4096
#define MESSAGES_MAXSIZE 4096
int stdin_get(int pid, void **stdin, size_t **stdin_size);
int heap_map_get(int pid, void **heap_map);

void clock_set_freq(int hz);
#endif //KERNEL_SYSTEM_H
