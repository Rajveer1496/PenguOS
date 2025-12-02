; vga.asm - prints a string directly to VGA text buffer (real mode)
org 0x7C00
bits 16

start:
    cli
    xor ax, ax
    mov ds, ax         ; DS = 0 (so lodsb reads message in same segment)
    mov si, message

    mov ax, 0xB800
    mov es, ax         ; ES -> VGA segment (0xB800)
    xor di, di         ; start at top-left (offset 0)

.print_char:
    lodsb              ; AL = [DS:SI], SI++
    				   ; al -> lower byte of ax
    				   ; ah -> higher byte of ax
    cmp al, 0
    je .done
    mov ah, 0x0F       ; color: white on black
    mov [es:di], ax    ; store ASCII (AL) then color (AH)
    add di, 2
    jmp .print_char

.done:
    cli
.hang:
    hlt
    jmp .hang

message db "VGA MODE WORKS!", 0

; pad to 510 bytes, then boot signature
times 510 - ($ - $$) db 0
dw 0xAA55
