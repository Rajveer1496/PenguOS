# Makefile for our kernel

# Compiler and tools
ASM = nasm
CC = gcc
LD = ld

# Flags for each tool
ASMFLAGS = -f elf32
CFLAGS = -m32 -ffreestanding -fno-stack-protector -nostdlib -fno-pie -O0 -Wall -Wextra -Igraphics -Imemory -Idebug -Ifile_system -Idrivers
LDFLAGS = -m elf_i386 -T linker.ld

# Output files
KERNEL_BIN = kernel.bin
ISO = myos.iso

# Object files
OBJS = boot.o kernel.o idt.o idt_load.o interrupt_stubs.o pic.o io.o keyboard.o shell.o pmm.o paging.o enable_paging.o serial.o vga_graphics.o vga_draw.o memtools.o pit.o mouse.o image_helper.o disk_driver.o

# Default target - build everything
all: $(ISO)

# Build the ISO image (bootable CD)
$(ISO): $(KERNEL_BIN)
	mkdir -p isodir/boot/grub
	cp $(KERNEL_BIN) isodir/boot/
	echo 'menuentry "PenguOS" {' > isodir/boot/grub/grub.cfg
	echo '    multiboot /boot/$(KERNEL_BIN)' >> isodir/boot/grub/grub.cfg
	echo '}' >> isodir/boot/grub/grub.cfg
	grub-mkrescue -o $(ISO) isodir

# Link object files into kernel binary
$(KERNEL_BIN): $(OBJS)
	$(LD) $(LDFLAGS) -o $@ $^

# Compile assembly file
boot.o: boot.asm
	$(ASM) $(ASMFLAGS) $< -o $@

enable_paging.o: memory/enable_paging.asm
	$(ASM) $(ASMFLAGS) $< -o $@

# Compile C file
kernel.o: kernel.c
	$(CC) $(CFLAGS) -c $< -o $@

paging.o: memory/paging.c
	$(CC) $(CFLAGS) -c $< -o $@

serial.o: debug/serial.c
	$(CC) $(CFLAGS) -c $< -o $@

vga_graphics.o: graphics/vga_graphics.c
	$(CC) $(CFLAGS) -c $< -o $@

vga_draw.o: graphics/vga_draw.c
	$(CC) $(CFLAGS) -c $< -o $@

memtools.o: memory/memtools.c
	$(CC) $(CFLAGS) -c $< -o $@

pit.o: drivers/pit.c
	$(CC) $(CFLAGS) -c $< -o $@

image_helper.o: graphics/image_helper.c
	$(CC) $(CFLAGS) -c $< -o $@

mouse.o: drivers/mouse.c
	$(CC) $(CFLAGS) -c $< -o $@

disk_driver.o: file_system/disk_driver.c
	$(CC) $(CFLAGS) -c $< -o $@

# Compile shell C file
shell.o: debug/shell.c
	$(CC) $(CFLAGS) -c $< -o $@

# Compile paging file
pmm.o: memory/pmm.c
	$(CC) $(CFLAGS) -c $< -o $@

# Compile IDT C file
idt.o: drivers/idt.c
	$(CC) $(CFLAGS) -c $< -o $@

# Compile PIC C file
pic.o: drivers/pic.c
	$(CC) $(CFLAGS) -c $< -o $@

# Compile keyboard C file
keyboard.o: drivers/keyboard.c
	$(CC) $(CFLAGS) -c $< -o $@

# Compile IDT loader assembly
idt_load.o: drivers/idt_load.asm
	$(ASM) $(ASMFLAGS) $< -o $@

# Compile interrupt stubs assembly
interrupt_stubs.o: drivers/interrupt_stubs.asm
	$(ASM) $(ASMFLAGS) $< -o $@

# Compile I/O assembly
io.o: drivers/io.asm
	$(ASM) $(ASMFLAGS) $< -o $@

# Run in QEMU
run: $(ISO)
	qemu-system-i386 -cdrom $(ISO) -serial stdio \
    -display sdl,grab-mod=rctrl

	#rm -rf $(OBJS) $(KERNEL_BIN) $(ISO) isodir

# Clean build artifacts
clean:
	rm -rf $(OBJS) $(KERNEL_BIN) $(ISO) isodir

# Phony targets (not actual files)
.PHONY: all clean run