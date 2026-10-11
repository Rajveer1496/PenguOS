#ifndef GUI_H
#define GUI_H

#include <stdint.h>

#define MAX_PLANES 100

struct plane{
    int x_size;
    int y_size;

    int x_pos;
    int y_pos;

    uint8_t * buffer;
    uint32_t pages; //no of pages used by buffer (to free it later)

    int Z_POS; // for render priority // TODO
    int in_use;
};

struct plane * create_plane(int x_pos,int y_pos,int x_size,int y_size);
void delete_plane(struct plane * p);

#endif
