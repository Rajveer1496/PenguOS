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
extern uint8_t *vbe_backBuffer;

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
    p->visible = 1;
    planectr++;

    return p;
}

void delete_plane(struct plane * p){
    free_continous_pages(p->buffer,p->pages);
    p->in_use = 0;
    planectr--;
}


//draw on plane using all vbe draw functions
void plane_draw_start(struct plane * p){
    vbe_set_draw_target(p->buffer,p->x_size,p->y_size);
}

void plane_draw_end(){
    vbe_reset_draw_target();
}

//copy plane to backbuffer row by row (cuts the part which is out of screen)
void paste_plane(struct plane * p){
    int x_start = p->x_pos;
    int x_end = p->x_pos + p->x_size;
    if(x_start < 0) x_start = 0;
    if(x_end > VBE_WIDTH) x_end = VBE_WIDTH;
    if(x_start >= x_end) return; //fully out of screen

    int row_bytes = (x_end - x_start)*VBE_BYTES_PER_PIXEL;

    for(int y=0;y<p->y_size;y++){
        int screen_y = p->y_pos + y;
        if(screen_y < 0) continue;
        if(screen_y >= VBE_HEIGHT) break;

        uint8_t * src = p->buffer + ((y*p->x_size) + (x_start - p->x_pos))*VBE_BYTES_PER_PIXEL;
        uint8_t * dest = vbe_backBuffer + ((screen_y*VBE_WIDTH) + x_start)*VBE_BYTES_PER_PIXEL;
        memcpy_fast(src,dest,row_bytes);
    }
}

//render all planes on backbuffer, lowest Z first so higher Z comes on top
void render_planes(){
    int done = 0;
    int last_z = -1;
    while(done < planectr){
        //find next lowest Z
        struct plane * next = NULL;
        for(int i=0 ;i<MAX_PLANES;i++){
            if(planes[i].in_use && planes[i].Z_POS > last_z){
                if(next == NULL || planes[i].Z_POS < next->Z_POS) next = &planes[i];
            }
        }
        if(next == NULL) break;

        if(next->visible) paste_plane(next);
        last_z = next->Z_POS;
        done++;
    }
}
