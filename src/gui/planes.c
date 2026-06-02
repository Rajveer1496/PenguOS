// create plane (assign number)
// draw to plane using number

#include "graphics.h"
#include "memory.h"
#include "debug.h"

extern uint16_t VBE_WIDTH;
extern uint16_t VBE_HEIGHT;
extern uint8_t VBE_BYTES_PER_PIXEL;

struct plane{
    int x_size;
    int y_size;

    int x_pos;
    int y_pos;

    void * buffer;

    int Z_POS; // for render priority // TODO
};

//array of all planes
void * plane_buffer_array[100]; // TODO: move from stack to heap
int planectr = 0;


void * create_plane(int x_size,int y_size){ //TODO: add plane info
    if(planectr >=100){
        serial_print("MAX 100 Planes reached in GUI!!");
        return NULL;
    }

    plane_buffer_array[planectr] = (uint32_t *)salloc(x_size*y_size*VBE_BYTES_PER_PIXEL);
    planectr++;
    
    return plane_buffer_array[planectr-1];
}


void render_plane(){
    for(int i=0 ;i<planectr;i++){

    }
}