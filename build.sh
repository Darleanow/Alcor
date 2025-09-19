nasm -f bin boot/mbr.asm -o build/mbr.img
qemu-system-x86_64 -drive format=raw,file=build/mbr.img

