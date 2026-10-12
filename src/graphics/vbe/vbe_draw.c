#include <stdint.h>
#include "graphics.h"
#include "memory.h"
#include "debug.h"

#define floor(y) ((int)y)
#define intensity(x) ((int)((x)*(255)))

uint32_t * VBE_MEMORY;
uint8_t * vbe_screenBuffer;
uint8_t *vbe_backBuffer;

uint16_t VBE_WIDTH;
uint16_t VBE_HEIGHT;
uint8_t VBE_BITS_PER_PIXEL;
uint8_t VBE_BYTES_PER_PIXEL;
int vbe_buffer_size_bytes;

//Draw target (where all draw functions will draw, backbuffer or any plane buffer)
uint8_t * draw_target;
int draw_target_width;
int draw_target_height;

void vbe_draw_init(){
    VBE_MEMORY = (uint32_t *)(*(uint32_t *)0x500);
    vbe_screenBuffer = (uint8_t *)VBE_MEMORY;

    VBE_WIDTH = (*(uint16_t *)0x505);
    VBE_HEIGHT = (*(uint16_t *)0x507);
    VBE_BITS_PER_PIXEL = (*(uint8_t *)0x509);
    VBE_BYTES_PER_PIXEL = VBE_BITS_PER_PIXEL/8;

    vbe_buffer_size_bytes = (VBE_WIDTH * VBE_HEIGHT) * (VBE_BYTES_PER_PIXEL);
    int vbe_buffer_size_pages = ((vbe_buffer_size_bytes)/4096);
    vbe_backBuffer = (uint8_t *)alloc_continous_pages(vbe_buffer_size_pages);
    vbe_reset_draw_target();

    serial_print("VBE MODE:\n");
    serial_print("WIDTH: ");
    serial_print_number(VBE_WIDTH);
    NEW_LINE
    serial_print("HEIGHT: ");
    serial_print_number(VBE_HEIGHT);
    NEW_LINE
    serial_print("BYTES_PER_PIXEL: ");
    serial_print_number(VBE_BYTES_PER_PIXEL);
    NEW_LINE
    
    // SETTING VBE BUFFER IN USE IN MEMORY
    uint32_t vbe_starting_page = (uint32_t)VBE_MEMORY/4096;
    uint32_t vbe_ending_page = ((uint32_t)VBE_MEMORY+vbe_buffer_size_bytes)/4096;

    for(uint32_t i = (vbe_starting_page-1); i<(vbe_ending_page+1);i++){
        set_page_inUse(i);
    }
    
                                               
    serial_print("VBE Initialisation Complete.\n");
    return;
}

void vbe_flipBuffer(){
    memcpy_fast(vbe_backBuffer,vbe_screenBuffer,vbe_buffer_size_bytes);
    return;
}

void vbe_write_pixel_BackBuffer(int x, int y, uint8_t RED,uint8_t GREEN, uint8_t BLUE){
    if(x<draw_target_width && y < draw_target_height && x>=0 && y>=0){
        int index = VBE_BYTES_PER_PIXEL*((draw_target_width * y) + x);
        //VBE stores pixel as B G R (not R G B)
        draw_target[index] = BLUE;
        draw_target[index+1] = GREEN;
        draw_target[index+2] = RED;
    }
    return;
}

void vbe_set_draw_target(uint8_t * buffer,int width,int height){
    draw_target = buffer;
    draw_target_width = width;
    draw_target_height = height;
}

void vbe_reset_draw_target(){ //back to backbuffer
    draw_target = vbe_backBuffer;
    draw_target_width = VBE_WIDTH;
    draw_target_height = VBE_HEIGHT;
}

void vbe_clear_backBuffer(){
    memInit_fast(vbe_backBuffer,vbe_buffer_size_bytes);
    return;
}

// Shape Draw functions

void vbe_draw_rect_solid(int x,int y,int width,int height,uint8_t RED,uint8_t GREEN, uint8_t BLUE){
    for(int j=y;j<y+height;j++){
        for(int i=x;i<x+width;i++){
            vbe_write_pixel_BackBuffer(i,j,RED,GREEN,BLUE);
        }
    }
}

void vbe_draw_rect_hollow(int x,int y,int width,int height,uint8_t RED,uint8_t GREEN, uint8_t BLUE){
    for(int i=x;i<x+width;i++){
        vbe_write_pixel_BackBuffer(i,y,RED,GREEN,BLUE);
        vbe_write_pixel_BackBuffer(i,y+height-1,RED,GREEN,BLUE);
    }
    for(int j=y;j<y+height;j++){
        vbe_write_pixel_BackBuffer(x,j,RED,GREEN,BLUE);
        vbe_write_pixel_BackBuffer(x+width-1,j,RED,GREEN,BLUE);
    }
}

void vbe_draw_line(int x1, int y1,int x2,int y2, int thickness_px){
    if(x2<x1){ // so we always go from left to right, in any quadrant
        int temp = x1;
        x1 = x2;
        x2 = temp;

        temp = y1;
        y1 = y2;
        y2 = temp;
    }
    
    float gradient = ((float)(y2-y1)/(float)(x2-x1));

    int step_y = (y2>y1) ? 1:(-1); 

    if(gradient>1 || gradient <-1){ //slope steeper than 45 degree
        for(int i = y1; i!=y2; i+=step_y){
            float x_pos = ((i-y1)/gradient) + x1;
            float frac = x_pos - (int)x_pos;

            vbe_write_pixel_BackBuffer(floor(x_pos),i,intensity(1-frac),intensity(1-frac),intensity(1-frac));
            vbe_write_pixel_BackBuffer(floor(x_pos)+1,i,intensity(frac),intensity(frac),intensity(frac));
        }
    }else{//step along side x
        for(int i = x1; i!=x2; i++){
            float y_pos = (gradient*(i - x1)) + y1;
            float frac = y_pos - (int)y_pos;

            vbe_write_pixel_BackBuffer(i,floor(y_pos),intensity(1-frac),intensity(1-frac),intensity(1-frac));
            vbe_write_pixel_BackBuffer(i,floor(y_pos)+1,intensity(frac),intensity(frac),intensity(frac));
        }
    }
}