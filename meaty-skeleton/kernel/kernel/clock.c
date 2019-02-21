
#include <kernel/system.h>
#include <stdio.h>

void clock_set_freq(int hz) {
    int divisor = 1193180 / hz;
    outportb(0x43, 0x34); // Rate generator
    outportb(0x40, (unsigned char) (divisor & 0xff));
    outportb(0x40, (unsigned char) (divisor >> 8));
}

int counter = 0;

void clock_handler(struct registers *r) {
    counter++;

    if(counter % 1000 == 0) printf("%d Seconds!\n", counter/100);
}

void clock_install() {
    irq_install_handler(0, clock_handler);
}