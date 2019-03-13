

#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <kernel/system.h>
#include <kernel/multiboot.h>
#include <kernel/tty.h>
#include <kernel/memlocs.h>



extern void load_pages(struct page_entry *page_directory);

struct page_entry *directory;
struct page_entry *meta_pagetable;

struct page_entry *get_directory() {
    return directory;
}

void set_page_entry(struct page_entry *entry, uint32_t aligned_address, uint16_t flags) {
    if((aligned_address & 0xFFF) != 0) {
        printf("Invalid page table address");
        abort();
    }
    entry->flags = (uint8_t) (flags & 0xFF);
    uint8_t high_flags = (uint8_t) ((flags >> 8) & 0xF);
    uint8_t low = (uint8_t) ((aligned_address >> 8) & 0xF0);
    entry->flags_low = low | high_flags;
    entry->high = (uint16_t) ((aligned_address >> 16) & 0xFFFF);

}

int map_page(uint32_t realaddr, uint32_t viraddr) {
    if(viraddr & 0xFFF) return 0;
    uint32_t page = viraddr >> 12;
    int de = page / 0x00400;
    int te = page % 0x00400;
    if(!(directory[de].flags & 0x1)) {
        // Directory entry does not exist yet
        void *new_table = allocate_real_page();
        if(!new_table) return 0;
        // Add new page table location to meta page table, to enable writing to it.
        set_page_entry(&meta_pagetable[de], (uint32_t) new_table, 0x3);
        // Add new page table to directory.
        set_page_entry(&directory[de], (uint32_t) new_table, 0x3);
        reload_pages(); //TODO Add reloading of specific pages?
    }
    struct page_entry *ktable = get_table(de);
    set_page_entry(&ktable[te], realaddr, 0x3);
    reload_pages();
    return 1;
}

void unmap_page(uint32_t viraddr) {
    if(viraddr & 0xFFF) return;
    uint32_t page = viraddr >> 12;
    int de = page / 0x00400;
    int te = page % 0x00400;
    struct page_entry *ktable = get_table(de);
    set_page_entry(&ktable[te], 0, 0x0);
    reload_pages();
}

int allocate_page(uint32_t viraddr) {
    if(viraddr & 0xFFF) return 0;
    return map_page((uint32_t) allocate_real_page(), viraddr);
}

uint32_t get_real(void *ptr) {
    if((uint32_t)ptr < 0xC0000000) return 0;
    int de = (uint32_t)ptr / 0x400000;
    struct page_entry *table = get_table(de);
    int te = ((uint32_t)ptr % 0x400000) / 0x1000;
    struct page_entry entry = table[te];
    return (uint32_t) (entry.high << 16 | (entry.flags_low & 0xF0) << 8);
}

/// Creates a page table to allow access to all other page tables.
void init_meta_pagetable() {
    struct page_entry *mem_pte = get_mem_pte();
    void *realaddr = allocate_real_page();
    // Add page to page table and write using virtual location.
    set_page_entry(mem_pte, (uint32_t) realaddr, 0x3);
    reload_pages();
    // Add the table to the directory and to itself.
    struct page_entry *table = (struct page_entry *) MEMMAP_PAGE;
    set_page_entry(&directory[PAGETABLES_DIR_ENTRY], (uint32_t) realaddr, 0x3);
    set_page_entry(&table[PAGETABLES_DIR_ENTRY], (uint32_t) realaddr, 0x3);
    meta_pagetable = get_table(PAGETABLES_DIR_ENTRY);
    // Clear the page table entry otherwise used for the memory map.
    set_page_entry(mem_pte, 0, 0x0);
    reload_pages();
}

// THe kernel is mapped to 0xC000 0000 virtual. which is directory entry 768.
// The kernel is loaded at 1MiB
// Page entry:      4KiB; 0x  0000,1000; 1024 entries
// Directory entry: 4MiB; 0x  0040,0000; 1024 entries
// All memory:      4GiB; 0x1,0000,0000;
// GENERAL PAGING NOTES
void init_boot_page_directory(struct page_entry *dir) {
    directory = dir;
}

// Map any regions required (excluding kernel, mapped at boot).
// Map VGA memory
void *paging_init_vga() {
    map_page(0xB8000, VGA_BASE);
    reload_pages();
    return VGA_BASE;
}
