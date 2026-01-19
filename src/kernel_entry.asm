; Reserve stack space (16KB)
section .bss
align 16
; stack_bottom:
;     resb 16384                    ; 16 KB stack
; stack_top:

; Entry point - GRUB will jump here after loading kernel
section .text
global _start
extern kernel_main                ; Our C function

_start:
    mov edi, 0xB8000
    mov byte [edi+6], 'C'
    mov byte [edi+7], 0x0E

    ; Set up stack pointer
    mov esp, 0x200000 ;Hardcoded stack at 2 MB

    mov edi, 0xB8000
    mov byte [edi+8], 'D'
    mov byte [edi+9], 0x0E
    
    ; Call our C kernel main function
    call kernel_main

    mov edi, 0xB8000
    mov byte [edi+10], 'E'
    mov byte [edi+11], 0x0E
    
    ; If kernel_main returns, hang the system
    cli
.hang:
    hlt
    jmp .hang