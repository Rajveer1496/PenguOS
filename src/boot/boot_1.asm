; BOOT LOADER STAGE 1 -> 512 bytes, basic setup and jump to stage 2
; STAGE 2 -> setup GDT , ATA Driver, load Kernel, jump to kernel

; Reason for 2 stages: Bootloader can only be 512 bytes for bios to load it, so to have space to devlop boot loader further more in future we are loading remaining boot loader part in stage 2 in memory

org 0x7C00          ;Tell NASM that our code will start from this memory location- BIOS loads boot sector here 
bits 16

stage_1_start:
    cli             ; disable interrupts while we set up (optional)
    xor ax, ax
    mov ds, ax      ; set DS = 0 (safe default for this tiny loader)
    mov es, ax
    mov ss, ax
    mov sp, 0x7C00 ;Temporary stack in bootloader area


load_stage_2:
    ; mov dl, 0x0 ;set by BIOS
    xor ax,ax
    mov ds, ax
    mov si, DAP
    mov ah, 0x42
    int 0x13

    ;wait for a bit
    ; cmp ah,0 ;error checking
    ; jne load_stage_2

    jmp 0x7E00 ;Stage 2 address


DAP: ; Data address packet (for INT 0x13h disk reading, help of BIOS)
    db 0x10   ; size of packet
    db 0x0    ; always 0
    dw 0x8    ; no. of sectors to transfer

    dw 0x7E00  ;offset of where to transfer (segment:offset layout, offset first cuz its little endian architecture)
    dw 0x0  ;Segment of where to transfer

    ; Lower 32 bit of 48-bit starting LBA
    dd 0x1 ;Starting from 1st sector (stage 1 is in sector 0)

    ; Upper 16-bits of Starting LBA (lower 16 bit are used from 32 bit thats why dd - upper once are used for allignment purposes)
    dd 0x0

; Ref: https://wiki.osdev.org/Disk_access_using_the_BIOS_(INT_13h)

done:
    cli
.hang:
    hlt             ; halt CPU (simple idle)
    jmp .hang


; pad to 510 bytes, then 0x55AA boot signature (total 512 bytes)
times 510 - ($ - $$) db 0
dw 0xAA55