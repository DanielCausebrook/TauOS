
#include <stdint.h>
#include <string.h>
#include <stdio.h>

struct idt_entry {
    uint16_t offset_low; // offset bits 0..15
    uint16_t selector; // a code segment selector in GDT or LDT
    uint8_t zero;      // unused, set to 0
    uint8_t type_attr; // type and attributes, see
    uint16_t offset_high; // offset bits 16..31
} __attribute__((packed));

struct idt_ptr {
    uint16_t size;
    uint32_t offset;
} __attribute__((packed));

extern void load_idt(struct idt_ptr *idt_ptr);

#define IDTLEN 256
struct idt_entry idt[IDTLEN];
struct idt_ptr idtPtr;

void idt_set_gate(uint16_t gateNum, uint64_t offset, uint16_t selector, uint8_t type_attr) {
    idt[gateNum].offset_low = (uint16_t) (offset & 0xFFFF);
    idt[gateNum].offset_high = (uint16_t) ((offset >> 16) & 0xFFFF);
    idt[gateNum].selector = selector;
    idt[gateNum].type_attr = type_attr;
    idt[gateNum].zero = 0;
}

void idt_init() {
    idtPtr.offset = (uint32_t) &idt;
    idtPtr.size = sizeof(struct idt_entry) * IDTLEN - 1;

    memset(&idt, 0, sizeof(struct idt_entry) * IDTLEN );

    load_idt(&idtPtr);
}
