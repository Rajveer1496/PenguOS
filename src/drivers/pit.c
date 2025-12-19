//programmable interval timer

#include<stdint.h>
#include "debug.h"

#define PIT_DATA_PORT 0x40
#define PIT_COMMAND_PORT 0x43
#define BASE_FREQUENCY 1193182 //1.2 million ticks per second

extern void outb(uint16_t port, uint8_t value); // to write data to a port
extern uint8_t inb(uint16_t port); //to read data from port 

uint16_t current_tps;

void setTPS(uint16_t TPS){
    if(TPS > BASE_FREQUENCY){
        serial_print("Error: TPS > Base Frquency of PIT!");
        return;
    }

    current_tps = TPS;

    uint16_t divisor = BASE_FREQUENCY/TPS;

    outb(PIT_COMMAND_PORT,0x36);

    outb(PIT_DATA_PORT,divisor & 0xFF ); //low byte of TPS

    outb(PIT_DATA_PORT, (divisor >> 8) & 0xFF); //High byte of TPS
}