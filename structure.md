* boot.asm : Using grub to boot the kernel (GDT set by GRUB)
* enable_paging.asm : Small Assembly wrapper to toggle paging memory method
* idt_load.asm : Assembly wrapper to load IDT (Interrupt Descriptor Table) for CPU
* io.asm : Functions to read and write from I/O Data ports
* kernel.c : Kernel Initializer
* Keyboard.c : Keyboard driver
* linker.ld : Linker file to combine all object files into one kernel.bin
* memtools.c : Memory management tools
* mouse.c : Mouse driver
* paging.c : Paging initializer
* pic.c : Programmable Interrupt Controller setup
* pmm.c : Physical memory manager (to Manage Pages)
* serial.c : To get serial outputs
* shell.c : VGA Text mode shell with few commands
* vga_draw.c : Drawing helper functions for vga 13h graphics mode
* vga_graphics.c : Vga 13h graphics mode initializer
* image_helper.c : to create/delete and print images on screen