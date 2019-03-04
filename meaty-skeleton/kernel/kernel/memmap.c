
#include <stdint.h>
#include <kernel/multiboot.h>
#include <kernel/memlocs.h>
#include <stddef.h>
#include <kernel/system.h>
#include <stdlib.h>
#include <stdio.h>


extern void load_pages(struct page_entry *page_directory);
extern void reload_pages();

// Page table entry for manipulating linked list of unreserved memory.
uint64_t *mem_pte_virloc;

struct page_entry *mem_pte;

struct page_entry *get_mem_pte() {
    return mem_pte;
}

uint64_t *next_free;
uint64_t *last_free;
size_t num_pages;

void mem_map(multiboot_info_t* mbi, unsigned int magic) {
    mem_pte_virloc = (uint64_t *) (uint64_t) MEMMAP_PAGE;
    struct page_entry dir_entry = get_directory()[KERNEL_DIR_ENTRY];
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
                if(page>=0x800000 && page < 0x100000000 && page + 4096 <= addr+len) {
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

