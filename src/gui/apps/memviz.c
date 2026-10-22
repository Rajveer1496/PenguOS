// Memory usage visualizer - shows pages from pmm bitmap (used/free)

#include <stddef.h>
#include "apps.h"
#include "graphics.h"
#include "memory.h"
#include "debug.h"

#define GRID_SIZE 128 //128x128 = 16384 pages = first 64 MB
#define CELL_SIZE 3
#define GRID_X 12
#define GRID_Y 12
#define INFO_X (GRID_X + GRID_SIZE*CELL_SIZE + 16)

extern uint32_t timer;
extern uint16_t current_tps;
extern int number_to_str(uint32_t n,char * str); //about.c

struct window * memviz_window = NULL;
uint32_t memviz_last_sec = 0;
int selected_page = -1;

void hex_to_str(uint32_t n,char * str){
    char hex_chars[] = "0123456789ABCDEF";
    str[0] = '0';
    str[1] = 'x';
    for(int i=7;i>=0;i--){
        str[2 + (7-i)] = hex_chars[(n >> (i*4)) & 0xF];
    }
    str[10] = '\0';
}

void memviz_draw(struct window * w){
    char buffer[16];
    int used = 0;

    window_draw_start(w);
    vbe_draw_rect_solid(0,0,w->width,w->height,230,230,230);

    //grid
    for(int i=0;i<GRID_SIZE*GRID_SIZE;i++){
        int x = GRID_X + (i%GRID_SIZE)*CELL_SIZE;
        int y = GRID_Y + (i/GRID_SIZE)*CELL_SIZE;
        if(check_page_usage(i)){
            used++;
            vbe_draw_rect_solid(x,y,CELL_SIZE,CELL_SIZE,220,60,60);
        }else{
            vbe_draw_rect_solid(x,y,CELL_SIZE,CELL_SIZE,60,170,80);
        }
    }
    vbe_draw_rect_hollow(GRID_X-1,GRID_Y-1,GRID_SIZE*CELL_SIZE+2,GRID_SIZE*CELL_SIZE+2,0,0,0);

    //selected page outline
    if(selected_page >= 0){
        int x = GRID_X + (selected_page%GRID_SIZE)*CELL_SIZE;
        int y = GRID_Y + (selected_page/GRID_SIZE)*CELL_SIZE;
        vbe_draw_rect_hollow(x-2,y-2,CELL_SIZE+4,CELL_SIZE+4,255,255,0);
    }

    //info
    int y = GRID_Y;
    vbe_draw_string(INFO_X,y,"First 64 MB",0,0,0);
    y += 20;
    vbe_draw_string(INFO_X,y,"1 cell = 1 page",0,0,0);
    y += 16;
    vbe_draw_string(INFO_X,y,"(4 KB)",0,0,0);

    y += 32;
    vbe_draw_rect_solid(INFO_X,y+3,10,10,220,60,60);
    vbe_draw_string(INFO_X+16,y,"Used:",0,0,0);
    number_to_str(used,buffer);
    vbe_draw_string(INFO_X+16+6*FONT_WIDTH,y,buffer,0,0,0);

    y += 20;
    vbe_draw_rect_solid(INFO_X,y+3,10,10,60,170,80);
    vbe_draw_string(INFO_X+16,y,"Free:",0,0,0);
    number_to_str(GRID_SIZE*GRID_SIZE - used,buffer);
    vbe_draw_string(INFO_X+16+6*FONT_WIDTH,y,buffer,0,0,0);

    y += 20;
    vbe_draw_string(INFO_X,y,"Used MB:",0,0,0);
    number_to_str((used*4)/1024,buffer);
    vbe_draw_string(INFO_X+9*FONT_WIDTH,y,buffer,0,0,0);

    //selected page info
    if(selected_page >= 0){
        y += 40;
        vbe_draw_string(INFO_X,y,"Page:",0,0,0);
        number_to_str(selected_page,buffer);
        vbe_draw_string(INFO_X+6*FONT_WIDTH,y,buffer,0,0,0);

        y += 20;
        hex_to_str(selected_page*4096,buffer);
        vbe_draw_string(INFO_X,y,buffer,0,0,0);

        y += 20;
        if(check_page_usage(selected_page)) vbe_draw_string(INFO_X,y,"IN USE",220,60,60);
        else vbe_draw_string(INFO_X,y,"FREE",60,170,80);
    }

    window_draw_end(w);
}

void memviz_click(struct window * w,int x,int y){
    int col = (x - GRID_X)/CELL_SIZE;
    int row = (y - GRID_Y)/CELL_SIZE;
    if(x < GRID_X || y < GRID_Y || col >= GRID_SIZE || row >= GRID_SIZE) return;

    selected_page = row*GRID_SIZE + col;
    memviz_draw(w);
}

void memviz_update(struct window * w){
    if(timer/current_tps != memviz_last_sec){ //refresh every second
        memviz_last_sec = timer/current_tps;
        memviz_draw(w);
    }
}

void memviz_close(struct window * w){
    (void)w;
    memviz_window = NULL;
}

void open_memviz(){
    if(memviz_window != NULL){
        window_focus(memviz_window);
        return;
    }

    memviz_window = create_window("Memory Usage",380,150,GRID_X + GRID_SIZE*CELL_SIZE + 180,GRID_SIZE*CELL_SIZE + 2*GRID_Y);
    if(memviz_window == NULL) return;

    memviz_window->on_draw = memviz_draw;
    memviz_window->on_click = memviz_click;
    memviz_window->on_update = memviz_update;
    memviz_window->on_close = memviz_close;

    selected_page = -1;
    memviz_draw(memviz_window);
}
