#include <stddef.h>
#include <kernel/system.h>
#include <string.h>
#include <kernel/stdio.h>
#include <stdio.h>

size_t read(int fildes, void *buf, size_t nbyte) {
    if(fildes == 0) {
        void **stdin_buffer = kmalloc(sizeof(void **));
        size_t **stdin_size = kmalloc(sizeof(size_t));
        if(!stdin_get(running_pid(), stdin_buffer, stdin_size) ) return 0;
        size_t return_nbytes = (**stdin_size < nbyte) ? **stdin_size : nbyte;
        memcpy(buf, *stdin_buffer, return_nbytes);
        **stdin_size -= return_nbytes;
        memcpy(*stdin_buffer, *stdin_buffer + return_nbytes, **stdin_size);
        kfree(stdin_buffer);
        kfree(stdin_size);
        return return_nbytes;
    }
    else return 0;
}