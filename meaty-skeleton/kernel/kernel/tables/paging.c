

#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <kernel/system.h>
#include <kernel/multiboot.h>
#include <kernel/tty.h>
#include <kernel/memlocs.h>


struct page_entry {
    uint8_t flags;
    uint8_t flags_low;
    uint16_t high;
} __attribute__((packed));

extern void load_pages(struct page_entry *page_directory);
extern void reload_pages();

struct page_entry *directory;
struct page_entry *meta_pagetable;

struct page_entry *curr_ktable;
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

int map_kpage(uint32_t realaddr, uint32_t viraddr) {
    if(viraddr & 0xFFF || viraddr < 0xC0000000) return 0;
    uint32_t page = viraddr >> 12;
    int de = page / 0x00400;
    int te = page % 0x00400;
    if(!(directory[de].flags & 0x1)) {
        // Directory entry does not exist yet
        void *new_table = allocate_real_page();
        if(!new_table) return 0;
        curr_ktable = get_table(de);
        // Add new page table location to meta page table, to enable writing to it.
        set_page_entry(&meta_pagetable[de], (uint32_t) new_table, 0x3);
        // Add new page table to directory.
        set_page_entry(&directory[de], (uint32_t) new_table, 0x3);
        reload_pages(); //TODO Add reloading of specific pages?
    }
    set_page_entry(&curr_ktable[te], realaddr, 0x3);
    return 1;
}

int allocate_kpage(uint32_t viraddr) {
    if(viraddr & 0xFFF || viraddr < 0xC0000000) return 0;
    return map_kpage((uint32_t) allocate_real_page(), viraddr);
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
    set_page_entry(&directory[PAGETABLES_DIR_ENTRY], (uint32_t) realaddr, 0x3);
    set_page_entry(&table[PAGETABLES_DIR_ENTRY], (uint32_t) realaddr, 0x3);
    meta_pagetable = get_table(PAGETABLES_DIR_ENTRY);
    // Clear the page table entry otherwise used for the memory map.
    set_page_entry(mem_pte, 0, 0x0);
    reload_pages();
}

uint64_t *next_free;
uint64_t *last_free;
size_t num_pages;

void mem_map(multiboot_info_t* mbi, unsigned int magic) {
    mem_pte_virloc = (uint64_t *) (uint64_t) (KERNEL_DIR_ENTRY * 0x00400000 + 1023 * 0x00001000);
    struct page_entry dir_entry = directory[KERNEL_DIR_ENTRY];
    struct page_entry *page_table = (struct page_entry *) (uint64_t) (((dir_entry.high << 16) | (dir_entry.flags_low << 8 & 0xF000)) + 0xC0000000);
    mem_pte = &page_table[1023];

    struct multiboot_mmap_entry *mmap = (struct multiboot_mmap_entry *) mbi->mmap_addr;
    int init = 0;
    while((unsigned int) mmap < mbi->mmap_addr + mbi->mmap_length) {
        uint64_t addr = ((uint64_t) mmap->addr_high << 32) | mmap->addr_low;
        uint64_t len = ((uint64_t) mmap->len_high << 32) | mmap->len_low;
        if(mmap->type == 1) {
            uint64_t page = addr + 4096 - addr%4096;
            while(page < addr+len) {
                if(page>=0x200000 && page < 0x100000000 && page + 4096 <= addr+len) {
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
                    num_pages++;
                }
                page += 4096;
            }
        }
        mmap = (multiboot_memory_map_t*) ( (unsigned int)mmap + mmap->size + sizeof(mmap->size) );
    }
    init_meta_pagetable();
}

void *allocate_real_page() {
    if(num_pages == 0) return 0;
    void *ptr = next_free;
    // Add page to page table and write using virtual location.
    set_page_entry(mem_pte, (uint32_t) next_free, 0x3);
    reload_pages();
    next_free = (uint64_t *) *mem_pte_virloc;
    *mem_pte_virloc = 0;
    num_pages--;
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
    num_pages++;
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
    map_kpage(0xB8000, VGA_BASE);
    reload_pages();
    return VGA_BASE;
}
