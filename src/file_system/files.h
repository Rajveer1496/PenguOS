#ifndef FILES_H
#define FILES_H

volatile int disk_ready = 0; // a hardware usage variable so volatile tag will tell compiler to dont optimize it away

void disk_init();

#endif