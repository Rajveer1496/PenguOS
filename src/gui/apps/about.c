// About app - basic system info

#include <stddef.h>
#include "apps.h"
#include "graphics.h"
#include "debug.h"

extern uint16_t VBE_WIDTH;
extern uint16_t VBE_HEIGHT;
extern uint8_t VBE_BITS_PER_PIXEL;

extern uint32_t timer;
extern uint16_t current_tps;

struct window * about_window = NULL;
uint32_t about_last_sec = 0;

//number to string, returns length
int number_to_str(uint32_t n,char * str){
    char temp[12];
    int len = 0;
    do{
        temp[len] = '0' + (n%10);
        n /= 10;
        len++;
    }while(n != 0);

    for(int i=0;i<len;i++){ //reverse
        str[i] = temp[len-1-i];
    }
    str[len] = '\0';
    return len;
}

void about_draw(struct window * w){
    char buffer[16];
    window_draw_start(w);
    vbe_draw_rect_solid(0,0,w->width,w->height,230,230,230);

    vbe_draw_string(16,16,"PenguOS",50,90,160);
    vbe_draw_string(16,36,"Made by Rj",0,0,0);

    //resolution
    vbe_draw_string(16,72,"Resolution:",0,0,0);
    int x = 16 + 12*FONT_WIDTH;
    x += number_to_str(VBE_WIDTH,buffer)*FONT_WIDTH;
    vbe_draw_string(16 + 12*FONT_WIDTH,72,buffer,0,0,0);
    vbe_draw_char(x,72,'x',0,0,0);
    x += FONT_WIDTH;
    number_to_str(VBE_HEIGHT,buffer);
    vbe_draw_string(x,72,buffer,0,0,0);

    vbe_draw_string(16,92,"Bits/pixel:",0,0,0);
    number_to_str(VBE_BITS_PER_PIXEL,buffer);
    vbe_draw_string(16 + 12*FONT_WIDTH,92,buffer,0,0,0);

    //uptime
    vbe_draw_string(16,112,"Uptime:",0,0,0);
    int len = number_to_str(timer/current_tps,buffer);
    vbe_draw_string(16 + 12*FONT_WIDTH,112,buffer,0,0,0);
    vbe_draw_string(16 + (13+len)*FONT_WIDTH,112,"sec",0,0,0);

    window_draw_end(w);
}

void about_update(struct window * w){
    if(timer/current_tps != about_last_sec){ //redraw every second for uptime
        about_last_sec = timer/current_tps;
        about_draw(w);
    }
}

void about_close(struct window * w){
    (void)w;
    about_window = NULL;
}

void open_about(){
    if(about_window != NULL){ //already open
        window_focus(about_window);
        return;
    }

    about_window = create_window("About",420,200,300,160);
    if(about_window == NULL) return;

    about_window->on_draw = about_draw;
    about_window->on_update = about_update;
    about_window->on_close = about_close;

    about_draw(about_window);
}
