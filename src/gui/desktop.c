// Draw Desktop
// clickable apps
// cursor

// combine planes and draw, with depth

#include <stddef.h>
#include "gui.h"
#include "graphics.h"
#include "debug.h"
#include "drivers.h"
#include "apps.h"

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

///---DESKTOP ICONS---
#define ICON_SIZE 48
#define ICON_SPACING 80
#define MAX_ICONS 10

struct desktop_icon{
    char name[16];
    int x;
    int y;
    void (*open)(); //function to open app
    void (*draw)(int x,int y); //icon picture
};

struct desktop_icon icons[MAX_ICONS];
int icon_count = 0;

void add_icon(const char * name,void (*open)(),void (*draw)(int x,int y)){
    if(icon_count >= MAX_ICONS) return;
    struct desktop_icon * icon = &icons[icon_count];

    int i=0;
    for(;name[i] != '\0' && i<15;i++) icon->name[i] = name[i];
    icon->name[i] = '\0';

    //icons in a column on left side
    icon->x = 24;
    icon->y = 20 + icon_count*ICON_SPACING;
    icon->open = open;
    icon->draw = draw;
    icon_count++;
}

//icon pictures
void icon_about(int x,int y){
    vbe_draw_rect_solid(x,y,ICON_SIZE,ICON_SIZE,50,90,160);
    vbe_draw_rect_solid(x+22,y+10,5,5,255,255,255); //dot of i
    vbe_draw_rect_solid(x+22,y+19,5,20,255,255,255);
}

void icon_terminal(int x,int y){
    vbe_draw_rect_solid(x,y,ICON_SIZE,ICON_SIZE,15,15,20);
    vbe_draw_rect_solid(x,y,ICON_SIZE,8,110,110,120); //title bar
    vbe_draw_string(x+6,y+16,">_",80,220,100);
}

void icon_paint(int x,int y){
    vbe_draw_rect_solid(x,y,ICON_SIZE,ICON_SIZE,250,250,250);
    vbe_draw_rect_solid(x+6,y+6,10,10,220,40,40);
    vbe_draw_rect_solid(x+19,y+6,10,10,60,180,75);
    vbe_draw_rect_solid(x+32,y+6,10,10,40,120,220);
    for(int i=0;i<28;i++){ //brush stroke
        vbe_draw_rect_solid(x+8+i,y+30+(i%8 < 4 ? i%4 : 4-(i%4)),3,3,140,70,200);
    }
}

void icon_memory(int x,int y){
    vbe_draw_rect_solid(x,y,ICON_SIZE,ICON_SIZE,230,230,230);
    for(int i=0;i<36;i++){ //6x6 small grid
        int cx = x + 4 + (i%6)*7;
        int cy = y + 4 + (i/6)*7;
        if(i < 15 || i == 20 || i == 27) vbe_draw_rect_solid(cx,cy,6,6,220,60,60);
        else vbe_draw_rect_solid(cx,cy,6,6,60,170,80);
    }
}

void draw_icons(){ //on background plane
    plane_draw_start(background);
    for(int i=0;i<icon_count;i++){
        struct desktop_icon * icon = &icons[i];
        icon->draw(icon->x,icon->y);
        vbe_draw_rect_hollow(icon->x,icon->y,ICON_SIZE,ICON_SIZE,20,20,30);

        //name in center below icon
        int len=0;
        while(icon->name[len] != '\0') len++;
        int name_x = icon->x + (ICON_SIZE/2) - (len*FONT_WIDTH)/2;
        vbe_draw_string(name_x+1,icon->y+ICON_SIZE+5,icon->name,0,0,0); //shadow
        vbe_draw_string(name_x,icon->y+ICON_SIZE+4,icon->name,255,255,255);
    }
    plane_draw_end();
}

int icon_click(int x,int y){ //returns 1 if clicked on icon
    for(int i=0;i<icon_count;i++){
        struct desktop_icon * icon = &icons[i];
        if(x >= icon->x && x < icon->x+ICON_SIZE && y >= icon->y && y < icon->y+ICON_SIZE+20){
            serial_print("[GUI]: opening ");
            serial_print(icon->name);
            NEW_LINE
            icon->open();
            return 1;
        }
    }
    return 0;
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

///---KEYBOARD---
//circular buffer, keyboard interrupt pushes and GUI loop pops
#define KEY_BUFFER_SIZE 64
char key_buffer[KEY_BUFFER_SIZE];
volatile int key_head = 0; //write index
volatile int key_tail = 0; //read index

void gui_key_push(char c){ //called from keyboard interrupt
    int next = (key_head + 1) % KEY_BUFFER_SIZE;
    if(next == key_tail) return; //buffer full, drop key
    key_buffer[key_head] = c;
    key_head = next;
}

int gui_key_pop(char * c){ //returns 0 if no key
    if(key_tail == key_head) return 0;
    *c = key_buffer[key_tail];
    key_tail = (key_tail + 1) % KEY_BUFFER_SIZE;
    return 1;
}

extern struct window * focused_window;

void handle_keys(){
    char c;
    while(gui_key_pop(&c)){
        // serial_print("[GUI]: key\n");
        if(focused_window != NULL && focused_window->on_key != NULL){
            focused_window->on_key(focused_window,c);
        }
    }
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

    //draw apps
    add_icon("Terminal",open_terminal,icon_terminal);
    add_icon("Paint",open_paint,icon_paint);
    add_icon("Memory",open_memviz,icon_memory);
    add_icon("About",open_about,icon_about);
    draw_icons();

    //draw taskbar
    taskbar = create_plane(0,VBE_HEIGHT-TASKBAR_HEIGHT,VBE_WIDTH,TASKBAR_HEIGHT);
    taskbar->Z_POS = ALWAYS_ON_TOP;

    uint32_t last_frame = 0;
    uint16_t FPS = 60;
    while(1){
        if(timer >= last_frame + (current_tps/FPS)){
            //mouse click
            if(left_clicked){
                left_clicked = 0;
                if(window_at(mouse_x,mouse_y) != NULL) window_click(mouse_x,mouse_y);
                else icon_click(mouse_x,mouse_y);
            }
            window_drag(mouse_x,mouse_y);
            handle_keys();

            windows_update();
            draw_taskbar();
            render_planes();
            draw_cursor();
            vbe_flipBuffer();
            last_frame = timer;
        }
    }
}
