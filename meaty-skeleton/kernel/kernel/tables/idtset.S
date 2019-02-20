.global load_idt

load_idt:
    mov 4(%esp), %edx    # Argument 1 - IDT pointer
    lidt (%edx)          # Load the IDT
    ret
