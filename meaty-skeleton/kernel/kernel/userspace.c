
#include <stdint.h>
#include <kernel/system.h>
#include <kernel/memlocs.h>
#include <stdio.h>

uint32_t users_loc;
uint32_t users_size;

void set_user_lma(uint32_t loc, uint32_t size) {
    users_loc = loc;
    users_size = size;
}

struct registers {
    unsigned int gs, fs, es, ds;      /* pushed the segs last */
    unsigned int edi, esi, ebp, esp, ebx, edx, ecx, eax;  /* pushed by 'pusha' */
    unsigned int eip, cs, eflags, useresp, ss;   /* pushed by the processor automatically */
};

struct process_control_block {
    int pid;
    struct page_entry *code_map;
    struct page_entry *heap_map;
    struct registers *regs;
};

#define MAX_PROCESSES 10
struct process_control_block process_control_blocks[MAX_PROCESSES];
int processes = 0;

void main0();

int install_userprog(int progno) {
    int pid = processes + 1;
    struct process_control_block *block = &process_control_blocks[pid-1];
    block->code_map = kcalloc_aligned(sizeof(struct page_entry) * 0x400, 0x1000);
    if(!block->code_map) return 0;
    for(uint32_t entry = 0; entry < 0x400; entry++) {
        set_page_entry(&block->code_map[entry], users_loc + progno * users_size + entry * 0x1000, 0x7);
        //if(entry < 3) printf("progno = %u, users_size = %u,  reloc = %d\n", progno, users_size, users_loc + progno * users_size + entry * 0x1000);
    }
    uint32_t heap_pages = (UHEAP_LIMIT - UHEAP_BASE) / 0x1000;
    block->heap_map = kcalloc_aligned(sizeof(struct page_entry) * heap_pages + 0x400, 0x1000);
    if(!block->heap_map) return 0;
    set_page_entry(&block->heap_map[0], (uint32_t) allocate_real_page(), 0x3);
    processes++;
    return pid;
}

void begin_process(int pid) {
    struct page_entry *dir = get_directory();
    struct process_control_block block = process_control_blocks[pid-1];
    uint32_t code_de = UCODE_BASE / 0x400000;
    uint32_t heap_de = UHEAP_BASE / (uint32_t)0x400000;
    uint32_t heap_de_size = UHEAP_LIMIT / 0x400000 - heap_de;
    set_page_entry(&dir[code_de], get_real(block.code_map), 0x7);
    for(uint32_t i = 0; i < heap_de_size; i++) {
        set_page_entry(&dir[i + heap_de], get_real(&block.heap_map[i * 0x400]), 0x7);
    }
    reload_pages();
    ((void (*)()) UCODE_BASE)();
}