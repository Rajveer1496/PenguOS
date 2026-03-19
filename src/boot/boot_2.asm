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

%macro PRINT_STRING 4
    push dword %4
    push dword %3
    push dword %2
    push dword %1
    call print_string
    add esp,16
%endmacro

%macro PRINT_HEX 3
    push word %3
    push word %2
    push word %1
    call print_hex_simple
    add esp,6 ;cleanup
%endmacro

%macro PRINT_HEX_32 3
    push word %3
    push word %2
    push dword %1
    call print_hex_32
    add esp,8 ;cleanup
%endmacro

%macro GET_VESA_MODE_INFO 1
    push dword %1
    call get_vesa_mode_info
    add esp,2
%endmacro

stage_2_start:

; TODO -> Select VBE Mode
set_VBE_mode: ; Refer: https://wiki.osdev.org/VESA_Video_Modes

    call vga_clear

; NOTE:
; VBE Adress at = 0x500
; Width at = 0x505
; Height at = 0x507
; Bitsper pixel at = 0x509

.print_vesa_mode_resolution: ; Note: Height and width are 16 bit numbers

    call get_vesa_info

    PRINT_STRING message,0,1,10

    GET_VESA_MODE_INFO 0

    PRINT_STRING msg_MODE,0,2,4
    PRINT_STRING msg_RESOLUTION,6,2,10
    PRINT_STRING msg_BITS,19,2,14
    PRINT_STRING msg_LFB_ADDRESS,34,2,11

    mov cx,66
    mov eax,3 ; y pos
    .print_all_modes:

        GET_VESA_MODE_INFO cx

        ;print MODE
        push bx
        push ax
        mov ax,cx
        mov bx,0x2
        mul bx
        mov bx, [vbe_info_structure.VideoModesOffset]
        add bx,ax
        pop ax
        PRINT_HEX [bx],0,ax
        mov [0x550],bx ;preserve Mode at safe address
        pop bx

        PRINT_HEX [vbe_mode_structure.Width],6,ax ;Resolution width

        PRINT_STRING msg_cross,11,eax,1

        PRINT_HEX [vbe_mode_structure.Height],13,ax ;Resolution Height
                
        xor bx,bx
        mov bl,[vbe_mode_structure.BitsPerPixel]
        PRINT_HEX bx,24,ax ;Bits per pixel

        PRINT_HEX_32 [vbe_mode_structure.LFBAddress],34,ax
        
        ;get desried mode
        push eax
        mov ax,[vbe_mode_structure.Width]
        cmp ax,0x500 ;width
        jne .next_mode

        mov ax,[vbe_mode_structure.Height]
        cmp ax,0x2D0 ;Height
        jne .next_mode

        mov al,[vbe_mode_structure.BitsPerPixel]
        cmp al,0x18 ;Bits per pixel
        jne .next_mode

        mov eax, [vbe_mode_structure.LFBAddress]
        mov [0x500], eax ;Save VBE address

        mov ax,[vbe_mode_structure.Width]
        mov [0x505], ax ;Save Width

        mov ax,[vbe_mode_structure.Height]
        mov [0x507], ax ;save Height

        mov al,[vbe_mode_structure.BitsPerPixel]
        mov [0x509], al ;save Bits per pixel

        PRINT_HEX_32 [0x500],50,2

        PRINT_HEX [0x550],50,4

        mov bx,[0x550]
        push word [bx]
        call set_vesa_mode
        add esp,2 ;clean
        
        jmp loadGDT


        .next_mode:
            pop eax
            inc eax
            inc cx
            cmp cx,88
            jne .print_all_modes


jmp done

msg_cross db "x",0
msg_MODE db "MODE",0
msg_RESOLUTION db "RESOLUTION",0
msg_BITS db "BITS_PER_PIXEL",0
msg_MAX_COLORS db "MAXIMUM_COLORS",0
msg_LFB_ADDRESS db "LFB_ADDRESS",0

get_vesa_info:
    pushad
    mov ax,0x0
    mov es,ax ;es=0
    mov di, vbe_info_structure

    mov ax,0x4F00
    int 0x10
    cmp al,0x4F
    jne VBE_err

    mov ah,0x0D
    popad
ret

