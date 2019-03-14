
#include <stdint.h>
#include <kernel/system.h>
#include <kernel/memlocs.h>
#include <kernel/messages.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h> 140

#define VULNERABLE 0

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
    char *stdin_buffer;
    size_t stdin_size;
    struct message **messages;
    size_t messages_size;
    size_t message_max_size;
};

#define MAX_PROCESSES 10
struct process_control_block process_control_blocks[MAX_PROCESSES];
int processes = 0;

struct process_control_block *ready;
size_t ready_size;
struct process_control_block *running;

void main0();

int install_userprog(int progno) {
    int pid = processes + 1;
    struct process_control_block *block = &process_control_blocks[pid-1];
    block->pid = pid;
    block->code_map = kcalloc_aligned(sizeof(struct page_entry) * 0x400, 0x1000);
    if(!block->code_map) return 0;
    for(uint32_t entry = 0; entry < 0x400; entry++) {
        set_page_entry(&block->code_map[entry], users_loc + progno * users_size + entry * 0x1000, 0x7);
        //if(entry < 3) printf("progno = %u, users_size = %u,  reloc = %d\n", progno, users_size, users_loc + progno * users_size + entry * 0x1000);
    }
    uint32_t heap_pages = (UHEAP_LIMIT - UHEAP_BASE) / 0x1000;
    block->heap_map = kcalloc_aligned(sizeof(struct page_entry) * heap_pages + 0x400, 0x1000);
    if(!block->heap_map) return 0;
    uint32_t uheap_real = allocate_real_page();
    set_page_entry(&block->heap_map[0], uheap_real, 0x7);
    map_page(uheap_real, UHEAP_INIT_BASE);
    uheap_init((void *) UHEAP_INIT_BASE);
    unmap_page(UHEAP_INIT_BASE);
    block->stdin_buffer = kmalloc(STDIN_MAXSIZE);
    block->stdin_size = 0;
    block->messages = kmalloc(MESSAGES_MAXSIZE);
    block->messages_size = 0;
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
    running = &process_control_blocks[pid-1];
    ((void (*)()) UCODE_BASE)();
    running = 0;
}

int running_pid() {
    if(!running) return 0;
    return running->pid;
}

int stdin_get(int pid, void **stdin_buffer, size_t **stdin_size) {
    if(pid > MAX_PROCESSES || pid == 0 || process_control_blocks[pid-1].pid == 0) return 0;
    *stdin_buffer = process_control_blocks[pid-1].stdin_buffer;
    *stdin_size = &(process_control_blocks[pid-1].stdin_size);
    return 1;
}

int heap_map_get(int pid, void **heap_map) {
    if(pid > MAX_PROCESSES || pid == 0 || process_control_blocks[pid-1].pid == 0) return 0;
    *heap_map = process_control_blocks[pid-1].heap_map;
    return 1;
}

int is_protecting = 0;

void set_message_passing_protection(int protected) {
    is_protecting = protected;
}

int get_message_passing_protection() {
    return is_protecting;
}

void enable_message_passing(size_t size) {
}
void enable_message_passing_pid(int pid, size_t size) {
    if(pid > MAX_PROCESSES || pid == 0 || process_control_blocks[pid-1].pid == 0) return 0;
    struct process_control_block *block = &process_control_blocks[pid - 1];
    block->message_max_size = size;
}

int send(void *message, size_t message_size, int dest_pid) {
    if(dest_pid > MAX_PROCESSES || dest_pid == 0 || process_control_blocks[dest_pid-1].pid == 0) return 0;
    struct process_control_block *destblock = &process_control_blocks[dest_pid - 1];
    if(destblock->messages_size >= MESSAGES_MAXSIZE) return 0;

    if(is_protecting && destblock->message_max_size < message_size) return 0;

    void *kmessage = kmalloc(message_size);
    struct message *messageblock = kmalloc(sizeof(struct message));
    memcpy(kmessage, message, message_size);
    messageblock->message = kmessage;
    messageblock->size = message_size;
    messageblock->sender = running_pid();
    destblock->messages[destblock->messages_size++] = messageblock;
    return 1;
}

int recieve(struct message *messageblock) {
    if(!running_pid()) return 0;
    struct process_control_block *block = &process_control_blocks[running_pid() - 1];
    if(block->messages_size == 0) return 0;
    struct message *kmessageblock = block->messages[0];
    void *message = malloc(kmessageblock->size);
    memcpy(message, kmessageblock->message, kmessageblock->size);
    messageblock->message = message;
    messageblock->size = kmessageblock->size;
    messageblock->sender = kmessageblock->sender;

    free(kmessageblock->message);
    block->messages_size--;
    memcpy(block->messages[0], block->messages[1], block->messages_size * sizeof(struct message));
    return 1;
}