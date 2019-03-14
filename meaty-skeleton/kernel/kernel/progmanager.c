
#include <kernel/system.h>
#include <kernel/messages.h>
#include <stdio.h>


int pid0;
int pid1;
int pid2;

void install_userprogs() {
    pid0 = install_userprog(0);
    pid1 = install_userprog(1);
    pid2 = install_userprog(2);
    enable_message_passing_pid(pid2, 12);
}

void run_userprog(int id) {
    printf("\nRunning user process %d...\n", id);
    printf("===============================================================================\n");
    if(id == 0) begin_process(pid0);
    if(id == 1) begin_process(pid1);
    if(id == 2) begin_process(pid2);
    printf("\n");
    printf("===============================================================================\n");
}