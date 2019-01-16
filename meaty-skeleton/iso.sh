#!/bin/sh
set -e
. ./build.sh

mkdir -p isodir
mkdir -p isodir/boot
mkdir -p isodir/boot/grub

cp sysroot/boot/myos.kernel isodir/boot/myos.kernel
cat > isodir/boot/grub/grub.cfg << EOF
menuentry "myos" {
	multiboot /boot/myos.kernel
}
EOF

if [ -f xorriso-loc.sh ]; then
	grub-mkrescue -o myos.iso isodir --xorriso="$(./xorriso-loc.sh)"
else
	grub-mkrescue -o myos.iso isodir
fi
