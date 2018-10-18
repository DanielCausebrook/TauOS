CC=i686-elf-gcc

build: 
	$(CC) -T linker.ld -o myos.bin -ffreestanding -O2 -nostdlib boot.o kernel.o -lgcc
	if grub-file --is-x86-multiboot myos.bin; then \
	  cp myos.bin isodir/boot/myos.bin; \
	  grub-mkrescue -o myos.iso isodir --xorriso="${HOME}/TauOS/xorriso/xorriso"; \
	fi
