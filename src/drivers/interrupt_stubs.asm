; interrupt_stubs.asm - Interrupt Service Routine stubs

section .text

; External C function that will handle interrupts
extern interrupt_handler

extern JustPrint ;DEBUG

; Common interrupt handler code
; This saves all registers, calls C handler, then restores everything
interrupt_common:
    ; Save all registers
    pusha                  ; Push EAX, ECX, EDX, EBX, ESP, EBP, ESI, EDI
    
    ; Save segment registers
    push ds
    push es
    push fs
    push gs
    
    ; Load kernel data segment (0x18) -> GRUB'S GDT
    mov ax, 0x18
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax


    ; Push interrupt number as parameter to C function
    mov eax, [esp + 48]  ; Get interrupt number (after all pushes)
    push eax
    
    ; Call C interrupt handler
    ; Stack now contains all saved registers
    call interrupt_handler

    add esp, 4  ; Clean up parameter
    
    ; Restore segment registers
    pop gs
    pop fs
    pop es
    pop ds
    
    ; Restore all registers
    popa
    
    ; Clean up the interrupt number and error code we pushed
    add esp, 8 ;(Moved the stack pointer to the return address)
    
    ; Return from interrupt (CPU BLOCKS ALL INTERRUPTS UNTIL ONE IS HANDLED AND iret is called)
    iret

; Macro to create interrupt stub WITHOUT error code
; CPU doesn't push error code for these interrupts
%macro ISR_NOERR 1
global isr%1
isr%1:
    push 0                 ; Push dummy error code
    push %1                ; Push interrupt number
    jmp interrupt_common
%endmacro

; Macro to create interrupt stub WITH error code
; CPU automatically pushes error code for these
%macro ISR_ERR 1
global isr%1
isr%1:
    ; CPU already pushed error code
    push %1                ; Push interrupt number
    jmp interrupt_common
%endmacro

; Macro for hardware interrupt (IRQ)
%macro IRQ 2
global irq%1
irq%1:
    push 0                 ; Dummy error code
    push %2                ; Push interrupt number (32 + IRQ number)
    jmp interrupt_common
%endmacro

; CPU Exception ISRs (0-31)
ISR_NOERR 0     ; Divide by zero
ISR_NOERR 1     ; Debug
ISR_NOERR 2     ; Non-maskable interrupt
ISR_NOERR 3     ; Breakpoint
ISR_NOERR 4     ; Overflow
ISR_NOERR 5     ; Bound range exceeded
ISR_NOERR 6     ; Invalid opcode
ISR_NOERR 7     ; Device not available
ISR_ERR   8     ; Double fault (has error code)
ISR_NOERR 9     ; Coprocessor segment overrun
ISR_ERR   10    ; Invalid TSS (has error code)
ISR_ERR   11    ; Segment not present (has error code)
ISR_ERR   12    ; Stack-segment fault (has error code)
ISR_ERR   13    ; General protection fault (has error code)
ISR_ERR   14    ; Page fault (has error code)
ISR_NOERR 15    ; Reserved
ISR_NOERR 16    ; x87 floating point exception
ISR_ERR   17    ; Alignment check (has error code)
ISR_NOERR 18    ; Machine check
ISR_NOERR 19    ; SIMD floating point exception
ISR_NOERR 20    ; Virtualization exception
ISR_NOERR 21    ; Reserved
ISR_NOERR 22    ; Reserved
ISR_NOERR 23    ; Reserved
ISR_NOERR 24    ; Reserved
ISR_NOERR 25    ; Reserved
ISR_NOERR 26    ; Reserved
ISR_NOERR 27    ; Reserved
ISR_NOERR 28    ; Reserved
ISR_NOERR 29    ; Reserved
ISR_ERR   30    ; Security exception (has error code)
ISR_NOERR 31    ; Reserved

; Hardware IRQs (32-47)
; IRQ 0-15 get mapped to interrupts 32-47
IRQ 0, 32       ; Timer
IRQ 1, 33       ; Keyboard
IRQ 2, 34       ; Cascade (used internally)
IRQ 3, 35       ; COM2
IRQ 4, 36       ; COM1
IRQ 5, 37       ; LPT2
IRQ 6, 38       ; Floppy disk
IRQ 7, 39       ; LPT1
IRQ 8, 40       ; CMOS real-time clock
IRQ 9, 41       ; Free
IRQ 10, 42      ; Free
IRQ 11, 43      ; Free
IRQ 12, 44      ; PS/2 Mouse
IRQ 13, 45      ; FPU
IRQ 14, 46      ; Primary ATA
IRQ 15, 47      ; Secondary ATA
