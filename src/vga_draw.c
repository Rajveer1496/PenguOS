#include <stdint.h>

#define VGA_GRAPHICS_MEMORY 0xA0000
#define WIDTH 320
#define HEIGHT 200

//The screenbuffer functions
extern void write_pixel(int x, int y, uint8_t color);

//serial debug functions
extern void serial_print(const char* str);

//The memory management functions
extern void* alloc_continous_pages(uint32_t count);
extern void free_continous_pages(void *address,uint32_t count);
extern void memcpy_fast(void* src, void* dest,uint32_t n);

void write_pixel_BackBuffer(int x, int y, uint8_t color);
void vga_flipBuffer();
void vga_clear_backBuffer();

uint8_t *screenBuffer = (uint8_t *)VGA_GRAPHICS_MEMORY;
uint8_t *backBuffer;

void vga_draw(){
    backBuffer = (uint8_t *)alloc_continous_pages(16); //we need a 64KB buffer

    //Clear BackBuffer!
    vga_clear_backBuffer();

    for(int i=0; i<50;i++){
        write_pixel_BackBuffer(i,30,0x0E);
    }

    vga_flipBuffer();
}

void vga_flipBuffer(){
    memcpy_fast(backBuffer,screenBuffer,64000);
}

void write_pixel_BackBuffer(int x, int y, uint8_t color){
    if(x<=WIDTH && y <=HEIGHT){
    int index = (WIDTH * y) + x;
    backBuffer[index] = color;
    }else serial_print("Error: Trying to print out of screen!\n");
}

void vga_clear_backBuffer(){
    uint32_t* d = (uint32_t *)backBuffer;
    for(int i=0; i<16000; i++){ //writing 4 bytes at a time
        d[i] =0x0;
    }
}