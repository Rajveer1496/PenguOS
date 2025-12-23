#ifndef FILES_H
#define FILES_H

extern volatile int disk_ready; // a hardware usage variable so volatile tag will tell compiler to dont optimize it away

int disk_init();

int read_sector(uint32_t sectorNO, uint16_t * buffer);
int write_sector(uint32_t sectorNO, uint16_t * buffer);


#endif