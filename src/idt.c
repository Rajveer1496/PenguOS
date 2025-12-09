// idt.c - Interrupt Descriptor Table implementation

#include <stdint.h>

// IDT entry structure (8 bytes)
struct idt_entry {
    uint16_t offset_low;    // Lower 16 bits of handler function address
    uint16_t selector;      // Code segment selector (0x10) - GRUB's Kripa
    uint8_t  zero;          // Always zero
    uint8_t  type_attr;     // Type and attributes
    uint16_t offset_high;   // Upper 16 bits of handler function address
} __attribute__((packed));  // Tell compiler: no padding!

// IDT descriptor (pointer to IDT)
struct idt_ptr {
    uint16_t limit;         // Size of IDT - 1
    uint32_t base;          // Address of IDT
} __attribute__((packed));

// The IDT itself (256 entries)
struct idt_entry idt[256];
struct idt_ptr idtp;

// External assembly functions (we'll write these next)
extern void idt_load(uint32_t);

// Set an IDT entry
void idt_set_gate(uint8_t num, uint32_t handler, uint16_t selector, uint8_t flags) {
    idt[num].offset_low = handler & 0xFFFF;         // Lower 16 bits
    idt[num].offset_high = (handler >> 16) & 0xFFFF; // Upper 16 bits
    idt[num].selector = selector;                    // Code segment (0x10)
    idt[num].zero = 0;
    idt[num].type_attr = flags;
}

// External assembly ISR stubs
extern void isr0(void);
extern void isr1(void);
extern void isr2(void);
extern void isr3(void);
extern void isr4(void);
extern void isr5(void);
extern void isr6(void);
extern void isr7(void);
extern void isr8(void);
extern void isr9(void);
extern void isr10(void);
extern void isr11(void);
extern void isr12(void);
extern void isr13(void);
extern void isr14(void);
extern void isr15(void);
extern void isr16(void);
extern void isr17(void);
extern void isr18(void);
extern void isr19(void);
extern void isr20(void);
extern void isr21(void);
extern void isr22(void);
extern void isr23(void);
extern void isr24(void);
extern void isr25(void);
extern void isr26(void);
extern void isr27(void);
extern void isr28(void);
extern void isr29(void);
extern void isr30(void);
extern void isr31(void);

extern void irq0(void);
extern void irq1(void);
extern void irq2(void);
extern void irq3(void);
extern void irq4(void);
extern void irq5(void);
extern void irq6(void);
extern void irq7(void);
extern void irq8(void);
extern void irq9(void);
extern void irq10(void);
extern void irq11(void);
extern void irq12(void);
extern void irq13(void);
extern void irq14(void);
extern void irq15(void);

