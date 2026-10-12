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

void gui_main(){
    serial_print("[GUI]: Starting Desktop\n");
    gui_running = 1;

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
            vbe_flipBuffer();
            last_frame = timer;
        }
    }
}
