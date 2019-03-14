
#include <stdio.h>
#include <kernel/system.h>

unsigned char kbdus[128] =
        {
                0,  27, '1', '2', '3', '4', '5', '6', '7', '8',	/* 9 */
                '9', '0', '-', '=', '\b',	/* Backspace */
                '\t',			/* Tab */
                'q', 'w', 'e', 'r',	/* 19 */
                't', 'y', 'u', 'i', 'o', 'p', '[', ']', '\n',	/* Enter key */
                0,			/* 29   - Control */
                'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', ';',	/* 39 */
                '\'', '`',   0,		/* Left shift */
                '\\', 'z', 'x', 'c', 'v', 'b', 'n',			/* 49 */
                'm', ',', '.', '/',   0,				/* Right shift */
                '*',
                0,	/* Alt */
                ' ',	/* Space bar */
                0,	/* Caps lock */
                0,	/* 59 - F1 key ... > */
                0,   0,   0,   0,   0,   0,   0,   0,
                0,	/* < ... F10 */
                0,	/* 69 - Num lock*/
                0,	/* Scroll Lock */
                0,	/* Home key */
                0,	/* Up Arrow */
                0,	/* Page Up */
                '-',
                0,	/* Left Arrow */
                0,
                0,	/* Right Arrow */
                '+',
                0,	/* 79 - End key*/
                0,	/* Down Arrow */
                0,	/* Page Down */
                0,	/* Insert Key */
                0,	/* Delete Key */
                0,   0,   0,
                0,	/* F11 Key */
                0,	/* F12 Key */
                0,	/* All other keys are undefined */
        };

void keyboard_handler(struct isr_registers *r) {
    unsigned char scancode = inportb(0x60);

    if (scancode & 0x80) {

    } else {
        char *stdin_buffer = 0;
        size_t *stdin_size = 0;
        if(running_pid()) {
            printf("%d\n", running_pid());
            if(!stdin_get(running_pid(), (void **) &stdin_buffer, &stdin_size) ) return;
            if(*stdin_size == STDIN_MAXSIZE) return;
            stdin_buffer[(*stdin_size)++] = kbdus[scancode];
        } else {
            if(kbdus[scancode] == '0') run_userprog(0);
            else if(kbdus[scancode] == '1') run_userprog(1);
            else if(kbdus[scancode] == '2') run_userprog(2);
            else if(kbdus[scancode] == 'c') {
                if(get_announce_clock()) {
                    set_announce_clock(0);
                    printf("Clock OFF.\n");
                } else {
                    set_announce_clock(1);
                    printf("Clock ON.\n");
                }
            }
            else if(kbdus[scancode] == 't') printf("%dms since boot.\n", get_time_ms());
            else if(kbdus[scancode] == 'p') {
                if(get_message_passing_protection()) {
                    set_message_passing_protection(0);
                    printf("Kernel protections are OFF.\n");
                } else {
                    set_message_passing_protection(1);
                    printf("Kernel protections are ON.\n");
                }
            }

        }
//        char str[2] = {
//                kbdus[scancode],
//                '\0'
//        };
//        printf(str);
//        printf(" key.\n");
    }
}

void keyboard_install() {
    irq_install_handler(1, keyboard_handler);
}