#include <stdint.h>
#include "files.h"
#include "memory.h"

// NOTE: for now mapping to use upto 1 TB (Maximum addressable sectors in 32 bit makes up to 2 TB)
// 1 TB = 2097152 sectors
// = 2097152 bit map entries
// = 65536 , 32 bit int
// 256 KB space required to store bit map
// 512 sectors space required to stroe bit map

// here reserving upto 1024 sectors (2 TB)

// FIRST 1024 sectors of disk will be used for bitMap
void initializeDriveBitmap(){
    uint16_t ZeroBuffer[256];
    memInit_fast(ZeroBuffer, 512);

    for(int i=0; i<1024 ; i++){
        write_sector(i, ZeroBuffer);
    }
}

void set_sector_inUse(uint32_t sectorNO){
    // TODO
}

void set_sector_free(uint32_t sectorNO){
    // TODO
}

int check_sector_usage(uint32_t sectorNO){
    // TODO
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