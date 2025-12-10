; vga.asm - prints a string directly to VGA text buffer (real mode)
org 0x7C00
bits 16

start:
    cli
    xor ax, ax
    mov ds, ax         ; DS = 0 (so lodsb reads message in same segment)
    mov si, message
    				   ;[DS:SI] to read 20bit address and store it into register

    mov ax, 0xB800
    mov es, ax         ; ES -> VGA segment (0xB800)
    xor di, di         ; start at top-left (offset 0)

.print_char:
;to read from memory we use lodsb
;what it does is it loads whatever value is at memory address [DS:SI] to AL and then increase SI by one
    lodsb              ; AL = [DS:SI], SI++
    				   ; al -> lower byte of ax
    				   ; ah -> higher byte of ax
    cmp al, 0
    je .done
    mov ah, 0x0F       ; color: white on black
    mov [es:di], ax    ; store ASCII (AL) then color (AH)
    				   ; the mov [ES:DI], ax will store whatever value of ax at the address [ES:DI] in memory
    				   ; [ES:DI] used to write at a 20bit memory address
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
