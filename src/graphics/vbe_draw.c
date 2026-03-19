#include <stdint.h>
#include "graphics.h"
#include "memory.h"
#include "debug.h"

uint32_t * VBE_MEMORY;
uint8_t * vbe_screenBuffer;
uint8_t *vbe_backBuffer;

uint16_t VBE_WIDTH;
uint16_t VBE_HEIGHT;
uint8_t VBE_BITS_PER_PIXEL;
uint8_t VBE_BYTES_PER_PIXEL;
int vbe_buffer_size_bytes;

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

    serial_print("VBE MODE:\n");
    serial_print("WIDTH: ");
    serial_print_number(VBE_WIDTH);
    serial_print("HEIGHT: ");
    serial_print_number(VBE_HEIGHT);
    serial_print("BYTES_PER_PIXEL: ");
    serial_print_number(VBE_BYTES_PER_PIXEL);

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
    if(x<VBE_WIDTH && y < VBE_HEIGHT && x>=0 && y>=0){
        int index = VBE_BYTES_PER_PIXEL*((VBE_WIDTH * y) + x);
        vbe_backBuffer[index] = RED;
        vbe_backBuffer[index+1] = GREEN;
        vbe_backBuffer[index+2] = BLUE;
    }
    return;
}

void vbe_clear_backBuffer(){
    memInit_fast(vbe_backBuffer,vbe_buffer_size_bytes);
    return;
}