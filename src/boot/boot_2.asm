org 0x7E00 ;stage 2 right after stage 1 boot loader

section .bss

; VBE Structure
vbe_info_structure:
	.Signature		resb 4		;	must be 'VESA'
	.Version		resw 1
	.OEMNamePtr		resd 1
	.Capabilities		resd 1

	.VideoModesOffset	resw 1
	.VideoModesSegment	resw 1

	.CountOf64KBlocks	resw 1
	.OEMSoftwareRevision    resw 1
	.OEMVendorNamePtr	resd 1
	.OEMProductNamePtr	resd 1
	.OEMProductRevisionPtr	resd 1
	.Reserved		resb 222
	.OEMData		resb 256

; VBE mode info - upon queryin about each mode, it returns this structure
vbe_mode_structure:				;	VesaModeInfoBlock_size = 256 bytes
	.ModeAttributes		resw 1
	.FirstWindowAttributes	resb 1
	.SecondWindowAttributes	resb 1
	.WindowGranularity	resw 1		;	in KB
	.WindowSize		resw 1		;	in KB
	.FirstWindowSegment	resw 1		;	0 if not supported
	.SecondWindowSegment	resw 1		;	0 if not supported
	.WindowFunctionPtr	resd 1
	.BytesPerScanLine	resw 1

	.Width			resw 1		;	in pixels(graphics)/columns(text)
	.Height			resw 1		;	in pixels(graphics)/columns(text)
	.CharWidth		resb 1		;	in pixels
	.CharHeight		resb 1		;	in pixels
	.PlanesCount		resb 1
	.BitsPerPixel		resb 1
	.BanksCount		resb 1
	.MemoryModel		resb 1
	.BankSize		resb 1		;	in KB
	.ImagePagesCount	resb 1		;	count - 1
	.Reserved1		resb 1		;	equals 0 in Revision 1.0-2.0, 1 in 3.0

	.RedMaskSize		resb 1
	.RedFieldPosition	resb 1
	.GreenMaskSize		resb 1
	.GreenFieldPosition	resb 1
	.BlueMaskSize		resb 1
	.BlueFieldPosition	resb 1
	.ReservedMaskSize	resb 1
	.ReservedMaskPosition	resb 1
	.DirectColorModeInfo	resb 1

	.LFBAddress		resd 1
	.OffscreenMemoryOffset	resd 1
	.OffscreenMemorySize	resw 1		;	in KB
	.Reserved2		resb 206   


section .text

bits 16
; NOTE: Make sure to put real mode code inside bits 16 directive
; Word in 16 bit mode is of 16 bits (2 bytes)

;------------------------------------------------ 16 bits -----------------------------------------

stage_2_start:

; TODO -> Select VBE Mode
set_VBE_mode: ; Refer: https://wiki.osdev.org/VESA_Video_Modes

.get_vesa_info:
    mov ax,0x0
    mov es,ax ;es=0
    mov di, vbe_info_structure

    mov ax,0x4F00
    int 0x10
    cmp al,0x4F
    jne VBE_err

    mov ah,0x0D
    mov al,'A'
    call print_char

; Selecting Mode
;Input: cx = mode
; es:di = targetBuffer (vbe_mode_structure)
.get_vesa_mode_info:
    mov ax,0x0
    mov es,ax ;es=0
    mov di, vbe_mode_structure
    mov cx, [vbe_info_structure.VideoModesOffset]
    mov ax, 0x4F01
    int 0x10

    cmp cx, 0xffff							;	vesa modes list empty
	je NoModes

    mov ah,0x0D
    mov al,'B'
    call print_char

.print_vesa_mode_resolution:
    mov eax,[vbe_mode_structure.Width]
    call print_hex_simple

jmp done

VBE_err:
NoModes:
    mov ah,0x0E
    mov al,'N'
    call print_char

jmp done ;TEMPPP

;-------------------------------------- Jump to protected Mode ---------------------------------
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

;input AL = character
;AH = colour
print_char:
    push di
    push es
    push eax
    mov ax,0xB800
    mov es,ax   ;ds=a 0xB800
    xor di,di   ;di=0
    pop eax
    mov [es:di], ax

    pop es
    pop di
    ret

; Input: EAX = value to print
; Destroys: EBX, ECX, EDI
; Input: EAX = value to print
print_hex_simple:
    pusha
    mov bx, 0xB800
    mov es, bx
    xor di, di

    mov cx, 8
.loop:
    rol eax, 4
    mov bx, ax
    and bx, 0x0F

    cmp bx, 9
    jg .letter
    add bx, '0'
    jmp .write
.letter:
    add bx, 'A' - 10
.write:
    mov [es:di], bl
    mov byte [es:di+1], 0x0F
    add di, 2

    dec cx
    jnz .loop

    popa
    ret



