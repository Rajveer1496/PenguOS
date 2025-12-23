#include <stdint.h>
#include "files.h"
#include "drivers.h"
#include "debug.h"

#define STATUS_COMMAND_PORT 0x1F7
#define SECTOR_COUNT_PORT 0x1F2
#define LBA_1 0x1F3
#define LBA_2 0x1F4
#define LBA_3 0x1F5
#define DRIVE_HEAD_BYTE 0x1F6
#define DATA 0x1F0

volatile int disk_ready = 0;
uint16_t identity_buffer[256]; 
char disk_name[21];
uint32_t total_sectors;

//External variables
extern uint32_t timer; //Timer Counter
extern uint16_t current_tps;

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
    uint32_t local_timer = timer;
    int local_time_out_seconds = 60;
    while(!disk_ready){
        if(timer >= (local_timer + (local_time_out_seconds * current_tps))){
            serial_print("ERROR: Disk not responding!\n");
            return -1;
        }
    };
    disk_ready = 0;

    //check if data is ready
    while(!(inb(STATUS_COMMAND_PORT) & 0x08)); // Bit 3 (DRQ bit) = 1 : data is ready to be transfered
    for(int i=0;i<256;i++){
        buffer[i] = inw(DATA);
    }

    return 0; //sucess
}

//---FUNCTION TO WRITE IN A SECTOR---
int write_sector(uint32_t sectorNO, uint16_t * buffer){
    while(inb(STATUS_COMMAND_PORT) & 0x80); //wait while disk is busy

    outb(SECTOR_COUNT_PORT,1); //write sector count

    outb(LBA_1,(sectorNO & 0xFF));//set LBA 1 = first 8 bits of sector no
    outb(LBA_2,(sectorNO >> 8) & 0xFF);
    outb(LBA_3,(sectorNO >> 16) & 0xFF);

    uint8_t headByte = ((sectorNO >> 24) & 0xF) | (0xE << 4);

    outb(DRIVE_HEAD_BYTE,headByte); // last 4 bits of LBA and FLAGS

    outb(STATUS_COMMAND_PORT,0x30); //write command

    //check if data is ready
    uint32_t local_timer = timer;
    int local_time_out_seconds = 60;
    while(!(inb(STATUS_COMMAND_PORT) & 0x08)){ // Bit 3 (DRQ bit) = 1 : data is ready to be transfered
        if(timer >= (local_timer + (local_time_out_seconds * current_tps))){
            serial_print("ERROR: Disk not responding!\n");
            return -1;
        }
    };

    for(int i=0;i<256;i++){ //write data to disk
        outw(DATA,buffer[i]);
    }

    //wait for the interrupt (disk has completed the operation)
    local_timer = timer;
    local_time_out_seconds = 60;
    while(!disk_ready){
        if(timer >= (local_timer + (local_time_out_seconds * current_tps))){
            serial_print("ERROR: Disk not responding!\n");
            return -1;
        }
    };
    disk_ready = 0;

    //check for error
    if((inb(STATUS_COMMAND_PORT) & 0x01)){
        serial_print("ERROR: Faild to write to disk!\n");
        return -1;
    } 

    return 0; //sucess
}

//---DISK INITIALISATION FUNCTION---
int disk_init(){
    pic_unmask(14);  // Enable ATA interrupts

    while(inb(STATUS_COMMAND_PORT) & 0x80); //wait while disk is busy

    uint8_t headByte = (0xE << 4);
    outb(DRIVE_HEAD_BYTE,headByte); // FLAGS

    uint8_t status = inb(STATUS_COMMAND_PORT);
    if(status == 0xFF || status == 0x0){ //if no disk is connected then all values of port will be 0(ground) or 1(High)
        serial_print("No disk is connected!\n");
        return -1;
    }

    outb(STATUS_COMMAND_PORT,0xEC); //IDENTIFY command

    //wait for the interrupt (disk has completed the operation)
    uint32_t local_timer = timer;
    int local_time_out_seconds = 60;
    while(!disk_ready){ 
        if(timer >= (local_timer + (local_time_out_seconds * current_tps))){
            serial_print("ERROR: Disk not responding!\n");
            return -1;
        }
    };
    disk_ready = 0;

    //Error checking
    if((inb(STATUS_COMMAND_PORT) & 0x01)){
        serial_print("Error while Identifying disk!\n");
        return -1;
    }

    while(!(inb(STATUS_COMMAND_PORT) & 0x08)); //check if asked data is ready or not
    
    for(int i=0;i<256;i++){ //Store the IDENTITY data
        identity_buffer[i] = inw(DATA);
    }

    int local_counter=0;
    for(int i=27;i<46;i++){ //storing disk name (while Un swapping words)
        uint8_t lowerbyte = identity_buffer[i] & 0xFF;
        uint8_t highbyte = (identity_buffer[i]>>8) & 0xFF;
        disk_name[local_counter] = (char)(highbyte);
        local_counter++;
        disk_name[local_counter] = (char)lowerbyte;
        local_counter++;
    }
    disk_name[20] = '\0';

    total_sectors = (identity_buffer[61]<<16) | identity_buffer[60]; //to get amount of sectors in disk
    uint32_t sizeinMB = (total_sectors * 512)/(1024*1024);

    vga_print(10,10,disk_name,0X0E);
    vga_print_hex(10,11,total_sectors);
    vga_print_hex(10,12,sizeinMB);

    serial_print("Disk Name: ");
    serial_print(disk_name);
    serial_print("\n");
    serial_print("Disk initialization Complete!\n");

    return 0; //sucess
}