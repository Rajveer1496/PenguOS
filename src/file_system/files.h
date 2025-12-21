#ifndef FILES_H
#define FILES_H

extern volatile int disk_ready; // a hardware usage variable so volatile tag will tell compiler to dont optimize it away

void disk_init();

#endif