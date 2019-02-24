.global load_pages
.global reload_pages

load_pages:
    mov 4(%esp), %edx # Argument - Directory pointer
    mov %edx, %cr3

    ret

reload_pages:
    mov %cr3, %edx
    mov %edx, %cr3

    ret
