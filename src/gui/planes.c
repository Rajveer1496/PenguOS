// create plane (assign number)
// draw to plane using number

#include <stddef.h>
#include "gui.h"
#include "graphics.h"
#include "memory.h"
#include "debug.h"

extern uint16_t VBE_WIDTH;
extern uint16_t VBE_HEIGHT;
extern uint8_t VBE_BYTES_PER_PIXEL;

//array of all planes
struct plane planes[MAX_PLANES];
int planectr = 0;


struct plane * create_plane(int x_pos,int y_pos,int x_size,int y_size){
    struct plane * p = NULL;

    for(int i=0;i<MAX_PLANES;i++){ //find free slot
        if(!planes[i].in_use){
            p = &planes[i];
            break;
        }
    }

    if(p == NULL){
        serial_print("MAX 100 Planes reached in GUI!!\n");
        return NULL;
    }

    p->x_size = x_size;
    p->y_size = y_size;
    p->x_pos = x_pos;
    p->y_pos = y_pos;
    p->Z_POS = planectr;

    // salloc can only give small chunks (~4KB) but planes need way more, so using pages directly
    uint32_t size_bytes = x_size*y_size*VBE_BYTES_PER_PIXEL;
    p->pages = (size_bytes/4096)+1;
    p->buffer = (uint8_t *)alloc_continous_pages(p->pages);
    memInit_fast(p->buffer,p->pages*4096);

    p->in_use = 1;
    planectr++;

    return p;
}

void delete_plane(struct plane * p){
    free_continous_pages(p->buffer,p->pages);
    p->in_use = 0;
    planectr--;
}


void render_plane(){
    for(int i=0 ;i<MAX_PLANES;i++){

    }
}
