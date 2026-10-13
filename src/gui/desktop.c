// Draw Desktop
// clickable apps
// cursor

// combine planes and draw, with depth

#include "gui.h"
#include "graphics.h"
#include "debug.h"

extern uint16_t VBE_WIDTH;
extern uint16_t VBE_HEIGHT;

//Timer
extern uint32_t timer;
extern uint16_t current_tps;

//Mouse
extern int mouse_x;
extern int mouse_y;

int gui_running = 0;

struct plane * background;

void draw_wallpaper(){
    plane_draw_start(background);

    //vertical gradient (dark blue -> light blue)
    for(int y=0;y<VBE_HEIGHT;y++){
        uint8_t r = 20 + (y*60)/VBE_HEIGHT;
        uint8_t g = 40 + (y*100)/VBE_HEIGHT;
        uint8_t b = 90 + (y*120)/VBE_HEIGHT;
        for(int x=0;x<VBE_WIDTH;x++){
            vbe_write_pixel_BackBuffer(x,y,r,g,b);
        }
    }

    plane_draw_end();
}

///---CURSOR---
#define CURSOR_WIDTH 30
#define CURSOR_HEIGHT 15

//Penguin beak cursor (same as 13h mode cursor in image_helper.c)
//values are 13h colors, 255 = transparent
static const uint8_t cursor_sprite[] = {
56,56,56,56,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
56,56,56,56,56,56,56,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
255,56,56,56,56,56,56,56,56,56,56,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
255,255,255,56,56,56,20,20,56,56,56,56,56,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
255,255,255,255,255,56,56,20,38,38,20,56,56,56,56,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
255,255,255,255,255,255,255,56,56,20,38,38,38,20,56,56,56,56,255,255,255,255,255,255,255,255,255,255,255,255,
255,255,255,255,255,255,255,255,255,56,56,20,20,38,38,38,56,56,56,56,255,255,255,255,255,255,255,255,255,255,
255,255,255,255,255,255,255,255,255,255,56,56,20,38,38,38,38,56,56,56,56,255,255,255,255,255,255,255,255,255,
255,255,255,255,255,255,255,255,255,255,255,56,56,20,38,38,38,38,20,56,56,56,255,255,255,255,255,255,255,255,
255,255,255,255,255,255,255,255,255,255,255,255,56,56,56,20,38,38,38,56,56,56,56,56,255,255,255,255,255,255,
255,255,255,255,255,255,255,255,255,255,255,255,255,56,56,56,56,20,20,20,56,56,56,56,56,255,255,255,255,255,
255,255,255,255,255,255,255,255,255,255,255,255,255,255,56,56,56,56,56,56,56,56,56,56,56,56,255,255,255,255,
255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,56,56,56,56,56,56,56,56,56,56,56,56,255,255,255,
255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,56,56,56,56,56,56,56,56,56,56,56,56,255,255,
255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,56,56,56,56,56,56,56,56,56,56,56,56,255
};

//13h mode colors 0-63 are from EGA pallet, bits: r g b R G B (small = low intensity, capital = high)
//each channel = 2*high + low (0 to 3)
void ega_to_rgb(uint8_t color,uint8_t * RED,uint8_t * GREEN,uint8_t * BLUE){
    *RED = (((color >> 2) & 0x1)*2 + ((color >> 5) & 0x1)) * 85;
    *GREEN = (((color >> 1) & 0x1)*2 + ((color >> 4) & 0x1)) * 85;
    *BLUE = ((color & 0x1)*2 + ((color >> 3) & 0x1)) * 85;
}

void draw_cursor(){ //directly on backbuffer, always on top of everything
    uint8_t r,g,b;
    for(int y=0;y<CURSOR_HEIGHT;y++){
        for(int x=0;x<CURSOR_WIDTH;x++){
            uint8_t c = cursor_sprite[(y*CURSOR_WIDTH) + x];
            if(c == 255) continue; //transparent
            ega_to_rgb(c,&r,&g,&b);
            vbe_write_pixel_BackBuffer(mouse_x+x,mouse_y+y,r,g,b);
        }
    }
}

void gui_main(){
    serial_print("[GUI]: Starting Desktop\n");
    gui_running = 1;
    mouse_x = VBE_WIDTH/2;
    mouse_y = VBE_HEIGHT/2;

    //draw background
    background = create_plane(0,0,VBE_WIDTH,VBE_HEIGHT);
    draw_wallpaper();

    //TEST
    struct plane * test = create_plane(200,150,400,300);
    plane_draw_start(test);
    vbe_draw_rect_solid(0,0,400,300,220,220,220);
    vbe_draw_rect_solid(0,0,400,24,50,90,160);
    vbe_draw_rect_hollow(0,0,400,300,0,0,0);
    plane_draw_end();

    //draw taskbar

    //draw apps

    uint32_t last_frame = 0;
    uint16_t FPS = 60;
    while(1){
        if(timer >= last_frame + (current_tps/FPS)){
            render_planes();
            draw_cursor();
            vbe_flipBuffer();
            last_frame = timer;
        }
    }
}
