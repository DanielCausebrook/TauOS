#ifndef KERNEL_STDIO_H
#define KERNEL_STDIO_H
#include <stddef.h>

size_t read(int fildes, void *buf, size_t nbyte);

#endif //KERNEL_STDIO_H
