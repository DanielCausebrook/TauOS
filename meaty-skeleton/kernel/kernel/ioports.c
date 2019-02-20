#include <stdio.h>

/// Read from an I/O port
unsigned char inportb (unsigned short port)
{
    unsigned char rv;
    __asm__ __volatile__ ("inb %1, %0" : "=a" (rv) : "dN" (port));
    return rv;
}

/// Write to an I/O port
void outportb (unsigned short port, unsigned char data)
{
    __asm__ __volatile__ ("outb %0, %1" : : "a" (data), "Nd" (port));
}