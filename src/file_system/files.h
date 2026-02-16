#ifndef FILES_H
#define FILES_H

extern volatile int disk_ready; // a hardware usage variable so volatile tag will tell compiler to dont optimize it away

int disk_init();

extern int read_sector(uint32_t sectorNO, uint16_t * buffer);
extern int write_sector(uint32_t sectorNO, uint16_t * buffer);

// file_system.c
extern void initializeDriveBitmap();
extern int set_sector_inUse(uint32_t sectorNO);
extern int check_sector_usage(uint32_t sectorNO);
extern int set_sector_free(uint32_t sectorNO);
extern int store_file(void * buffer, uint32_t size_bytes, char * name);

//File metadata format
struct file{
    char name[100];
    int size_bytes; //in bytes

}

//My solution #1
/*
How many sectors needed?
store some sectors (for small files)
store info of sector of sectors
512 bytes -> (32 bit int) store info of 127 sectors (128th reserved for sector of sectors of sectors)
if need more sectors
store info of sector of sectors of sector at last 128th sector

store sector info in range format (more efficient)
[(1000,4), (1045,2), (2001,10)] -> (starting sector, no of continous sectors)
*/

/* SOLUTION 2
How many sectors needed?
how big will be sector info?
create file for file to store sector info
store info of sector info file
*/

#endif