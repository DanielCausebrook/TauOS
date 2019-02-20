.global irq0
.global irq1
.global irq2
.global irq3
.global irq4
.global irq5
.global irq6
.global irq7
.global irq8
.global irq9
.global irq10
.global irq11
.global irq12
.global irq13
.global irq14
.global irq15

# 32: IRQ0
irq0:
    cli
    push $0
    push $0
    jmp irq_common_stub

# 33: IRQ1
irq1:
    cli
    push $0
    push $1
    jmp irq_common_stub

# 34: IRQ2
irq2:
    cli
    push $0
    push $2
    jmp irq_common_stub

# 35: IRQ3
irq3:
    cli
    push $0
    push $3
    jmp irq_common_stub

# 36: IRQ4
irq4:
    cli
    push $0
    push $4
    jmp irq_common_stub

# 37: IRQ5
irq5:
    cli
    push $0
    push $5
    jmp irq_common_stub

# 38: IRQ6
irq6:
    cli
    push $0
    push $6
    jmp irq_common_stub

# 39: IRQ7
irq7:
    cli
    push $0
    push $7
    jmp irq_common_stub

# 40: IRQ8
irq8:
    cli
    push $0
    push $8
    jmp irq_common_stub

# 41: IRQ9
irq9:
    cli
    push $0
    push $9
    jmp irq_common_stub

# 42: IRQ10
irq10:
    cli
    push $0
    push $10
    jmp irq_common_stub

# 43: IRQ11
irq11:
    cli
    push $0
    push $11
    jmp irq_common_stub

# 44: IRQ12
irq12:
    cli
    push $0
    push $12
    jmp irq_common_stub

# 45: IRQ13
irq13:
    cli
    push $0
    push $13
    jmp irq_common_stub

# 46: IRQ14
irq14:
    cli
    push $0
    push $14
    jmp irq_common_stub

# 47: IRQ15
irq15:
    cli
    push $0
    push $15
    jmp irq_common_stub

# This is a stub that we have created for IRQ based ISRs. This calls
# '_irq_handler' in our C code. We need to create this in an 'irq.c'
irq_common_stub:
    pusha
    push %ds
    push %es
    push %fs
    push %gs
    mov $0x10, %ax
    mov %ax, %ds
    mov %ax, %es
    mov %ax, %fs
    mov %ax, %gs
    mov %esp, %eax
    push %eax
    # mov irq_handler, %eax
    call irq_handler
    pop %eax
    pop %gs
    pop %fs
    pop %es
    pop %ds
    popa
    add $8, %esp
    iret
