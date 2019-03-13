
full_path="myos${1}.iso"
qemu-system-i386 -d guest_errors -cdrom $full_path
