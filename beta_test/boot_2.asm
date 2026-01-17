org 0x7E00 ;stage 2 right after stage 1 boot loader
bits 16
stage_2_start:

lgdt [gdt_descriptor] ;LOAD GDT

    mov eax, cr0      ; Read CR0
    or eax, 0x1       ; Set bit 0 (Protected Mode Enable)
    mov cr0, eax      ; Write back to CR0

jmp CODE_SEG:protected_mode_setup

; --- GDT ---
gdt_start:
    ; Null descriptor (required)
    dq 0x0000000000000000

gdt_code:
    ; Code segment descriptor
    ; Base: 0x00000000, Limit: 0xFFFFF
    ; Access: 10011010b = 0x9A
    ;   Present=1, Privilege=00, Type=1, Executable=1, 
    ;   Conforming=0, Readable=1, Accessed=0
    ; Flags: 1100b = 0xC
    ;   Granularity=1 (4KB), Size=1 (32-bit)
    
    dw 0xFFFF       ; Limit (bits 0-15)
    dw 0x0000       ; Base (bits 0-15)
    db 0x00         ; Base (bits 16-23)
    db 0x9A         ; Access byte
    db 0xCF         ; Flags (4 bits) + Limit (bits 16-19)
    db 0x00         ; Base (bits 24-31)

gdt_data:
    ; Data segment descriptor
    ; Same as code but Access = 10010010b = 0x92
    ;   Executable=0, Writable=1
    
    dw 0xFFFF       ; Limit
    dw 0x0000       ; Base
    db 0x00         ; Base
    db 0x92         ; Access byte
    db 0xCF         ; Flags + Limit
    db 0x00         ; Base

gdt_end:

; GDT Descriptor (what we load into GDTR)
gdt_descriptor:
    dw gdt_end - gdt_start - 1    ; Size of GDT (16-bit)
    dd gdt_start                   ; Address of GDT (32-bit)

; Define selector constants
CODE_SEG equ gdt_code - gdt_start  ; 0x08
DATA_SEG equ gdt_data - gdt_start  ; 0x10


bits 32
protected_mode_setup:
    ; Load data segment selector into all data segment registers
    mov ax, DATA_SEG    ; 0x10
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    mov ss, ax
    

protected_mode:

    mov edi, 0xB8000 ;VGA text buffer
    mov byte [edi], 'Y'
    mov byte [edi+1], 0x0F

;The ATA driver
;Load kernel at 1 MB


done:
    cli
.hang:
    hlt             ; halt CPU (simple idle)
    jmp .hang