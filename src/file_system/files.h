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


#endif