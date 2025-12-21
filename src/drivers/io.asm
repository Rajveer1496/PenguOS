; io.asm - I/O port read/write functions

section .text

; void outb(uint16_t port, uint8_t value)
; Write a byte to an I/O port
global outb
outb:
    ;first parameter
    mov dx, [esp + 4]    ; port number (16-bit)
    ;second parameter
    mov al, [esp + 8]    ; value to write (8-bit)
    out dx, al           ; Send AL to port DX
    ret

; uint8_t inb(uint16_t port)
; Read a byte from an I/O port
global inb
inb:
    mov dx, [esp + 4]    ; port number (16-bit)
    in al, dx            ; Read from port DX into AL
    ret                  ; Return value is in AL


global outw
outw:
    ;first parameter
    mov dx, [esp + 4]    ; port number (16-bit)
    ;second parameter
    mov ax, [esp + 8]    ; value to write (16-bit)
    out dx, ax           ; Send AX to port DX
    ret

; uint16_t inw(uint16_t port)
; Read a word (2 bytes) from an I/O port
global inw
inw:
    mov dx, [esp + 4]    ; port number (16-bit)
    in ax, dx            ; Read from port DX into AX
    ret                  ; Return value is in AX