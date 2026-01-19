; boot.asm - Multiboot header for GRUB
; This tells GRUB: "Hey, I'm a valid kernel!"

section .multiboot
align 4

; Multiboot specification constants
MAGIC    equ 0x1BADB002           ; Magic number (GRUB looks for this)
FLAGS    equ 0x00000003           ; Flags (align modules on page boundaries + provide memory map)
CHECKSUM equ -(MAGIC + FLAGS)     ; Checksum (must equal -(MAGIC + FLAGS))

; The multiboot header (MUST be in first 8KB of kernel file)
dd MAGIC
dd FLAGS  
dd CHECKSUM

; Reserve stack space (16KB)
section .bss
align 16
stack_bottom:
    resb 16384                    ; 16 KB stack
stack_top:

; Entry point - GRUB will jump here after loading kernel
section .text
global _start
extern kernel_main                ; Our C function

_start:
    ; Set up stack pointer
    mov esp, stack_top
    
    ; Call our C kernel main function
    call kernel_main
    
    ; If kernel_main returns, hang the system
    cli
.hang:
    hlt
    jmp .hang