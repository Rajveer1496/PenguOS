; boot.asm - Enter protected mode and print to VGA
; Assemble: nasm -f bin boot.asm -o boot.bin
; Run: qemu-system-i386 -drive format=raw,file=boot.bin

org 0x7C00
bits 16

start:
    cli                    ; Disable interrupts during setup
    xor ax, ax
    mov ds, ax
    mov es, ax
    mov ss, ax
    mov sp, 0x7C00        ; Set up stack below bootloader

    ; Print message in real mode
    mov si, msg_real
    call print_real_mode

    ; === STEP 1: Load the GDT ===
    lgdt [gdt_descriptor]  ; Load GDT descriptor into GDTR register

    ; === STEP 2: Enable Protected Mode ===
    mov eax, cr0           ; Read CR0 (Control Register 0)
    or eax, 1              ; Set bit 0 (PE - Protection Enable)
    mov cr0, eax           ; Write back to CR0
    
    ; === STEP 3: Far jump to reload CS ===
    ; This jump does two things:
    ; 1. Flushes the CPU pipeline
    ; 2. Loads CS with the code segment selector (0x08)
    jmp 0x08:protected_mode

; --- Real Mode Functions ---
print_real_mode:
    lodsb                  ; Load byte from DS:SI into AL
    cmp al, 0
    je .done
    mov ah, 0x0E           ; BIOS teletype
    int 0x10
    jmp print_real_mode
.done:
    ret

; ============================================
; 32-BIT PROTECTED MODE CODE STARTS HERE
; ============================================
bits 32
protected_mode:
    
    ; === STEP 4: Reload all segment registers ===
    ; We must reload ALL segment registers with valid selectors
    ; 0x10 = offset of Data Segment in GDT (entry 2)
    mov ax, 0x10
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    mov ss, ax
    
    ; Set up a new stack in protected mode
    mov esp, 0x90000       ; Stack grows down from 0x90000

    ; Print success message to VGA
    mov esi, msg_protected
    mov edi, 0xB8000       ; VGA text buffer
    call print_protected_mode

    ; Infinite loop
.hang:
    hlt
    jmp .hang

; Print string to VGA in protected mode
; ESI = pointer to null-terminated string
; EDI = VGA buffer address
print_protected_mode:
    mov ah, 0x0E           ; White on black
.loop:
    lodsb                  ; Load byte from DS:ESI into AL
    cmp al, 0
    je .done
    mov [edi], ax          ; Write char (AL) and color (AH)
    add edi, 2
    jmp .loop
.done:
    ret

; ============================================
; DATA SECTION
; ============================================
msg_real: db "Real mode OK. Entering protected mode...", 13, 10, 0
msg_protected: db "Protected Mode Active!", 0

; ============================================
; GDT (Global Descriptor Table)
; ============================================
; Each entry is 8 bytes with this structure:
; Bytes 0-1: Limit (bits 0-15)
; Bytes 2-4: Base (bits 0-23)
; Byte 5: Access Byte
; Byte 6: Limit (bits 16-19) + Flags
; Byte 7: Base (bits 24-31)

align 4
gdt_start:

; Entry 0: Null Descriptor (required, must be all zeros)
gdt_null:
    dd 0x00000000          ; 4 bytes of zeros
    dd 0x00000000          ; 4 bytes of zeros

; Entry 1: Code Segment (selector 0x08)
; Base = 0x00000000, Limit = 0xFFFFF
; Access = 0x9A (Present, Ring 0, Code, Execute/Read)
; Flags = 0xC (Granularity=4KB, 32-bit)
gdt_code:
    dw 0xFFFF              ; Limit (bits 0-15)
    dw 0x0000              ; Base (bits 0-15)
    db 0x00                ; Base (bits 16-23)
    db 0x9A                ; Access: 1001 1010
                           ; P=1 (present)
                           ; DPL=00 (ring 0)
                           ; S=1 (code/data segment)
                           ; Type=1010 (code, execute/read)
    db 0xCF                ; Flags + Limit: 1100 1111
                           ; G=1 (4KB granularity)
                           ; D=1 (32-bit)
                           ; L=0 (not 64-bit)
                           ; Limit (bits 16-19) = 0xF
    db 0x00                ; Base (bits 24-31)

; Entry 2: Data Segment (selector 0x10)
; Base = 0x00000000, Limit = 0xFFFFF
; Access = 0x92 (Present, Ring 0, Data, Read/Write)
; Flags = 0xC (Granularity=4KB, 32-bit)
gdt_data:
    dw 0xFFFF              ; Limit (bits 0-15)
    dw 0x0000              ; Base (bits 0-15)
    db 0x00                ; Base (bits 16-23)
    db 0x92                ; Access: 1001 0010
                           ; P=1 (present)
                           ; DPL=00 (ring 0)
                           ; S=1 (code/data segment)
                           ; Type=0010 (data, read/write)
    db 0xCF                ; Flags + Limit (same as code)
    db 0x00                ; Base (bits 24-31)

gdt_end:

; GDT Descriptor (loaded with LGDT)
; This tells the CPU where the GDT is and how big it is
gdt_descriptor:
    dw gdt_end - gdt_start - 1  ; Size of GDT (minus 1)
    dd gdt_start                ; Linear address of GDT

; ============================================
; BOOT SIGNATURE
; ============================================
times 510 - ($ - $$) db 0
dw 0xAA55