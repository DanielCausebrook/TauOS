//
// Created by daniel on 26/02/19.
//

#ifndef KERNEL_MEMLOCS_H
#define KERNEL_MEMLOCS_H

#include <stdint.h>

struct page_entry {
    uint8_t flags;
    uint8_t flags_low;
    uint16_t high;
} __attribute__((packed));

struct page_entry *get_directory();
struct page_entry *get_mem_pte();
uint32_t get_real(void *ptr);
void set_page_entry(struct page_entry *entry, uint32_t aligned_address, uint16_t flags);
extern void reload_pages();


// @formatter:off

#define UCODE_BASE                     0x80000000
#define UHEAP_BASE                     0x90000000
    #define UHEAP_LIMIT                0xB0000000
#define USTACK_BOTTOM                  0xB0000000
#define USTACK_TOP                     0xC0000000
#define KCODE_BASE                     0xC0000000
    #define KERNEL_BASE                    0xC0000000
        #define KERNEL_DIR_ENTRY               768
        #define KERNEL_LIMIT                   0xC03FF000
    #define MEMMAP_PAGE                    0xC03FF000
    #define KCODE_LIMIT                    0xC03FF000
#define KDATA_BASE                     0xC03FF000
    #define PAGETABLES_BASE                0xC0800000
        #define PAGETABLES_DIR_ENTRY           770
        #define PAGETABLES_LIMIT               0xC0C00000
    #define UHEAP_INIT_BASE                0xC0C00000
        #define UHEAP_INIT_LIMIT               0xC0C01000
    #define KHEAP_BASE                     0xD0000000
        #define KHEAP_LIMIT                    0xF0000000
    #define VGA_BASE                       0xF0000000

// @formatter:on

#define get_table(de) ((struct page_entry *) (uint64_t) (PAGETABLES_DIR_ENTRY * 0x00400000 + de * 0x00001000))

#endif //KERNEL_MEMLOCS_H
