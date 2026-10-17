// Draw Desktop
// clickable apps
// cursor

// combine planes and draw, with depth

#include "gui.h"
#include "graphics.h"
#include "debug.h"
#include "drivers.h"

extern uint16_t VBE_WIDTH;
extern uint16_t VBE_HEIGHT;

//Timer
extern uint32_t timer;
extern uint16_t current_tps;

//Mouse
extern int mouse_x;
extern int mouse_y;
extern int left_hold;
extern int left_clicked;

int gui_running = 0;

struct plane * background;
struct plane * taskbar;

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

///---TASKBAR---
int last_second = -1;

//two digit number to string
void two_digit(int n,char * str){
    str[0] = '0' + (n/10);
    str[1] = '0' + (n%10);
}

void draw_taskbar(){
    int hour,minute,second;
    rtc_get_time(&hour,&minute,&second);

    if(second == last_second) return; //redraw only once per second
    last_second = second;

    plane_draw_start(taskbar);

    vbe_draw_rect_solid(0,0,taskbar->x_size,TASKBAR_HEIGHT,30,30,40);
    vbe_draw_rect_solid(0,0,taskbar->x_size,1,90,90,110); //top border line

    //PenguOS button
    vbe_draw_rect_solid(4,4,84,TASKBAR_HEIGHT-8,50,90,160);
    vbe_draw_string(14,8,"PenguOS",255,255,255);

    //clock
    char time_str[9] = "00:00:00";
    two_digit(hour,&time_str[0]);
    two_digit(minute,&time_str[3]);
    two_digit(second,&time_str[6]);
    vbe_draw_string(taskbar->x_size - (8*FONT_WIDTH) - 12,8,time_str,255,255,255);

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
    struct window * test = create_window("Test Window",200,150,400,276);
    window_draw_start(test);
    vbe_draw_string(10,16,"Pengu says hello to world!!",0,0,0);
    window_draw_end(test);

    struct window * test2 = create_window("Another one",450,250,300,200);
    window_draw_start(test2);
    vbe_draw_string(10,16,"Click on me",0,0,0);
    window_draw_end(test2);

    //draw taskbar
    taskbar = create_plane(0,VBE_HEIGHT-TASKBAR_HEIGHT,VBE_WIDTH,TASKBAR_HEIGHT);
    taskbar->Z_POS = ALWAYS_ON_TOP;

    //draw apps

    uint32_t last_frame = 0;
    uint16_t FPS = 60;
    while(1){
        if(timer >= last_frame + (current_tps/FPS)){
            //mouse click
            if(left_clicked){
                left_clicked = 0;
                window_click(mouse_x,mouse_y);
            }

            windows_update();
            draw_taskbar();
            render_planes();
            draw_cursor();
            vbe_flipBuffer();
            last_frame = timer;
        }
    }
}