; Selecting Mode
; 1st para no. of mode
get_vesa_mode_info:
    pushad ;32
    mov ax,0x0
    mov es,ax ;es=0
    mov di, vbe_mode_structure

    mov ax,[esp+34]
    mov bx,0x2
    mul bx
    mov bx, [vbe_info_structure.VideoModesOffset]
    add bx,ax
    mov cx, [bx]
    mov ax, 0x4F01
    int 0x10

    cmp cx, 0xffff							;	vesa modes list empty
	je NoModes

    xor di,di
    push 0x0D
    push 'B'
    call print_char
    add esp,4
    
    popad
ret

; 1st para= Mode number (16 bits)
set_vesa_mode:
    pushad

    mov ax,0x4F02
    ;0x18E mode -> 1280x720x24
    mov bx,[esp+34] ;bits 0-13 mode number, bit 14 (LFB) enables linear frame buffer, bit 15 (DM) to clear screen
    or bx,0x4000 ;enable LFB
    mov di,0
    int 0x10

    cmp ax, 0x004F	; test for error
    jne VBE_err

    popad
ret



VBE_err:
NoModes:
    PRINT_STRING err_msg,0,0,26

jmp done ;TEMPPP

err_msg db "ERROR: CANT GET VBE MODES!",0

message db "VBE MODES:",0

loadGDT:
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

; di = offset
; 2nd para = color
; 1st para = charachter
print_char:
    push eax
    push es
    mov ax,0xB800
    mov es,ax   ;ds=a 0xB800
    
    mov al,[esp + 8] ; charachter
    mov ah,[esp + 10] ; color
    mov [es:di], ax

    pop es
    pop eax
ret

; 4th para = length to string
; 3rd para = posY
; 2nd para = posX
; 1st para = address of string

print_string:
    pushad

    .getPos: 
        xor di, di ;offset
        mov eax,[VGA_WIDTH]
        mov ebx,[esp+42] ; Y pos
        mul ebx
        mov ebx,[esp+38] ; X pos
        add eax,ebx

        xor ebx,ebx
        mov ebx,0x2
        mul ebx
        
        mov di,ax ;final offset

    ;TODO print whole string

    mov cx,[esp+46] ; lenght of string
    mov eax,[esp+34] ; Address of string

    .print_one_char:
        push 0x0E
        push word [eax]
        call print_char
        add esp,4

        inc eax
        add di,2

        dec cx
        jnz .print_one_char

    popad
ret

vga_clear:
    pushad
    mov cx,2000
    xor di,di

    .print_one_char:
    push 0x0E
    push ' '
    call print_char
    add esp,4

    add di,2

    dec cx
    jnz .print_one_char

    popad
ret


; 4th para = length to print
; 3rd para = posY
; 2nd para = posX
; 1st para = Value to print
VGA_WIDTH dd 80
print_hex_simple:
    pushad ; Push EAX, ECX, EDX, EBX, ESP, EBP, ESI, EDI

    mov bx, 0xB800
    mov es, bx ;es = 0xB800
    .getPos: 
        xor di, di ;offset
        mov ax,[VGA_WIDTH]
        mov bx,[esp+38] ; Y pos
        mul bx
        mov bx,[esp+36] ; X pos
        add ax,bx

        xor bx,bx
        mov bx,0x2
        mul bx
        
        mov di,ax ;final offset


    mov ax, [esp+34] ;value to print
    mov cx, 4
    .loop:
        rol ax, 4
        mov bx, ax
        and bx, 0x0F

        cmp bx, 9
        jg .letter
        add bx, '0'
        jmp .write
    .letter:
        add bx, 'A' - 10
    .write:
        mov [es:di], bl ; Address: es * 0x10 + di = 0xB8000 + 0
        mov byte [es:di+1], 0x0F
        add di, 2

    dec cx
    jnz .loop
        
    popad
ret

; 3rd para = posY
; 2nd para = posX
; 1st para = Value to print
print_hex_32:
    pushad ; Push EAX, ECX, EDX, EBX, ESP, EBP, ESI, EDI

    mov bx, 0xB800
    mov es, bx ;es = 0xB800
    .getPos: 
        xor di, di ;offset
        mov ax,[VGA_WIDTH]
        mov bx,[esp+40] ; Y pos
        mul bx
        mov bx,[esp+38] ; X pos
        add ax,bx

        xor bx,bx
        mov bx,0x2
        mul bx
        
        mov di,ax ;final offset


    mov eax, [esp+34] ;value to print
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
        mov [es:di], bl ; Address: es * 0x10 + di = 0xB8000 + 0
        mov byte [es:di+1], 0x0F
        add di, 2

    dec cx
    jnz .loop
        
    popad
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