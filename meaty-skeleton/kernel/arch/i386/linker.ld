/* The bootloader will look at this image and start execution at the symbol
   designated at the entry point. */
ENTRY(_start)
/* Tell where the various sections of the object files will be put in the final
   kernel image. */
SECTIONS
{

	uservma = 0x80000000;

    userlma = 0x400000;

    usersize = 0x100000;

    . = uservma;

	.usertext0 ALIGN(4K) : AT (ADDR (.usertext0) + userlma - uservma)
	{
	    user/user0.o(.main)
	    user/user0.o(.text)
	}
	.userrodata0 ALIGN(4K) : AT (ADDR (.userrodata0) + userlma - uservma)
	{
	    user/user0.o(.rodata)
	}
	.userdata0 ALIGN(4K) : AT (ADDR (.userdata0) + userlma - uservma)
	{
	    user/user0.o(.data)
	}
	.userbss0 ALIGN(4K) : AT (ADDR (.userbss0) + userlma - uservma)
	{
	    user/user0.o(COMMON)
	    user/user0.o(.bss)
	}

    . = uservma;

	.usertext1 ALIGN(4K) : AT (ADDR (.usertext1) + userlma + usersize - uservma)
	{
	    user/user1.o(.main)
	    user/user1.o(.text)
	}
	.userrodata1 ALIGN(4K) : AT (ADDR (.userrodata1) + userlma + usersize - uservma)
	{
	    user/user1.o(.rodata)
	}
	.userdata1 ALIGN(4K) : AT (ADDR (.userdata1) + userlma + usersize - uservma)
	{
	    user/user1.o(.data)
	}
	.userbss1 ALIGN(4K) : AT (ADDR (.userbss1) + userlma + usersize - uservma)
	{
	    user/user1.o(COMMON)
	    user/user1.o(.bss)
	}

	. = uservma;

    .usertext2 ALIGN(4K) : AT (ADDR (.usertext2) + userlma + usersize*2 - uservma)
    {
	    user/user2.o(.main)
        user/user2.o(.text)
    }
    .userrodata2 ALIGN(4K) : AT (ADDR (.userrodata2) + userlma + usersize*2 - uservma)
    {
        user/user2.o(.rodata)
    }
    .userdata2 ALIGN(4K) : AT (ADDR (.userdata2) + userlma + usersize*2 - uservma)
    {
        user/user2.o(.data)
    }
    .userbss2 ALIGN(4K) : AT (ADDR (.userbss2) + userlma + usersize*2 - uservma)
    {
        user/user2.o(COMMON)
        user/user2.o(.bss)
    }

	/* Begin putting sections at 1 MiB, a conventional place for kernels to be
	   loaded at by the bootloader. */
	. = 0xC0100000;

	/* First put the multiboot header, as it is required to be put very early
	   early in the image or the bootloader won't recognize the file format.
	   Next we'll put the .text section. */
	kernel_start = .;
	.text ALIGN (4K) : AT (ADDR (.text) - 0xC0000000)
	{
		*(.multiboot)
		*(.text)
	}

	/* Read-only data. */
	.rodata ALIGN (4K) : AT (ADDR (.rodata) - 0xC0000000)
	{
		*(.rodata)
	}

	/* Read-write data (initialized) */
	.data ALIGN (4K) : AT (ADDR (.data) - 0xC0000000)
	{
		*(.data)
	}

	/* Space for page directory and initial page table */
	.pagetables ALIGN (4K) : AT (ADDR (.pagetables) - 0xC0000000)
	{
		*(.pagetables)
	}

	/* Read-write data (uninitialized) and stack */
	.bss ALIGN (4K) : AT (ADDR (.bss) - 0xC0000000)
	{
		*(COMMON)
		*(.bss)
	}

	kernel_end = .;

	/* The compiler may produce other sections, put them in the proper place in
	   in this file, if you'd like to include them in the final kernel. */
}
