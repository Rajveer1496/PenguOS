#include <stdint.h>
#include "files.h"
#include "drivers.h"

#define STATUS_COMMAND_PORT 0x1F7
#define SECTOR_COUNT_PORT 0x1F2
#define LBA_1 0x1F3
#define LBA_2 0x1F4
#define LBA_3 0x1F5
#define DRIVE_HEAD_BYTE 0x1F6
#define DATA 0x1F0

volatile int disk_ready = 0;

int read_sector(uint32_t sectorNO, uint16_t * buffer){
    while(inb(STATUS_COMMAND_PORT) & 0x80); //wait while disk is busy

    outb(SECTOR_COUNT_PORT,1); //write sector count

    outb(LBA_1,(sectorNO & 0xFF));//set LBA 1 = first 8 bits of sector no
    outb(LBA_2,(sectorNO >> 8) & 0xFF);
    outb(LBA_3,(sectorNO >> 16) & 0xFF);

    uint8_t headByte = ((sectorNO >> 24) & 0xF) | (0xE << 4);

    outb(DRIVE_HEAD_BYTE,headByte); // last 4 bits of LBA and FLAGS

    outb(STATUS_COMMAND_PORT,0x20); //Read command

    //wait for the interrupt (disk has completed the operation)
    // ADD TIMEOUT FEATURE USING TIMERRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRR
    while(!disk_ready);
    disk_ready = 0;

    //check if data is ready
    while(!(inb(STATUS_COMMAND_PORT) & 0x08)); // Bit 3 (DRQ bit) = 1 : data is ready to be transfered
    for(int i=0;i<256;i++){
        buffer[i] = inw(DATA);
    }

    return 0; //sucess
}

void disk_init(){
    pic_unmask(14);  // Enable ATA interrupts
}