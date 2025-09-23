nasm -f bin boot/mbr.asm -o build/mbr.bin

nasm -f bin boot/loader.asm -o build/loader.bin

cat build/mbr.bin build/loader.bin > build/disk.img

qemu-system-x86_64 -drive format=raw,file=build/disk.img


