// pic.c - Programmable Interrupt Controller setup

#include <stdint.h>
#include "debug.h"

// PIC ports
#define PIC1_COMMAND 0x20
#define PIC1_DATA    0x21
#define PIC2_COMMAND 0xA0
#define PIC2_DATA    0xA1

// PIC commands
#define ICW1_INIT    0x10
#define ICW1_ICW4    0x01
#define ICW4_8086    0x01
#define PIC_EOI      0x20

// I/O port functions (we'll write these in assembly)
extern void outb(uint16_t port, uint8_t value);
extern uint8_t inb(uint16_t port);


void pic_unmask(uint32_t irqNo);

uint8_t mask1;
uint8_t mask2;

// Remap the PIC to use interrupts 32-47 instead of 0-15
// (We need to remap because 0-15 conflict with CPU exceptions!)
void pic_remap(void) {
    // Save masks
    mask1 = inb(PIC1_DATA);
    mask2 = inb(PIC2_DATA);
    
    // Start initialization sequence
    outb(PIC1_COMMAND, ICW1_INIT | ICW1_ICW4);
    outb(PIC2_COMMAND, ICW1_INIT | ICW1_ICW4);
    
    // Set vector offsets
    outb(PIC1_DATA, 32);  // Master PIC: IRQ 0-7 → interrupts 32-39
    outb(PIC2_DATA, 40);  // Slave PIC: IRQ 8-15 → interrupts 40-47
    
    // Tell Master PIC there's a Slave PIC at IRQ2
    outb(PIC1_DATA, 4);
    
    // Tell Slave PIC its cascade identity
    outb(PIC2_DATA, 2);
    
    // Set mode to 8086
    outb(PIC1_DATA, ICW4_8086);
    outb(PIC2_DATA, ICW4_8086);

    // Restore masks
    outb(PIC1_DATA, mask1);
    outb(PIC2_DATA, mask2);
}

// Send End-Of-Interrupt signal to PIC
void pic_send_eoi(uint8_t irq) {
    // If IRQ came from slave PIC, send EOI to both
    if (irq >= 8) {
        outb(PIC2_COMMAND, PIC_EOI);
    }
    
    // Always send EOI to master
    outb(PIC1_COMMAND, PIC_EOI);
}


/*
Bit = 1 in mask register means IRQ is MASKED (disabled)
Bit = 0 in mask register means IRQ is UNMASKED (enabled)
*/

void pic_unmask(uint32_t irqNo){
    if(irqNo <= 7){
        mask1 = mask1 & ~(1<< irqNo);
        outb(PIC1_DATA, mask1);
    }else if(irqNo >7 && irqNo<=15){
        mask2 = mask2 & ~(1<<(irqNo - 8));
        outb(PIC2_DATA, mask2);
    }else{
        serial_print("ERROR: Bad IRQ Number!\n");
    }
}