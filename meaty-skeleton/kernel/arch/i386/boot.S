# Declare constants for the multiboot header.
.set ALIGN,    1<<0             # align loaded modules on page boundaries
.set MEMINFO,  1<<1             # provide memory map
.set FLAGS,    ALIGN | MEMINFO  # this is the Multiboot 'flag' field
.set MAGIC,    0x1BADB002       # 'magic number' lets bootloader find the header
.set CHECKSUM, -(MAGIC + FLAGS) # checksum of above, to prove we are multiboot

# Declare a header as in the Multiboot Standard.
.section .multiboot
.align 4
.long MAGIC
.long FLAGS
.long CHECKSUM

# Reserve a stack for the initial thread.
.section .bss
.align 16
stack_bottom:
.skip 16384 # 16 KiB
stack_top:

.section .pagetables , "aw", @nobits
    .align 4096
boot_page_directory:
.skip 4096
boot_page_table:
.skip 4096

# The kernel entry point.
.section .text
.global _start
.type _start, @function
_start:


##### PAGING SETUP #####

    # Set destination (%edi) to be the memory reserved above.
    movl $(boot_page_table - 0xC0000000), %edi
    # %esi is the table data to be moved into %edi
    movl $0, %esi
    # %ecx is the loop variable. Leave one entry free for memory mapping.
    movl $1023, %ecx

1: # Loop start
    # If page is not part of kernel, don't write to table.
    # cmpl $(kernel_start - 0xC0000000), %esi
    # jl 2f # Loop tail
    # If we've finished, exit loop.
    cmpl $(kernel_end - 0xC0000000), %esi
    jge 3f # Loop end

    # Create table entry
    movl %esi, %edx
    orl $0x0003, %edx
    movl %edx, (%edi)

2: # Loop tail
    # Add to pointer counters.
    addl $4096, %esi
    addl $4, %edi

    loop 1b # Loop start

3: # Loop end

    # Map the table to 0x0 (entry 0) and 0xC0000000 (entry 768) in the directory.
    movl $(boot_page_table - 0xC0000000 + 0x003), boot_page_directory - 0xC0000000
    movl $(boot_page_table - 0xC0000000 + 0x003), boot_page_directory - 0xC0000000 + 768*4

    # Set cr3 to the address of the directory.
    movl $(boot_page_directory - 0xC0000000), %ecx
    movl %ecx, %cr3

    # Enable paging and write protection in cr0.
    movl %cr0, %ecx
    orl $0x80010000, %ecx
    movl %ecx, %cr0

    # Jump to higher half!
    lea 4f, %ecx
    jmp *%ecx

4: # Higher half start



##### END PAGING SETUP #####
	movl $stack_top, %esp

    # Load the paging directory
    push $(boot_page_directory)
    call init_boot_page_directory

	# Load the memory map information.
    push %eax
    push %ebx
    call mem_map

    # Remove identity mapping
    movl $0, boot_page_directory

    # Force TLB flush
    movl %cr3, %ecx
    movl %ecx, %cr3

    push $(usersize)
    push $(userlma)
    call set_user_lma

	# Call the global constructors.
    call _init

	# Transfer control to the main kernel.
	call kernel_main

	# Hang if kernel_main unexpectedly returns.
	cli
5:	hlt
	jmp 5b
.size _start, . - _start
