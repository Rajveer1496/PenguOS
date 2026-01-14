//programmable interval timer

#include <stdint.h>
#include "debug.h"
// #include "drivers.h"

#define PIT_DATA_PORT 0x40
#define PIT_COMMAND_PORT 0x43
#define BASE_FREQUENCY 1193182 //1.2 million ticks per second

extern void outb(uint16_t port, uint8_t value); // to write data to a port
extern uint8_t inb(uint16_t port); //to read data from port 

extern uint32_t timer; //from idt.c

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

void wait(uint32_t miliseconds){ ///IMPROVE IT SO IT CAN HANDLE LOW TPS
    uint32_t last_frame = timer;

    if(current_tps < (1000/miliseconds)){
        serial_print("Cant wait for that low!, too low TPS!\n");
        //ADD DEFAULT WAIT OF 0.5 SEC
        return;
    }

    while(1){
        if(timer >= last_frame + (current_tps * miliseconds)/1000){
            break;
        }
    }
    return;
}