; idt_load.asm - Load IDT into CPU

section .text
global idt_load

; void idt_load(uint32_t idt_ptr_address)
idt_load:
    mov eax, [esp + 4]     ; Get the pointer to IDT descriptor from stack
    lidt [eax]             ; Load IDT using the LIDT instruction
    ret