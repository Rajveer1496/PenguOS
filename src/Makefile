# Makefile for our kernel

# Compiler and tools
ASM = nasm
CC = gcc
LD = ld

# Flags for each tool
ASMFLAGS = -f elf32
CFLAGS = -g -m32 -ffreestanding -fno-stack-protector -nostdlib -fno-pie -O0 -Wall -Wextra
LDFLAGS = -m elf_i386 -T linker.ld

#FOR INCLUDE FLAG
SUBDIRS = $(shell find . -type d) #Find all sub directories
INCLUDES = $(addprefix -I,$(SUBDIRS))

# Output files
KERNEL_BIN = kernel.bin
BOOTLOADER_IMG = bootloader.img
BOOT_1_BIN = boot/boot_1.bin
BOOT_2_BIN = boot/boot_2.bin
BOOT_O = boot.o

#To create a sector (512 bytes) of kernel size
FILE=kernel.bin
SIZE=$(shell stat -L -c %s $(FILE)) #calculating kernel File size

PADDED_KERNEL_SIZE_BIN = padded_kernel_size.bin
PADDING_FILE_BIN = paddingFile.bin
KERNEL_SIZE_BIN = kernel_size.bin

#All c and asm files
C_SOURCES = $(shell find . -name '*.c')
ASM_SOURCES = $(shell find . -name '*.asm' ! -path './boot/*') # Dont link the bootloader, it has to be seperate

#changing .c from c files to .o (OBJECT FILES)
OBJS = $(patsubst %.c,%.o,$(C_SOURCES)) $(patsubst %.asm,%.o,$(ASM_SOURCES)) 

# Default target - build everything
all: $(BOOTLOADER_IMG)

# Build the ISO image (bootable CD)
# Combine into bootable image
$(BOOTLOADER_IMG): $(BOOT_1_BIN) $(BOOT_2_BIN) $(KERNEL_BIN) $(PADDED_KERNEL_SIZE_BIN)
	cat $(BOOT_1_BIN) $(BOOT_2_BIN) $(PADDED_KERNEL_SIZE_BIN) $(KERNEL_BIN) > $@

#create padded kernel size sector

$(PADDED_KERNEL_SIZE_BIN): $(KERNEL_SIZE_BIN) $(PADDING_FILE_BIN)
	cat $(KERNEL_SIZE_BIN) $(PADDING_FILE_BIN) > $(PADDED_KERNEL_SIZE_BIN)

$(PADDING_FILE_BIN):
	dd if=/dev/zero of=$(PADDING_FILE_BIN) bs=508 count=1

# storing it in little endian (using xxd with byte reversal)
$(KERNEL_SIZE_BIN): $(KERNEL_BIN)
	printf '%08x' $(SIZE) | sed 's/\(..\)\(..\)\(..\)\(..\)/\4\3\2\1/' | xxd -r -p > $(KERNEL_SIZE_BIN)

# Link to ELF first (keeps symbols for debugging)
kernel.elf: $(OBJS)
	$(LD) $(LDFLAGS) -o $@ kernel_entry.o $(filter-out kernel_entry.o, $^)

# Extract flat binary from ELF (for bootloader)
$(KERNEL_BIN): kernel.elf
	objcopy -O binary kernel.elf $(KERNEL_BIN)

#VPATH (tells make to look where for source files)
VPATH = memory:debug:graphics:drivers:file_system

# Build bootloader binaries
$(BOOT_1_BIN): boot/boot_1.asm
	$(ASM) -f bin $< -o $@

$(BOOT_2_BIN): boot/boot_2.asm
	$(ASM) -f bin $< -o $@

#compile all c and asm files
%.o: %.c
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

%.o: %.asm
	$(ASM) $(ASMFLAGS) $< -o $@


# Run in QEMU
run: $(BOOTLOADER_IMG)
	qemu-system-i386 -enable-kvm -drive file=bootloader.img,format=raw -drive file=disk.img,format=raw -serial stdio \
    -display sdl,grab-mod=rctrl

	rm -rf $(OBJS) $(KERNEL_BIN) $(BOOT_1_BIN) $(BOOT_2_BIN) $(BOOT_O) $(BOOTLOADER_IMG) $(KERNEL_SIZE_BIN) $(PADDED_KERNEL_SIZE_BIN) $(PADDING_FILE_BIN) kernel.elf

# Clean build artifacts
clean:
	rm -rf $(OBJS) $(KERNEL_BIN) $(BOOT_1_BIN) $(BOOT_2_BIN) $(BOOT_O) $(BOOTLOADER_IMG) $(KERNEL_SIZE_BIN) $(PADDED_KERNEL_SIZE_BIN) $(PADDING_FILE_BIN) kernel.elf

#create disk
createdisk:
	qemu-img create -f raw disk.img 1G

check_size:
	@echo "Size of $(FILE) is $(SIZE) bytes"

# Phony targets (not actual files)
.PHONY: all clean run


#---GUIDE---
# $@ - represents the target (the thing being built, left side of : )
# $< - represents the source file (right side of : )
# $^ - represents all prerequisites (all files it takes, e.g: (obj) to compile kernel)