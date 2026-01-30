#include <stdint.h>
#include "files.h"
#include "memory.h"
#include "debug.h"

// NOTE: for now mapping to use upto 1 TB (Maximum addressable sectors in 32 bit makes up to 2 TB)
// 1 TB = 2097152 sectors
// = 2097152 bit map entries
// = 65536 , 32 bit int
// 256 KB space required to store bit map
// 512 sectors space required to stroe bit map

// here reserving upto 1024 sectors (2 TB)

// FIRST 1024 sectors of disk will be used for bitMap
void initializeDriveBitmap(){
    // uint16_t ZeroBuffer[256];
    uint16_t * ZeroBuffer = (uint16_t *)alloc_page(); //Dont wanted to use stack
    memInit_fast(ZeroBuffer, 4096); // Whole Buffer to 0

// NOTE: idexing of sectors starts from 1
    for(int i=1; i<=1024 ; i++){
        write_sector(i, ZeroBuffer);
    }

    free_page(ZeroBuffer); // Free the buffer
    serial_print("File system initialization complete!\n");
    return;
}


int set_sector_inUse(uint32_t sectorNO){
    uint16_t* buffer = (uint16_t*) alloc_page();

    uint32_t readSector = (sectorNO / 4096) + 1;
    uint32_t remainingBits = sectorNO - (readSector-1)*4096;
    uint32_t readInt = remainingBits / 32;
    uint32_t exactBit = remainingBits % 32;

    if(read_sector(readSector, buffer) != 0){
        serial_print("Error: in set_sector_inUse(), Can't read disk!\n");
        return -1;
    }

    uint32_t * buffer_32 = (uint32_t *) buffer;

    buffer_32[readInt] = (buffer_32[readInt]) | (1<<(32 - exactBit));

    write_sector(readSector, buffer);

    return 0;
}

int set_sector_free(uint32_t sectorNO){
    uint16_t* buffer = (uint16_t*) alloc_page();

    uint32_t readSector = (sectorNO / 4096) + 1;
    uint32_t remainingBits = sectorNO - (readSector-1)*4096;
    uint32_t readInt = remainingBits / 32;
    uint32_t exactBit = remainingBits % 32;

    if(read_sector(readSector, buffer) != 0){
        serial_print("Error: in set_sector_inUse(), Can't read disk!\n");
        return -1;
    }

    uint32_t * buffer_32 = (uint32_t *) buffer;

    buffer_32[readInt] = (buffer_32[readInt]) & ~(1<<(32 - exactBit));

    write_sector(readSector, buffer);

    return 0;
}

int check_sector_usage(uint32_t sectorNO){
    uint16_t* buffer = (uint16_t*) alloc_page();

    uint32_t readSector = (sectorNO / 4096) + 1;;
    uint32_t remainingBits = sectorNO - (readSector-1)*4096;
    uint32_t readInt = remainingBits / 32;
    uint32_t exactBit = remainingBits % 32;

    if(read_sector(readSector, buffer) != 0){
        serial_print("Error: in check_sector_usage(), Can't read disk!\n");
        return -1;
    }

    uint32_t * buffer_32 = (uint32_t *) buffer;

    if(((buffer_32[readInt]) & (1<<(32 - exactBit)))){ // true if not free
        return 1; // not free
    }

    return 0; // is free
}

/*
TODO
Files Metadata format
Storing Files
Loading Files
Deleting Files
Directory concept
*/