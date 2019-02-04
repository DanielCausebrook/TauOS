
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

struct gdt_entry {
    uint16_t limit_low;
    uint16_t base_low;
    uint8_t  base_mid;
    uint8_t  access;
    uint8_t  limit_flags;
    uint8_t  base_high;
} __attribute__((packed));

struct gdt_ptr {
    uint16_t limit;
    uint32_t base;
} __attribute__((packed));

extern void load_gdt(struct gdt_ptr *gdt_ptr, unsigned int data_sel, unsigned int code_sel);

struct gdt_entry gdt[3];
struct gdt_ptr gdtPtr;

void gdt_encode_entry(struct gdt_entry *target, uint32_t limit, uint32_t base, uint8_t access) {

    // Check the limit to make sure that it can be encoded
    if ((limit > 65536) && (limit & 0xFFF) != 0xFFF) {
        abort();
    }
    if (limit > 65536) {
        // Adjust granularity if required
        limit = limit >> 12;
        target->limit_flags = 0xC0;
    } else {
        target->limit_flags = 0x40;
    }

    target->base_low = (uint16_t) (base & 0xFFFF);
    target->base_mid = (uint8_t) ((base >> 16) & 0xFF);
    target->base_high = (uint8_t) ((base >> 24) & 0xFF);

    target->limit_low = (uint16_t) (limit & 0xFFFF);
    target->limit_flags |= (uint8_t) ((limit >> 16) & 0x0F);

    target->access = access;
};

void gdt_init() {

    gdt_encode_entry(&gdt[0], 0, 0, 0);

    gdt_encode_entry(&gdt[1], 0xffffffff, 0, 0x9A);
    gdt_encode_entry(&gdt[2], 0xffffffff, 0, 0x92);

    gdtPtr.base = (uint32_t) &gdt;
    gdtPtr.limit = sizeof(gdt);

    load_gdt(&gdtPtr, 0x10, 0x08);
}