#ifndef DRIVERS_H
#define DRIVERS_H

extern uint8_t inb(uint16_t port); //to read data from port (in io.asm)
extern void outb(uint16_t port, uint8_t value); // to write data to a port
extern uint16_t inw(uint16_t port);
extern void outw(uint16_t port, uint16_t value);
extern void pic_unmask(uint32_t irqNo);

//TIMER
extern void setTPS(uint16_t TPS);
extern void wait(uint32_t miliseconds);

#endif