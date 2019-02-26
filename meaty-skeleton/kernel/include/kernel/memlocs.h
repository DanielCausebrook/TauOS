//
// Created by daniel on 26/02/19.
//

#ifndef KERNEL_MEMLOCS_H
#define KERNEL_MEMLOCS_H


// @formatter:off

#define KCODE_BASE                     0xC0000000
    #define KERNEL_BASE                    0xC0000000
        #define KERNEL_DIR_ENTRY               768
        #define KERNEL_LIMIT                   0xC03FEFFF
    #define KCODE_LIMIT                    0xC03FEFFF
#define KDATA_BASE                     0xC03FF000
    #define PAGETABLES_BASE                0xC0400000
        #define PAGETABLES_DIR_ENTRY           769
    #define KHEAP_BASE                     0xD0000000
        #define KHEAP_LIMIT                    0xEFFFFFFF
    #define VGA_BASE                       0xF0000000
    #define KDATA_LIMIT                    0xFFFFFFFF

// @formatter:on

#define get_table(de) ((struct page_entry *) (uint64_t) (PAGETABLES_DIR_ENTRY * 0x00400000 + de * 0x00001000))

#endif //KERNEL_MEMLOCS_H
