# Build 32-bit kernel with bootloader

# Tools
ASM     := nasm
CC      := gcc
LD      := ld
OBJCOPY := objcopy

# Directories
BOOT    := boot
KERNEL  := kernel
BUILD   := build

# Compiler flags
CFLAGS  := -m32 -std=gnu99 -ffreestanding -O2 -g \
           -Wall -Wextra -fno-builtin -fno-stack-protector \
           -nostdlib -nostartfiles -nodefaultlibs \
           -I$(KERNEL)
LDFLAGS := -m elf_i386 -T $(KERNEL)/linker.ld

# Sources
MBR_SRC    := $(BOOT)/mbr.asm
LOADER_SRC := $(BOOT)/loader.asm
START_SRC  := $(KERNEL)/start.asm
KERNEL_C   := $(sort $(shell find $(KERNEL) -name '*.c'))
KERNEL_ASM := $(sort $(shell find $(KERNEL) -name '*.asm' ! -name 'start.asm'))

# Objects
START_OBJ   := $(patsubst $(KERNEL)/%.asm,$(BUILD)/%.o,$(START_SRC))
KERNEL_OBJS := $(START_OBJ) \
               $(patsubst $(KERNEL)/%.c,$(BUILD)/%.o,$(KERNEL_C)) \
               $(patsubst $(KERNEL)/%.asm,$(BUILD)/%.o,$(KERNEL_ASM))

# Targets
MBR_BIN    := $(BUILD)/mbr.bin
LOADER_BIN := $(BUILD)/loader.bin
KERNEL_ELF := $(BUILD)/kernel.elf
KERNEL_BIN := $(BUILD)/kernel.bin
DISK_IMG   := $(BUILD)/disk.img

.PHONY: all clean run debug check
all: $(DISK_IMG)

$(BUILD):
	mkdir -p $@

$(MBR_BIN): $(MBR_SRC) | $(BUILD)
	$(ASM) -f bin $< -o $@

$(LOADER_BIN): $(LOADER_SRC) | $(BUILD)
	$(ASM) -f bin $< -o $@

$(BUILD)/%.o: $(KERNEL)/%.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD)/%.o: $(KERNEL)/%.asm
	@mkdir -p $(dir $@)
	$(ASM) -f elf32 $< -o $@

$(KERNEL_BIN): $(KERNEL_OBJS) | $(BUILD)
	$(LD) $(LDFLAGS) -o $(KERNEL_ELF) $^
	$(OBJCOPY) -O binary $(KERNEL_ELF) $@

$(DISK_IMG): $(MBR_BIN) $(LOADER_BIN) $(KERNEL_BIN)
	dd if=/dev/zero of=$@ bs=512 count=2880 status=none
	dd if=$(MBR_BIN)    of=$@ bs=512 count=1 conv=notrunc status=none
	dd if=$(LOADER_BIN) of=$@ bs=512 count=1 seek=1 conv=notrunc status=none
	dd if=$(KERNEL_BIN) of=$@ bs=512 seek=2 conv=notrunc status=none

run: $(DISK_IMG)
	qemu-system-i386 -drive format=raw,file=$< -m 32M

debug: $(DISK_IMG)
	qemu-system-i386 -drive format=raw,file=$< -m 32M -s -S

clean:
	rm -rf $(BUILD)

check:
	@which $(CC) >/dev/null || (echo "gcc not found" && exit 1)
	@$(CC) -m32 --version >/dev/null 2>&1 || \
	  (echo "32-bit support missing (install gcc-multilib)" && exit 1)
	@echo "toolchain OK"
