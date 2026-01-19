/* linker.ld - Linker script for our kernel */

/* Entry point - where execution starts */
ENTRY(_start)

/* Our kernel will be loaded at 1MB (0x100000) */
SECTIONS
{
    /* Tell linker to place everything starting at 1MB */
    . = 0x100000;

    /* .text section - executable code */
    .text : ALIGN(4096)
    {   
        kernel_entry.o(.text)
        *(.text)         /* _start will be here first */
        /* All code sections from all object files */
    }

    /* .rodata section - read-only data (strings, constants) */
    .rodata : ALIGN(4096)
    {
        *(.rodata)
    }

    /* .data section - initialized global/static variables */
    .data : ALIGN(4096)
    {
        *(.data)
    }

    /* .bss section - uninitialized global/static variables */
    .bss : ALIGN(4096)
    {
        *(.bss)
        *(COMMON)
    }
    _kernel_end = .;  /* This symbol = address where kernel ends! */
}

/* 
1. You write: idt.c, keyboard.c, paging.c, etc.
2. Compiler compiles each into: idt.o, keyboard.o, paging.o, etc.
3. Linker combines them all into: kernel.bin
*/