;-------------------------------------------------- 32 bit --------------------------------------

; 32 bit mode directive
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
    mov byte [edi+1], 0x0E

    mov edi, 0xB8000
    mov byte [edi+2], 'A'  ; Print 'A' before reading disk
    mov byte [edi+3], 0x0E



; _____________NOTE______________________
; ONLY READ THE NUMBER OF SECTORS THAT ARE EQUAL TO YOUR DISK SIZE (READING MORE THAN THAT WILL CAUSE ATA TO HANG FOREVER)


;---GET SIZE OF KERNEL--- (Number of sectors to load)
;FFE00
    mov edx, 0xFFE00 ;Put everything at 1MB and so on
    mov edi, edx
    mov ebx, 0x9 ;read from sector 9

    push edi ;preserving edi so it would not get changed by accident
    mov eax, 0x1  ; Read only one sector
    push eax
    push ebx ;sector to start from
    call ata_read
    add esp,12 ;clean parameter (sector no)
    ;[EDI] has data

    ;get no of sectors
    mov eax, [0xFFE00]        ; get the Size of kernel

    cmp eax, 0x0
    xor edx, edx        ; Clear EDX (high 32 bits = 0)
    mov ebx, 512        ; Divisor
    div ebx             ; EAX has answer, edx has remainder
    cmp edx,0
    je no_roofValue
    inc eax ;Roof value of division if have a remainder

no_roofValue:

;----LOAD THE KERNEL----
; Load kernel at 1 MB
; LBA = no. of sector to start reading from
; CL = number of sectors to read ()
    mov edx, 0x100000 ;Put everything at 1MB and so on
    mov edi, edx
    mov ecx, eax ; No of Sectors (calculated early)
    mov ebx, 0xA ;read from sector 10

;push no of sectors
;push sector to start from
.read_sector_by_sector:

    push edi ;preserving edi so it would not get changed by accident
    mov eax, 0x1  ; Read only one sector
    push eax
    push ebx ;sector to start from
    call ata_read
    add esp,12 ;clean parameter (sector no)
    ;[EDI] has data
    inc ebx ;read from next sector

    add edx, 0x200 ;Next memory address
    mov edi, edx

    dec ecx
    cmp ecx,0
    jg .read_sector_by_sector
 
    jmp 0x100000 ;Jump to _start

done:
    cli
.hang:
    hlt             ; halt CPU (simple idle)
    jmp .hang

;The ATA driver
; Ref: https://wiki.osdev.org/ATA_read/write_sectors
ata_read: ;LBA mode

    pushfd ;push 32 bit flags
    push eax
    push ebx
    push ecx
    push edx
    push edi

    mov eax, [esp+28] ; No. of sector to start reading from
    mov ecx, [esp+32] ; Number of sectors to read

    mov ebx, eax         ; Save LBA in EBX
               
    mov edx, 0x01F6      ; Port to send drive and bit 24 - 27 of LBA
    shr eax, 24          ; Get bit 24 - 27 in al
    or al, 11100000b     ; Set bit 6 in al for LBA mode
    out dx, al

    mov edx, 0x01F2      ; Port to send number of sectors
    mov al, cl           ; Get number of sectors from CL
    out dx, al
               
    mov edx, 0x1F3       ; Port to send bit 0 - 7 of LBA
    mov eax, ebx         ; Get LBA from EBX
    out dx, al

    mov edx, 0x1F4       ; Port to send bit 8 - 15 of LBA
    mov eax, ebx         ; Get LBA from EBX
    shr eax, 8           ; Get bit 8 - 15 in AL
    out dx, al


    mov edx, 0x1F5       ; Port to send bit 16 - 23 of LBA
    mov eax, ebx         ; Get LBA from EBX
    shr eax, 16          ; Get bit 16 - 23 in AL
    out dx, al

    mov edx, 0x1F7       ; Command port
    mov al, 0x20         ; Read with retry.
    out dx, al

    push ecx
    ; ADD THIS DEBUG CODE:
    mov ecx, 10000       ; Wait a bit for disk to respond
.delay:
    loop .delay

    pop ecx

.still_going:
    in al, dx
    test al, 8           ; the sector buffer requires servicing.
    jz .still_going      ; until the sector buffer is ready.

    mov eax, 256         ; to read 256 words = 1 sector
    xor bx, bx
    mov bl, cl           ; read CL sectors
    mul bx
    mov ecx, eax         ; RCX is counter for INSW
    mov edx, 0x1F0       ; Data port, in and out
    mov edi, [esp+36]
    cld ;clear direction flag
    rep insw             ; in to [EDI]

    pop edi
    pop edx
    pop ecx
    pop ebx
    pop eax
    popfd
    ret
    

    
; Pad to 4096 bytes (1-8 sectors)
times 4096 - ($ - $$) db 0