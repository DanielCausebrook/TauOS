

#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <kernel/system.h>
#include <kernel/multiboot.h>
#include <kernel/tty.h>


struct page_entry {
    uint8_t flags;
    uint8_t flags_low;
    uint16_t high;
} __attribute__((packed));

extern void load_pages(struct page_entry *page_directory);
extern void reload_pages();

struct page_entry *directory;
struct page_entry *meta_pagetable;

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

void init_boot_page_directory(struct page_entry *dir) {
    directory = dir;
}
// For now, just allocate consecutive pages.
#define KERNEL_DIR_ENTRY 768
int curr_kdir_entry = KERNEL_DIR_ENTRY + 1;
struct page_entry *curr_ktable;
int next_kpage = 1024;

void *allocate_kpage_at(uint32_t realaddr) {
    if(next_kpage >= 1024){
        next_kpage = 0;
        curr_kdir_entry++;
        void *new_table = allocate_real_page();
        curr_ktable = (struct page_entry *) (uint64_t) (((KERNEL_DIR_ENTRY+1) * 0x00400000 + curr_kdir_entry * 0x00001000));
        // Add new page table location to meta page table, to enable writing to it.
        set_page_entry(&meta_pagetable[curr_kdir_entry], (uint32_t) new_table, 0x3);
        // Add new page table to directory.
        set_page_entry(&directory[curr_kdir_entry], (uint32_t) new_table, 0x3);
        reload_pages(); //TODO Add reloading of specific pages?
    }
    void *alloced = (void *) (curr_kdir_entry*0x00400000 + next_kpage * 0x00001000);
    set_page_entry(&curr_ktable[next_kpage], realaddr, 0x3);
    next_kpage++;
    return alloced;
}

void *allocate_kpage() {
    allocate_kpage_at((uint32_t) allocate_real_page());
}


// Page table entry for manipulating linked list of unreserved memory.
uint64_t *mem_pte_virloc;
struct page_entry *mem_pte;

/// Creates a page table to allow access to all other page tables.
void init_meta_pagetable() {
    void *realaddr = allocate_real_page();
    // Add page to page table and write using virtual location.
    set_page_entry(mem_pte, (uint32_t) realaddr, 0x3);
    reload_pages();
    // Add the table to the directory and to itself.
    struct page_entry *table = (struct page_entry *) mem_pte_virloc;
    set_page_entry(&directory[KERNEL_DIR_ENTRY + 1], (uint32_t) realaddr, 0x3);
    set_page_entry(&table[KERNEL_DIR_ENTRY + 1], (uint32_t) realaddr, 0x3);
    meta_pagetable = (struct page_entry *) (uint64_t) ((KERNEL_DIR_ENTRY + 1) * 0x00400000 + (KERNEL_DIR_ENTRY + 1) * 0x00001000);
    // Clear the page table entry otherwise used for the memory map.
    set_page_entry(mem_pte, 0, 0x0);
    reload_pages();
}

uint64_t *next_free;
uint64_t *last_free;

void mem_map(multiboot_info_t* mbi, unsigned int magic) {
    mem_pte_virloc = (uint64_t *) (uint64_t) (KERNEL_DIR_ENTRY * 0x00400000 + 1023 * 0x00001000);
    struct page_entry dir_entry = directory[KERNEL_DIR_ENTRY];
    struct page_entry *page_table = (struct page_entry *) (uint64_t) (((dir_entry.high << 16) | (dir_entry.flags_low << 8 & 0xF000)) + 0xC0000000);
    mem_pte = &page_table[1023];

    struct multiboot_mmap_entry *mmap = (struct multiboot_mmap_entry *) mbi->mmap_addr;
    int init = 0;
    while(mmap < mbi->mmap_addr + mbi->mmap_length) {
        uint64_t addr = ((uint64_t) mmap->addr_high << 32) | mmap->addr_low;
        uint64_t len = ((uint64_t) mmap->len_high << 32) | mmap->len_low;
        if(mmap->type == 1) {
            uint64_t page = addr + 4096 - addr%4096;
            while(page < addr+len) {
                if(page>=0x200000 && page < 0x100000000) {
                    if (!init) {
                        init = 1;
                        next_free = (uint64_t *) page;
                        last_free = (uint64_t *) page;
                    } else {
                        // Add page to page table and write using virtual location.
                        set_page_entry(mem_pte, (uint32_t) last_free, 0x3);
                        reload_pages(); //TODO Add reloading of specific pages?
                        *mem_pte_virloc = (uint32_t) page;
                        last_free = (uint64_t *) page;
                    }
                }
                page += 4096;
            }
        }
        mmap = (multiboot_memory_map_t*) ( (unsigned int)mmap + mmap->size + sizeof(mmap->size) );
    }
    init_meta_pagetable();
}

void *allocate_real_page() {
    void *ptr = next_free;
    // Add page to page table and write using virtual location.
    set_page_entry(mem_pte, (uint32_t) next_free, 0x3);
    reload_pages();
    next_free = (uint64_t *) *mem_pte_virloc;
    *mem_pte_virloc = 0;
    return ptr;
}

void free_real_page(void *page) {
    if(((uint64_t) page & 0xFFF) != 0) {
        printf("Cannot free a page that is not 4KiB aligned.");
        abort();
    }
    // Add page to page table and write using virtual location.
    set_page_entry(mem_pte, (uint32_t) page, 0x3);
    reload_pages();
    *mem_pte_virloc = (uint64_t) next_free;
    next_free = page;
}
// GENERAL PAGING NOTES
// All memory:      4GiB; 0x1,0000,0000;
// Directory entry: 4MiB; 0x  0040,0000; 1024 entries
// Page entry:      4KiB; 0x  0000,1000; 1024 entries
// The kernel is loaded at 1MiB
// THe kernel is mapped to 0xC000 0000 virtual. which is directory entry 768.

// Map any regions required (excluding kernel, mapped at boot).
// Map VGA memory
void *paging_init_vga() {
    void *vgaptr = allocate_kpage_at(0xB8000);

    reload_pages();
    return vgaptr;
}