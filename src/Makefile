# Makefile for our kernel

# Compiler and tools
ASM = nasm
CC = gcc
LD = ld

# Flags for each tool
ASMFLAGS = -f elf32
CFLAGS = -m32 -ffreestanding -nostdlib -fno-pie -O2 -Wall -Wextra
LDFLAGS = -m elf_i386 -T linker.ld

# Output files
KERNEL_BIN = kernel.bin
ISO = myos.iso

# Object files
OBJS = boot.o kernel.o

# Default target - build everything
all: $(ISO)

# Build the ISO image (bootable CD)
$(ISO): $(KERNEL_BIN)
	mkdir -p isodir/boot/grub
	cp $(KERNEL_BIN) isodir/boot/
	echo 'menuentry "PenguOS" {' > isodir/boot/grub/grub.cfg
	echo '    multiboot /boot/$(KERNEL_BIN)' >> isodir/boot/grub/grub.cfg
	echo '}' >> isodir/boot/grub/grub.cfg
	grub2-mkrescue -o $(ISO) isodir

# Link object files into kernel binary
$(KERNEL_BIN): $(OBJS)
	$(LD) $(LDFLAGS) -o $@ $^

# Compile assembly file
boot.o: boot.asm
	$(ASM) $(ASMFLAGS) $< -o $@

# Compile C file
kernel.o: kernel.c
	$(CC) $(CFLAGS) -c $< -o $@

# Run in QEMU
run: $(ISO)
	qemu-system-i386 -cdrom $(ISO)

# Clean build artifacts
clean:
	rm -rf $(OBJS) $(KERNEL_BIN) $(ISO) isodir

# Phony targets (not actual files)
.PHONY: all clean run