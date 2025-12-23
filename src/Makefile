# Makefile for our kernel

# Compiler and tools
ASM = nasm
CC = gcc
LD = ld

# Flags for each tool
ASMFLAGS = -f elf32
CFLAGS = -m32 -ffreestanding -fno-stack-protector -nostdlib -fno-pie -O0 -Wall -Wextra
LDFLAGS = -m elf_i386 -T linker.ld

#FOR INCLUDE FLAG
SUBDIRS = $(shell find . -type d) #Find all sub directories
INCLUDES = $(addprefix -I,$(SUBDIRS))

# Output files
KERNEL_BIN = kernel.bin
ISO = myos.iso

#All c and asm files
C_SOURCES = $(shell find . -name '*.c')
ASM_SOURCES = $(shell find . -name '*.asm')

#changing .c from c files to .o (OBJECT FILES)
OBJS = $(patsubst %.c,%.o,$(C_SOURCES)) $(patsubst %.asm,%.o,$(ASM_SOURCES)) 

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

#VPATH (tells make to look where for source files)
VPATH = memory:debug:graphics:drivers:file_system

#compile all c and asm files
%.o: %.c
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

%.o: %.asm
	$(ASM) $(ASMFLAGS) $< -o $@


# Run in QEMU
run: $(ISO)
	qemu-system-i386 -cdrom $(ISO) -serial stdio \
    -display sdl,grab-mod=rctrl -drive file=disk.img,format=raw

	rm -rf $(OBJS) $(KERNEL_BIN) $(ISO) isodir

# Clean build artifacts
clean:
	rm -rf $(OBJS) $(KERNEL_BIN) $(ISO) isodir

#create disk
createdisk:
	qemu-img create -f raw disk.img 1G

# Phony targets (not actual files)
.PHONY: all clean run


#---GUIDE---
# $@ - represents the target (the thing being built, left side of : )
# $< - represents the source file (right side of : )
# $^ - represents all prerequisites (all files it takes, e.g: (obj) to compile kernel)