// Initialize IDT
void idt_init(void) {
    // Set up IDT pointer
    idtp.limit = (sizeof(struct idt_entry) * 256) - 1;
    idtp.base = (uint32_t)&idt;
    
    // Clear IDT (all zeros)
    for (int i = 0; i < 256; i++) {
        idt_set_gate(i, 0, 0, 0);
    }
    
    // Install CPU exception handlers (0-31)
    // Flags: 0x8E = Present, Ring 0, 32-bit Interrupt Gate
    idt_set_gate(0, (uint32_t)isr0, 0x10, 0x8E);
    idt_set_gate(1, (uint32_t)isr1, 0x10, 0x8E);
    idt_set_gate(2, (uint32_t)isr2, 0x10, 0x8E);
    idt_set_gate(3, (uint32_t)isr3, 0x10, 0x8E);
    idt_set_gate(4, (uint32_t)isr4, 0x10, 0x8E);
    idt_set_gate(5, (uint32_t)isr5, 0x10, 0x8E);
    idt_set_gate(6, (uint32_t)isr6, 0x10, 0x8E);
    idt_set_gate(7, (uint32_t)isr7, 0x10, 0x8E);
    idt_set_gate(8, (uint32_t)isr8, 0x10, 0x8E);
    idt_set_gate(9, (uint32_t)isr9, 0x10, 0x8E);
    idt_set_gate(10, (uint32_t)isr10, 0x10, 0x8E);
    idt_set_gate(11, (uint32_t)isr11, 0x10, 0x8E);
    idt_set_gate(12, (uint32_t)isr12, 0x10, 0x8E);
    idt_set_gate(13, (uint32_t)isr13, 0x10, 0x8E);
    idt_set_gate(14, (uint32_t)isr14, 0x10, 0x8E);
    idt_set_gate(15, (uint32_t)isr15, 0x10, 0x8E);
    idt_set_gate(16, (uint32_t)isr16, 0x10, 0x8E);
    idt_set_gate(17, (uint32_t)isr17, 0x10, 0x8E);
    idt_set_gate(18, (uint32_t)isr18, 0x10, 0x8E);
    idt_set_gate(19, (uint32_t)isr19, 0x10, 0x8E);
    idt_set_gate(20, (uint32_t)isr20, 0x10, 0x8E);
    idt_set_gate(21, (uint32_t)isr21, 0x10, 0x8E);
    idt_set_gate(22, (uint32_t)isr22, 0x10, 0x8E);
    idt_set_gate(23, (uint32_t)isr23, 0x10, 0x8E);
    idt_set_gate(24, (uint32_t)isr24, 0x10, 0x8E);
    idt_set_gate(25, (uint32_t)isr25, 0x10, 0x8E);
    idt_set_gate(26, (uint32_t)isr26, 0x10, 0x8E);
    idt_set_gate(27, (uint32_t)isr27, 0x10, 0x8E);
    idt_set_gate(28, (uint32_t)isr28, 0x10, 0x8E);
    idt_set_gate(29, (uint32_t)isr29, 0x10, 0x8E);
    idt_set_gate(30, (uint32_t)isr30, 0x10, 0x8E);
    idt_set_gate(31, (uint32_t)isr31, 0x10, 0x8E);
    
    // Install hardware IRQ handlers (32-47)
    idt_set_gate(32, (uint32_t)irq0, 0x10, 0x8E);
    idt_set_gate(33, (uint32_t)irq1, 0x10, 0x8E);
    idt_set_gate(34, (uint32_t)irq2, 0x10, 0x8E);
    idt_set_gate(35, (uint32_t)irq3, 0x10, 0x8E);
    idt_set_gate(36, (uint32_t)irq4, 0x10, 0x8E);
    idt_set_gate(37, (uint32_t)irq5, 0x10, 0x8E);
    idt_set_gate(38, (uint32_t)irq6, 0x10, 0x8E);
    idt_set_gate(39, (uint32_t)irq7, 0x10, 0x8E);
    idt_set_gate(40, (uint32_t)irq8, 0x10, 0x8E);
    idt_set_gate(41, (uint32_t)irq9, 0x10, 0x8E);
    idt_set_gate(42, (uint32_t)irq10, 0x10, 0x8E);
    idt_set_gate(43, (uint32_t)irq11, 0x10, 0x8E);
    idt_set_gate(44, (uint32_t)irq12, 0x10, 0x8E);
    idt_set_gate(45, (uint32_t)irq13, 0x10, 0x8E);
    idt_set_gate(46, (uint32_t)irq14, 0x10, 0x8E);
    idt_set_gate(47, (uint32_t)irq15, 0x10, 0x8E);
    
    // Load IDT
    idt_load((uint32_t)&idtp);
}

// External keyboard handler
extern void keyboard_handler(void);
extern void pic_send_eoi(uint8_t irq);


extern void vga_putchar(int x, int y, char c, unsigned char color); // for debug
extern void vga_print_hex(int x, int y, uint32_t value); //for debug

// C interrupt handler - called from assembly
void interrupt_handler(uint32_t int_no) {
    
    // DBUG: show interrupt number on screen
    // vga_print_hex(60, 0, int_no); //to show the interrupt number

    // int_no = 33; //DEBUG: but if we purposefully set it the keyboard starts to work, cuz it called keyboard handler
    if (int_no == 33) {  // IRQ 1 = Keyboard (interrupt 33)
            keyboard_handler();
    } 
    // Handle all other hardware IRQs (just acknowledge them)
    else if (int_no >= 32 && int_no < 48 && int_no != 33) {
        pic_send_eoi(int_no - 32);  // Send EOI for unhandled IRQs
    }
    // For CPU exceptions (0-31), do nothing for now

    // DEBUG: show we got here
    // vga_putchar(70, 0, 'K', 0x0F);  // Print 'K' in top-right corner
}

/* WHAT HAPPNES WHEN I PRESS A KEY
okay so when i press key on keyboard, a chip in my keyboard send an interrupt signal to cpu with a 8 bit number that tells which key is pressed/released etc. then upon that interrupt cpu stops whatever it was doing, pushes error code ,interrupt number and return address and jumps to the IDT table according to which interrupt it was and then calls common handler function which stores all state of registers in the stack then passes value of interrupt number to interrupt handler then the interrupt handler in our idt.c runs a function according to the int_no like printing a character on screen according the which key was pressed, then at end it send the EOI signal that we are done with this interrupt and you can send another interrupt, then after that the common handler function restores all registers and cleans up the error_code and int_no by shifting stack pointer to return address and then does iret which shifts cpu to what it was doin
*/