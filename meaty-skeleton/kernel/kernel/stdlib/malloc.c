
#include <stdint.h>
#include <kernel/system.h>
#include <kernel/memlocs.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>

/*
 *  Header
 *
 * 0..31  : 32 bit pointer to next block header in different page
 * 32     : Used block if set
 * 33     : Last block
 * 34..38 : Unused
 *
 * If this block is the last block, the pointer points to the next unallocated page.
 * THe last block must always be unused.
 *
 */
struct block_header {
    uint32_t next;
    uint8_t flags;
};
#define HEADER_SIZE 5

#define USED (uint8_t)0x1
#define LAST (uint8_t)0x2
#define getflag(b, f) (b->flags & f)
#define islast(b) getflag(b, LAST)
#define isused(b) getflag(b, USED)

#define flags(u, l) ((u ? 0x1 : 0) |)

struct block_header *kheap_start;
size_t kheap_num_pages = 0;
size_t kheap_page_limit;

/// Returns a pointer to an address some number of bytes above a header.
void *get_addr_rel(struct block_header *block, size_t c) {
    return (void *) (uint64_t) ((uint32_t)block + c);
}

struct block_header *next_block(struct block_header *curr) {
    if(islast(curr)) return 0;
    return (struct block_header *) (uint64_t) curr->next;
}

size_t block_space(struct block_header *block) {
    struct block_header *next = next_block(block);
    return block->next - (uint32_t)block - HEADER_SIZE;
}

int set_block(struct block_header *block, struct block_header *next, uint8_t flags) {
    // Next is in different page, use long header.
    if((uint32_t)next - (uint32_t)block < HEADER_SIZE) return 0;
    block->flags = flags;
    block->next = (uint32_t) next;
    return 1;
}

/// Merges the provided block with any subsequent free blocks.
void block_merge_free(struct block_header *block) {
    struct block_header *next = next_block(block);
    if(islast(block) || isused(next) || isused(block)) return;
    while(!isused(next) && !islast(next)) next = next_block(next);
    if(islast(next) && !isused(next)) {
        set_block(block, next_block(next), LAST);
    } else {
        set_block(block, next, 0);
    }
}

int allocate_block(struct block_header *block, size_t size) {
    if(isused(block)) return 0;
    struct block_header *next = get_addr_rel(block, size + HEADER_SIZE);
    struct block_header *old_next = next_block(block);
    if(islast(block)) {
        // Require space for an extra free header to be marked last at the end of allocated memory.
        size_t space = block_space(block) - HEADER_SIZE;
        uint32_t heap_end = block->next;
        if(heap_end & 0xFFF) {
            printf("SEGFAULT in kernel heap");
            abort();
        }
        // Checks if extra pages are needed
        if(size > space) {
            // Check if enough pages are available
            if(size - space > (kheap_page_limit - kheap_num_pages) * 0x1000) return 0;
            while(space < size) {
                if(!allocate_kpage(heap_end)) return 0;
                heap_end += 0x1000;
                space += 0x1000;
                kheap_num_pages++;
            }
        }
        // Create the last free header,
        set_block(next, (struct block_header *) (uint64_t) heap_end, LAST);
    } else {
        // If the requested memory is less than the free space, split memory with new header.
        size_t space = block_space(block);
        if(space < size) {
            return 0;
        } else if(space < size + HEADER_SIZE) {
            next = old_next;
        } else {
            set_block(next, old_next, 0);
        }
    }
    set_block(block, next, USED);
    return 1;
}


void *kmalloc(size_t size) {
    struct block_header *curr = kheap_start;
    block_merge_free(curr);
    while((isused(curr) || block_space(curr) < size) && !islast(curr)) {
        if(size == 2) printf("BSP: %lu\n", block_space(curr));
        curr = next_block(curr);
        block_merge_free(curr);
    }
    if(!allocate_block(curr, size)) return 0;
    return get_addr_rel(curr, HEADER_SIZE);
}

void kfree(void* ptr) {
    struct block_header *block = ptr - HEADER_SIZE;
    set_block(block, next_block(block), 0);
}

void kheap_init() {
    kheap_start = (struct block_header *) KHEAP_BASE;
    kheap_page_limit = (KHEAP_LIMIT + 1 - KHEAP_BASE) / 0x1000;
    allocate_kpage((uint32_t) kheap_start);
    set_block(kheap_start, get_addr_rel(kheap_start, 0x1000), LAST);
}
