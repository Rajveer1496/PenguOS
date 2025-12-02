; boot.asm - 512-byte bootloader that prints "Hello OS!"
org 0x7C00          ;Tell NASM that our code will start from this memory location- BIOS loads boot sector here 
bits 16

start:
    cli             ; disable interrupts while we set up (optional)
    xor ax, ax
    mov ds, ax      ; set DS = 0 (safe default for this tiny loader)
    mov si, msg     ; SI -> message string

.print:
    lodsb           ; AL = [DS:SI] ; SI++
    cmp al, 0
    je .done
    mov ah, 0x0E    ; BIOS teletype function (TTY) - writes AL to screen and advances cursor
    int 0x10
    jmp .print

.done:
    cli
.hang:
    hlt             ; halt CPU (simple idle)
    jmp .hang

; message (zero-terminated)
msg db "Hello OS!", 0

; pad to 510 bytes, then 0x55AA boot signature (total 512 bytes)
times 510 - ($ - $$) db 0
dw 0xAA55
