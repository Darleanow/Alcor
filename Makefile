# Makefile

# Tools
ASM = nasm
CC = gcc
LD = ld

# Flags
ASMFLAGS = -f bin
CFLAGS = -m32 -std=gnu99 -ffreestanding -O2 -Wall -Wextra -fno-builtin -fno-stack-protector -nostdlib -nostartfiles -nodefaultlibs
LDFLAGS = -m elf_i386 -T kernel/linker.ld

# Directories
BUILD_DIR = build
BOOT_DIR = boot
KERNEL_DIR = kernel

# Files
MBR_SRC = $(BOOT_DIR)/mbr.asm
LOADER_SRC = $(BOOT_DIR)/loader.asm
KERNEL_SOURCES = $(wildcard $(KERNEL_DIR)/*.c)
KERNEL_OBJECTS = $(KERNEL_SOURCES:$(KERNEL_DIR)/%.c=$(BUILD_DIR)/%.o)

# Output files
MBR_BIN = $(BUILD_DIR)/mbr.bin
LOADER_BIN = $(BUILD_DIR)/loader.bin
KERNEL_BIN = $(BUILD_DIR)/kernel.bin
DISK_IMG = $(BUILD_DIR)/disk.img

.PHONY: all clean run

all: $(DISK_IMG)

# Create build directory
$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

# Build MBR
$(MBR_BIN): $(MBR_SRC) | $(BUILD_DIR)
	$(ASM) $(ASMFLAGS) $< -o $@

# Build Loader  
$(LOADER_BIN): $(LOADER_SRC) | $(BUILD_DIR)
	$(ASM) $(ASMFLAGS) $< -o $@

# Build kernel object files
$(BUILD_DIR)/%.o: $(KERNEL_DIR)/%.c | $(BUILD_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

# Link kernel
$(KERNEL_BIN): $(KERNEL_OBJECTS) | $(BUILD_DIR)
	$(LD) $(LDFLAGS) -o kernel_temp.elf $^ 
	objcopy -O binary kernel_temp.elf $@
	rm kernel_temp.elf

# Create disk image
$(DISK_IMG): $(MBR_BIN) $(LOADER_BIN) $(KERNEL_BIN) | $(BUILD_DIR)
	# Create empty disk image (1.44MB floppy size)
	dd if=/dev/zero of=$@ bs=512 count=2880
	
	# Write MBR to sector 1
	dd if=$(MBR_BIN) of=$@ bs=512 count=1 conv=notrunc
	
	# Write loader to sector 2
	dd if=$(LOADER_BIN) of=$@ bs=512 count=1 seek=1 conv=notrunc
	
	# Write kernel starting at sector 3
	dd if=$(KERNEL_BIN) of=$@ bs=512 seek=2 conv=notrunc

# Run in QEMU
run: $(DISK_IMG)
	qemu-system-x86_64 -drive format=raw,file=$< -m 32M

# Debug run
debug: $(DISK_IMG)
	qemu-system-x86_64 -drive format=raw,file=$< -m 32M -s -S

# Clean build files
clean:
	rm -rf $(BUILD_DIR)

# Check if 32-bit support is available
check:
	@which $(CC) > /dev/null || (echo "GCC not found!" && exit 1)
	@echo "GCC found: $($(CC) --version | head -1)"
	@echo "Checking 32-bit support..."
	@$(CC) -m32 --version > /dev/null 2>&1 || (echo "32-bit support missing! Install gcc-multilib" && exit 1)
	@echo "32-bit support: OK"
