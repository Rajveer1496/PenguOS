#include <stdint.h>

#define VGA_GRAPHICS_MEMORY 0xA0000
#define WIDTH 320
#define HEIGHT 200

//Timer
extern void setTPS(uint16_t TPS);
extern uint32_t timer;
extern uint16_t current_tps;

//---KEYBOARD HANDLER---
int anim_cursor_x = 0;
int anim_cursor_y = 0;

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
void anim_keyboard_handler();

uint8_t *screenBuffer = (uint8_t *)VGA_GRAPHICS_MEMORY;
uint8_t *backBuffer;


//---DRAWING SHAPES---

void draw_Rectangle_solid(int x,int y,int width,int height,uint8_t color){
    for(int i=x;i<=width+x;i++){
        for(int j=y;j<=height+y;j++){
            write_pixel_BackBuffer(i,j,color);
        }
    }
}

void draw_Rectangle_hollow(int x,int y,int width,int height,uint8_t color){
    for(int i=x;i<=width+x;i++){
        write_pixel_BackBuffer(i,y,color);
        write_pixel_BackBuffer(i,y+height,color);
    }
    for(int i=y; i <= height+y;i++){
        write_pixel_BackBuffer(x,i,color);
        write_pixel_BackBuffer(x+width,i,color);
    }
}

void animation(){
    uint32_t last_frame = 0;
    int height = 50;
    int width = 50;
    setTPS(120);
    uint16_t FPS=60; //Animation is drawing "FPS" frames every second
    if(FPS>current_tps){
        serial_print("FPS cannot go faster than TPS!\n");
        return;
    }
    while(1){
    if(timer >= last_frame + (current_tps/FPS)){ 
        vga_clear_backBuffer();
        draw_Rectangle_solid(anim_cursor_x,anim_cursor_y,width,height,0x0D);
        draw_Rectangle_hollow(anim_cursor_x,anim_cursor_y,width,height,0x0E);
        vga_flipBuffer();
        last_frame = timer;
    } 
    }
}

//---HELPER FUNCTIONS----
void vga_draw_init(){
    backBuffer = (uint8_t *)alloc_continous_pages(16); //we need a 64KB buffer

    //Clear BackBuffer!
    vga_clear_backBuffer();

    for(int i=24; i<64 ;i++){
        draw_Rectangle_solid((i-24)*11,0,10,10,i);
    }

    vga_flipBuffer();

    // animation();
}

void vga_flipBuffer(){
    memcpy_fast(backBuffer,screenBuffer,64000);
}

void write_pixel_BackBuffer(int x, int y, uint8_t color){
    if(x<WIDTH && y <HEIGHT && x>=0 && y>=0){ //screen is from 0 to 319 in X
    int index = (WIDTH * y) + x;
    backBuffer[index] = color;
    }
    // else serial_print("Error: Trying to print out of screen!\n");
}

void vga_clear_backBuffer(){
    uint32_t* d = (uint32_t *)backBuffer;
    for(int i=0; i<16000; i++){ //writing 4 bytes at a time
        d[i] =0x0;
    }
}

