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
OBJS = boot.o kernel.o idt.o idt_load.o interrupt_stubs.o pic.o io.o keyboard.o shell.o pmm.o paging.o enable_paging.o serial.o vga_graphics.o

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

enable_paging.o: enable_paging.asm
	$(ASM) $(ASMFLAGS) $< -o $@

# Compile C file
kernel.o: kernel.c
	$(CC) $(CFLAGS) -c $< -o $@

paging.o: paging.c
	$(CC) $(CFLAGS) -c $< -o $@

serial.o: serial.c
	$(CC) $(CFLAGS) -c $< -o $@

vga_graphics.o: vga_graphics.c
	$(CC) $(CFLAGS) -c $< -o $@

# Compile shell C file
shell.o: shell.c
	$(CC) $(CFLAGS) -c $< -o $@

# Compile paging file
pmm.o: pmm.c
	$(CC) $(CFLAGS) -c $< -o $@

# Compile IDT C file
idt.o: idt.c
	$(CC) $(CFLAGS) -c $< -o $@

# Compile PIC C file
pic.o: pic.c
	$(CC) $(CFLAGS) -c $< -o $@

# Compile keyboard C file
keyboard.o: keyboard.c
	$(CC) $(CFLAGS) -c $< -o $@

# Compile IDT loader assembly
idt_load.o: idt_load.asm
	$(ASM) $(ASMFLAGS) $< -o $@

# Compile interrupt stubs assembly
interrupt_stubs.o: interrupt_stubs.asm
	$(ASM) $(ASMFLAGS) $< -o $@

# Compile I/O assembly
io.o: io.asm
	$(ASM) $(ASMFLAGS) $< -o $@

# Run in QEMU
run: $(ISO)
	# rm -rf $(OBJS) $(KERNEL_BIN)
	qemu-system-i386 -cdrom $(ISO) -serial stdio

# Clean build artifacts
clean:
	rm -rf $(OBJS) $(KERNEL_BIN) $(ISO) isodir

# Phony targets (not actual files)
.PHONY: all clean run